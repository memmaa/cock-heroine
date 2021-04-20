#include "croppedvideosurface.h"

#include <QtWidgets>
#include <qabstractvideosurface.h>
#include <qvideosurfaceformat.h>

CroppedVideoSurface::CroppedVideoSurface(QWidget *widget, QObject *parent)
                                         : QAbstractVideoSurface(parent),
                                           widget(widget),
                                           imageFormat(QImage::Format_Invalid)
                                     {
                                     }

QList<QVideoFrame::PixelFormat> CroppedVideoSurface::supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const
{
    Q_UNUSED(handleType);
        return QList<QVideoFrame::PixelFormat>()
            << QVideoFrame::Format_ARGB32
            << QVideoFrame::Format_ARGB32_Premultiplied
            << QVideoFrame::Format_RGB32
            << QVideoFrame::Format_RGB24
            << QVideoFrame::Format_RGB565
            << QVideoFrame::Format_RGB555
            << QVideoFrame::Format_ARGB8565_Premultiplied
            << QVideoFrame::Format_BGRA32
            << QVideoFrame::Format_BGRA32_Premultiplied
            << QVideoFrame::Format_BGR32
            << QVideoFrame::Format_BGR24
            << QVideoFrame::Format_BGR565
            << QVideoFrame::Format_BGR555
            << QVideoFrame::Format_BGRA5658_Premultiplied
            << QVideoFrame::Format_AYUV444
            << QVideoFrame::Format_AYUV444_Premultiplied
            << QVideoFrame::Format_YUV444
            << QVideoFrame::Format_YUV420P
            << QVideoFrame::Format_YV12
            << QVideoFrame::Format_UYVY
            << QVideoFrame::Format_YUYV
            << QVideoFrame::Format_NV12
            << QVideoFrame::Format_NV21
            << QVideoFrame::Format_IMC1
            << QVideoFrame::Format_IMC2
            << QVideoFrame::Format_IMC3
            << QVideoFrame::Format_IMC4
            << QVideoFrame::Format_Y8
            << QVideoFrame::Format_Y16
            << QVideoFrame::Format_Jpeg
            << QVideoFrame::Format_CameraRaw
            << QVideoFrame::Format_AdobeDng;
}

bool CroppedVideoSurface::isFormatSupported(const QVideoSurfaceFormat &format) const
{
    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();

    return imageFormat != QImage::Format_Invalid
            && !size.isEmpty()
            && format.handleType() == QAbstractVideoBuffer::NoHandle;
}

bool CroppedVideoSurface::start(const QVideoSurfaceFormat &format)
{
    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();

    if (imageFormat != QImage::Format_Invalid && !size.isEmpty()) {
        this->imageFormat = imageFormat;
        imageSize = size;
        sourceRect = format.viewport();

        QAbstractVideoSurface::start(format);

        widget->updateGeometry();
        updateVideoRect();

        return true;
    } else {
        return false;
    }
}

void CroppedVideoSurface::stop()
{
    targetRect = QRect();

    QAbstractVideoSurface::stop();

    widget->update();
}

bool CroppedVideoSurface::present(const QVideoFrame &frame)
{
    if (surfaceFormat().pixelFormat() != frame.pixelFormat()
                || surfaceFormat().frameSize() != frame.size()) {
            setError(IncorrectFormatError);
            stop();

            return false;
    }
    int pixelsToCrop = 100;
    QVideoFrame copy(frame);
    if (copy.map(QAbstractVideoBuffer::ReadWrite))
    {
        size_t bytesToBlank = pixelsToCrop * copy.bytesPerLine();
        size_t bytesToKeep = copy.mappedBytes() - bytesToBlank;
        unsigned char * startLocation = copy.bits();
        startLocation += bytesToKeep;
        memset(startLocation, 0, bytesToBlank);
        copy.unmap();

        widget->repaint(targetRect);

        return true;
    }

    return false;
}

void CroppedVideoSurface::updateVideoRect()
{
    QSize size = surfaceFormat().sizeHint();
    size.scale(widget->size().boundedTo(size), Qt::KeepAspectRatio);

    targetRect = QRect(QPoint(0, 0), size);
    targetRect.moveCenter(widget->rect().center());
}
