#include "ClientPlayer.h"

#include "DataTransfer.h"

#include <QVideoWidget>
#include <QMediaPlayer>
#include <QVideoProbe>
#include <QAudioProbe>
#include <QUdpSocket>
#include <QtWidgets>

ClientPlayer::ClientPlayer(QWidget *parent)
    : QWidget (parent)
{
    m_player = new QMediaPlayer(this, QMediaPlayer::StreamPlayback);
    m_player->setAudioRole(QAudio::VideoRole);
    m_player->setVideoOutput(new QVideoWidget());

    m_videoProbe = new QVideoProbe();
    m_videoProbe->setSource(m_player);

    m_audioProbe = new QAudioProbe();
    m_audioProbe->setSource(m_player);

    m_frameUpdateTimer = new QTimer();

    // TO DO: Implement frame showing slot function.
    // Connect timer timeout signal to the frame showing slot.

    m_socket = new QUdpSocket(this);

    connect(m_socket, &QAbstractSocket::readyRead, this, &ClientPlayer::onReadyRead);
}

ClientPlayer::~ClientPlayer()
{
    this->stop();

    disconnect(m_socket, &QAbstractSocket::readyRead, this, &ClientPlayer::onReadyRead);
}

void ClientPlayer::start()
{
    const bool b_binded = m_socket->bind(VIDEO_ADDRESS, VIDEO_CLIENT_PORT, QAbstractSocket::ShareAddress);

    m_player->setMedia(QMediaContent(), m_socket);
//    m_player->play();
}

void ClientPlayer::stop()
{
    m_socket->close();
}

void ClientPlayer::paintEvent(QPaintEvent *event)
{
    if (!m_frameQueue.isEmpty())
    {
        QVideoFrame frame = m_frameQueue.dequeue();

        QImage image = frame.image();

        QPainter painter;

        painter.drawImage(QRectF(0, 0, image.width(), image.height()), image, event->rect());
    }
}

void ClientPlayer::onReadyRead()
{
    while (m_socket->hasPendingDatagrams())
    {
        QByteArray* datagram = new QByteArray(m_socket->pendingDatagramSize(), Qt::Uninitialized);

        QHostAddress address;
        quint16      port;

        m_socket->readDatagram(datagram->data(), datagram->size(), &address, &port);

        if (!datagram->isEmpty())
        {
            VideoPacketHeader* videoPacketHeader = (VideoPacketHeader*)datagram->data();

            const char* data = (char*)videoPacketHeader + sizeof(VideoPacketHeader);

//            qDebug() << "Frame index:  "   << videoPacketHeader->frameIndex;
//            qDebug() << "Packet index: " << videoPacketHeader->packetIndex;
//            qDebug() << "Byte count:   "    << videoPacketHeader->byteCount;

            if (m_frameData.isEmpty())
            {
                m_frameIndex = videoPacketHeader->frameIndex;
                m_frameData.append(data, videoPacketHeader->byteCount);
            }
            else if (m_frameIndex == videoPacketHeader->frameIndex)
            {
                m_frameData.append(data, videoPacketHeader->byteCount);
            }
            else
            {
                m_frameIndex = videoPacketHeader->frameIndex;

                m_frameData.clear();
                m_frameData.append(data, videoPacketHeader->byteCount);
            }

            if (videoPacketHeader->b_last)
            {
//                m_frameQueue.enqueue(*(QVideoFrame*)m_frameData.data());

                // TO DO: Create frame, map its buffer and copy data into it.
                // How to resize the frame buffer?

                QVideoFrame frame = QVideoFrame(m_frameData.size(), QSize(videoPacketHeader->width, videoPacketHeader->height), videoPacketHeader->bytesPerLine, videoPacketHeader->pixelFormat);

                if (frame.map(QAbstractVideoBuffer::WriteOnly))
                {
                    memcpy((char*)frame.bits(), m_frameData.data(), m_frameData.size());

                    frame.unmap();

                    m_frameQueue.enqueue(frame);
                }

                m_frameData.clear();
            }
        }

        delete datagram;
    }
}
