#include "ServerPlayer.h"

#include "DataTransfer.h"

#include <QMediaPlayer>
#include <QVideoProbe>
#include <QAudioProbe>
#include <QUdpSocket>

ServerPlayer::ServerPlayer(QObject *parent)
    : QObject (parent)
{
    m_player = new QMediaPlayer(this);
    m_player->setAudioRole(QAudio::VideoRole);

    m_videoProbe = new QVideoProbe();
    m_videoProbe->setSource(m_player);

    m_audioProbe = new QAudioProbe();
    m_audioProbe->setSource(m_player);

    m_socket = new QUdpSocket(this);

    connect(m_videoProbe, &QVideoProbe::videoFrameProbed, this, &ServerPlayer::onVideoFrameProbed);
}

ServerPlayer::~ServerPlayer()
{
    this->stop();

    disconnect(m_videoProbe, &QVideoProbe::videoFrameProbed, this, &ServerPlayer::onVideoFrameProbed);
}

void ServerPlayer::start(const QUrl &mediaUrl)
{
    const bool b_binded = m_socket->bind(VIDEO_ADDRESS, VIDEO_SERVER_PORT, QAbstractSocket::ShareAddress);

    m_player->setMedia(mediaUrl);
    m_player->play();
//    m_player->setMuted(true);
}

void ServerPlayer::stop()
{
    m_player->stop();

    m_socket->close();
}

void ServerPlayer::onVideoFrameProbed(const QVideoFrame &frame)
{
    QVideoFrame* frameCopy = new QVideoFrame(frame);

    if (!frameCopy->map(QAbstractVideoBuffer::ReadOnly))
    {
        return;
    }

    QByteArray* datagram = new QByteArray((char*)frameCopy->bits(), frameCopy->mappedBytes());

    const QVideoFrame::PixelFormat pixelFormat  = frameCopy->pixelFormat();
    const qint32                   width        = frameCopy->width();
    const qint32                   height       = frameCopy->height();
    const qint32                   bytesPerLine = frameCopy->bytesPerLine();

    frameCopy->unmap();

    delete frameCopy;

    char* data = datagram->data();

    qint32 byteCount = 0;

    char buffer[UDP_MAX_SIZE];

    quint32 packetIndex = 0;

    for (qint32 i = 0; i < datagram->size(); ++i)
    {
        buffer[byteCount + sizeof(VideoPacketHeader)] = data[i];

        byteCount++;

        if (byteCount == VIDEO_DATA_SIZE)
        {
            VideoPacketHeader* videoPacketHeader = (VideoPacketHeader*)buffer;

            videoPacketHeader->frameIndex   = m_frameIndex;
            videoPacketHeader->pixelFormat  = pixelFormat;
            videoPacketHeader->width        = width;
            videoPacketHeader->height       = height;
            videoPacketHeader->bytesPerLine = bytesPerLine;
            videoPacketHeader->packetIndex  = packetIndex;
            videoPacketHeader->byteCount    = byteCount;
            videoPacketHeader->b_last       = false;

            m_socket->writeDatagram(buffer, sizeof(VideoPacketHeader) + byteCount, VIDEO_ADDRESS, VIDEO_CLIENT_PORT);

            byteCount = 0;

//            memset(buffer, 0, VIDEO_DATA_SIZE);

            packetIndex++;
        }
    }

    if (byteCount > 0)
    {
        VideoPacketHeader* videoPacketHeader = (VideoPacketHeader*)buffer;

        videoPacketHeader->frameIndex   = m_frameIndex;
        videoPacketHeader->pixelFormat  = pixelFormat;
        videoPacketHeader->width        = width;
        videoPacketHeader->height       = height;
        videoPacketHeader->bytesPerLine = bytesPerLine;
        videoPacketHeader->packetIndex  = packetIndex;
        videoPacketHeader->byteCount    = byteCount;
        videoPacketHeader->b_last       = true;

        m_socket->writeDatagram(buffer, sizeof(VideoPacketHeader) + byteCount, VIDEO_ADDRESS, VIDEO_CLIENT_PORT);
    }

    delete datagram;

    m_frameIndex++;
}
