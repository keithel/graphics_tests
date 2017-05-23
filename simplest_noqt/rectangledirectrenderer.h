#ifndef RECTANGLEDIRECTRENDERER_H
#define RECTANGLEDIRECTRENDERER_H

#include <QObject>
#include <QImage>
#include <QPainter>
#include <QColor>

class RectangleDirectRenderer : public QObject
{
    Q_OBJECT
public:
    RectangleDirectRenderer() = delete;
    ~RectangleDirectRenderer();
    static bool injectInstance(RectangleDirectRenderer* injectedInstance);
    static RectangleDirectRenderer* instance(QObject* parent = nullptr);

    void fillAlternatingRectsQPainter(
            const QColor& color1, const QColor& color2,
            const QRectF& rect, unsigned int nTimes,
            unsigned long delay);

// Private Methods
private:
    RectangleDirectRenderer(QObject* parent = nullptr);

// Private members
private:
    static RectangleDirectRenderer *sInstance;
    int mFramebufferFd;
    int mTtyFd;
    QRect mGeometry;
    QImage::Format mFormat;
    QSizeF mPhysicalSize;
    QImage mFramebufferScreenImage;
    int mDepth;
    int mBytesPerLine;
    int mOldTtyMode;
    struct {
        uchar *data;
        int offset, size;
    } mMmap;

    QImage mFbScreenImage;
    QPainter *mBlitter = nullptr;
};

#endif // RECTANGLEDIRECTRENDERER_H
