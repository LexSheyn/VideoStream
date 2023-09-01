#ifndef SERVERPLAYER_H
#define SERVERPLAYER_H

#include <QObject>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QMediaPlayer;
class QVideoProbe;
class QVideoFrame;
class QAudioProbe;
class QUdpSocket;
QT_END_NAMESPACE

class ServerPlayer : public QObject
{
    Q_OBJECT;

public:

    explicit ServerPlayer(QObject* parent = nullptr);
    virtual ~ServerPlayer() override;

    void start(const QUrl& mediaUrl);
    void stop();

private slots:

    void onVideoFrameProbed(const QVideoFrame& frame);

private:

    QMediaPlayer* m_player;
    QVideoProbe*  m_videoProbe;
    QAudioProbe*  m_audioProbe;
    QUdpSocket*   m_socket;
    quint32       m_frameIndex;
};

#endif // SERVERPLAYER_H
