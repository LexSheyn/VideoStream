#ifndef CLIENTPLAYER_H
#define CLIENTPLAYER_H

#include <QWidget>
#include <QQueue>

QT_BEGIN_NAMESPACE
class QMediaPlayer;
class QVideoProbe;
class QAudioProbe;
class QVideoFrame;
class QUdpSocket;
QT_END_NAMESPACE

class ClientPlayer : public QWidget
{
    Q_OBJECT;

public:

    explicit ClientPlayer(QWidget* parent = nullptr);
    virtual ~ClientPlayer() override;

    void start();
    void stop();

protected:

    // QWidget.

    virtual void paintEvent(QPaintEvent* event) override;

private slots:

    void onReadyRead();

private:

    QMediaPlayer*        m_player;
    QVideoProbe*         m_videoProbe;
    QAudioProbe*         m_audioProbe;
    quint32              m_frameIndex;
    QByteArray           m_frameData;
    QQueue<QVideoFrame>  m_frameQueue;
    QTimer*              m_frameUpdateTimer;
    QUdpSocket*          m_socket;
};

#endif // CLIENTPLAYER_H
