#ifndef RECTANGLEDIRECTRENDERER_H
#define RECTANGLEDIRECTRENDERER_H

#include <QObject>
#include <QImage>
#include <QPainter>

class RectangleDirectRenderer
{
    Q_OBJECT
public:
    RectangleDirectRenderer() = delete;
    bool injectInstance(RectangleDirectRenderer* injectedInstance);
    RectangleDirectRenderer* instance(QObject* parent = nullptr);

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

    QPainter *mBlitter;
};

#endif // RECTANGLEDIRECTRENDERER_H
