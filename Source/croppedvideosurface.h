#ifndef CROPPEDVIDEOSURFACE_H
#define CROPPEDVIDEOSURFACE_H

#include <QAbstractVideoSurface>
#include <QVideoWidget>

class CroppedVideoSurface : public QVideoWidget, public QAbstractVideoSurface
{
public:
    CroppedVideoSurface(QWidget *widget, QObject *parent = 0);
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType) const;
    bool present(const QVideoFrame &frame);

public:
    bool isFormatSupported(const QVideoSurfaceFormat &format) const;

    bool start(const QVideoSurfaceFormat &format);
    void stop();

    QRect videoRect() const { return targetRect; }
    void updateVideoRect();

    void paint(QPainter *painter);

private:
    QWidget *widget;
    QImage::Format imageFormat;
    QRect targetRect;
    QSize imageSize;
    QRect sourceRect;
};

#endif // CROPPEDVIDEOSURFACE_H
