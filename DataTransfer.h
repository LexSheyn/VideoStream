#ifndef DATATRANSFER_H
#define DATATRANSFER_H

#include <Qt>
#include <QVideoFrame>

#define VIDEO_ADDRESS     QHostAddress::LocalHost
#define VIDEO_SERVER_PORT 4000
#define VIDEO_CLIENT_PORT 4001

struct alignas(16) VideoPacketHeader
{
    quint32                  frameIndex;
    QVideoFrame::PixelFormat pixelFormat;
    qint32                   width;
    qint32                   height;
    qint32                   bytesPerLine;
    quint32                  packetIndex;
    quint32                  byteCount;
    bool                     b_last;
};

#define UDP_MAX_SIZE 512
#define VIDEO_DATA_SIZE (512 - sizeof(VideoPacketHeader))

#endif // DATATRANSFER_H
