#include "rectangledirectrenderer.h"

#include <QString>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <QRect>
#include <QDebug>
#include <QSizeF>
#include <limits.h>
#include <QFile>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <sys/mman.h>

#include <QThread>
#include <QCoreApplication>
#include <QTimer>

RectangleDirectRenderer* RectangleDirectRenderer::sInstance = nullptr;

// Some of this code lifted from QLinuxFbScreen
static int openFramebufferDevice(const QString &dev)
{
    int fd = -1;

    if (access(dev.toLatin1().constData(), R_OK|W_OK) == 0)
        fd = open64(dev.toLatin1().constData(), O_RDWR);

    if (fd == -1) {
        if (access(dev.toLatin1().constData(), R_OK) == 0)
            fd = open64(dev.toLatin1().constData(), O_RDONLY);
    }

    return fd;
}

static int determineDepth(const fb_var_screeninfo &vinfo)
{
    int depth = vinfo.bits_per_pixel;
    if (depth== 24) {
        depth = vinfo.red.length + vinfo.green.length + vinfo.blue.length;
        if (depth <= 0)
            depth = 24; // reset if color component lengths are not reported
    } else if (depth == 16) {
        depth = vinfo.red.length + vinfo.green.length + vinfo.blue.length;
        if (depth <= 0)
            depth = 16;
    }
    return depth;
}

static int openTtyDevice(const QString &device)
{
    const char *const devs[] = { "/dev/tty0", "/dev/tty", "/dev/console", 0 };

    int fd = -1;
    if (device.isEmpty()) {
        for (const char * const *dev = devs; *dev; ++dev) {
            fd = open64(*dev, O_RDWR);
            if (fd != -1)
                break;
        }
    } else {
        fd = open64(QFile::encodeName(device).constData(), O_RDWR);
    }

    return fd;
}

static void switchToGraphicsMode(int ttyfd, int *oldMode)
{
    // Do not warn if the switch fails: the ioctl fails when launching from a
    // remote console and there is nothing we can do about it.  The matching
    // call in resetTty should at least fail then, too, so we do no harm.
    if (ioctl(ttyfd, KDGETMODE, oldMode) == 0) {
        if (*oldMode != KD_GRAPHICS)
            ioctl(ttyfd, KDSETMODE, KD_GRAPHICS);
    }
}

static void resetTty(int ttyfd, int oldMode)
{
    ioctl(ttyfd, KDSETMODE, oldMode);

    close(ttyfd);
}

static void blankScreen(int fd, bool on)
{
    ioctl(fd, FBIOBLANK, on ? VESA_POWERDOWN : VESA_NO_BLANKING);
}

static QImage::Format determineFormat(const fb_var_screeninfo &info, int depth)
{
    const fb_bitfield rgba[4] = { info.red, info.green,
                                  info.blue, info.transp };

    QImage::Format format = QImage::Format_Invalid;

    switch (depth) {
    case 32: {
        const fb_bitfield argb8888[4] = {{16, 8, 0}, {8, 8, 0},
                                         {0, 8, 0}, {24, 8, 0}};
        const fb_bitfield abgr8888[4] = {{0, 8, 0}, {8, 8, 0},
                                         {16, 8, 0}, {24, 8, 0}};
        if (memcmp(rgba, argb8888, 4 * sizeof(fb_bitfield)) == 0) {
            format = QImage::Format_ARGB32;
        } else if (memcmp(rgba, argb8888, 3 * sizeof(fb_bitfield)) == 0) {
            format = QImage::Format_RGB32;
        } else if (memcmp(rgba, abgr8888, 3 * sizeof(fb_bitfield)) == 0) {
            format = QImage::Format_RGB32;
            // pixeltype = BGRPixel;
        }
        break;
    }
    case 24: {
        const fb_bitfield rgb888[4] = {{16, 8, 0}, {8, 8, 0},
                                       {0, 8, 0}, {0, 0, 0}};
        const fb_bitfield bgr888[4] = {{0, 8, 0}, {8, 8, 0},
                                       {16, 8, 0}, {0, 0, 0}};
        if (memcmp(rgba, rgb888, 3 * sizeof(fb_bitfield)) == 0) {
            format = QImage::Format_RGB888;
        } else if (memcmp(rgba, bgr888, 3 * sizeof(fb_bitfield)) == 0) {
            format = QImage::Format_RGB888;
            // pixeltype = BGRPixel;
        }
        break;
    }
    case 18: {
        const fb_bitfield rgb666[4] = {{12, 6, 0}, {6, 6, 0},
                                       {0, 6, 0}, {0, 0, 0}};
        if (memcmp(rgba, rgb666, 3 * sizeof(fb_bitfield)) == 0)
            format = QImage::Format_RGB666;
        break;
    }
    case 16: {
        const fb_bitfield rgb565[4] = {{11, 5, 0}, {5, 6, 0},
                                       {0, 5, 0}, {0, 0, 0}};
        const fb_bitfield bgr565[4] = {{0, 5, 0}, {5, 6, 0},
                                       {11, 5, 0}, {0, 0, 0}};
        if (memcmp(rgba, rgb565, 3 * sizeof(fb_bitfield)) == 0) {
            format = QImage::Format_RGB16;
        } else if (memcmp(rgba, bgr565, 3 * sizeof(fb_bitfield)) == 0) {
            format = QImage::Format_RGB16;
            // pixeltype = BGRPixel;
        }
        break;
    }
    case 15: {
        const fb_bitfield rgb1555[4] = {{10, 5, 0}, {5, 5, 0},
                                        {0, 5, 0}, {15, 1, 0}};
        const fb_bitfield bgr1555[4] = {{0, 5, 0}, {5, 5, 0},
                                        {10, 5, 0}, {15, 1, 0}};
        if (memcmp(rgba, rgb1555, 3 * sizeof(fb_bitfield)) == 0) {
            format = QImage::Format_RGB555;
        } else if (memcmp(rgba, bgr1555, 3 * sizeof(fb_bitfield)) == 0) {
            format = QImage::Format_RGB555;
            // pixeltype = BGRPixel;
        }
        break;
    }
    case 12: {
        const fb_bitfield rgb444[4] = {{8, 4, 0}, {4, 4, 0},
                                       {0, 4, 0}, {0, 0, 0}};
        if (memcmp(rgba, rgb444, 3 * sizeof(fb_bitfield)) == 0)
            format = QImage::Format_RGB444;
        break;
    }
    case 8:
        break;
    case 1:
        format = QImage::Format_Mono; //###: LSB???
        break;
    default:
        break;
    }

    return format;
}

RectangleDirectRenderer::RectangleDirectRenderer(QObject *parent)
    : QObject(parent)
{
    QLatin1String fbDevice("/dev/fb0");
    QLatin1String ttyDevice("");

    mFramebufferFd = openFramebufferDevice(fbDevice);
    if (mFramebufferFd == -1)
        qErrnoWarning(errno, "Failed to open framebuffer %s", qPrintable(fbDevice));
    Q_ASSERT_X((mFramebufferFd != -1), "RectangleDirectRenderer::RectangleDirectRenderer", "Failed to open framebuffer");

    fb_fix_screeninfo fixScreeninfo;
    fb_var_screeninfo varScreeninfo;
    memset(&fixScreeninfo, 0, sizeof(fixScreeninfo));
    memset(&varScreeninfo, 0, sizeof(varScreeninfo));

    if (ioctl(mFramebufferFd, FBIOGET_FSCREENINFO, &fixScreeninfo) != 0) {
        qErrnoWarning(errno, "Error reading fixed information");
        Q_ASSERT_X(false, "RectangleDirectRenderer::RectangleDirectRenderer", "Error reading FB fixed information");
    }
    if (ioctl(mFramebufferFd, FBIOGET_VSCREENINFO, &varScreeninfo) != 0) {
        qErrnoWarning(errno, "Error reading variable information");
        Q_ASSERT_X(false, "RectangleDirectRenderer::RectangleDirectRenderer", "Error reading FB variable information");
    }

    mDepth = determineDepth(varScreeninfo);
    mBytesPerLine = fixScreeninfo.line_length;
    QRect geometry(varScreeninfo.xoffset, varScreeninfo.yoffset, varScreeninfo.xres, varScreeninfo.yres);
    mGeometry = QRect(QPoint(0, 0), geometry.size());
    mFormat = determineFormat(varScreeninfo, mDepth);
    mPhysicalSize = QSizeF(qRound(geometry.width()*25.4/100), qRound(geometry.height()*25.4/100));

    mMmap.size = fixScreeninfo.smem_len;
    uchar *data = (unsigned char*)mmap(0, mMmap.size, PROT_READ | PROT_WRITE, MAP_SHARED, mFramebufferFd, 0);
    if ((long)data == -1) {
        qErrnoWarning(errno, "Failed to mmap framebuffer");
    }
    Q_ASSERT_X((long)data != -1, "RectangleDirectRenderer::RectangleDirectRenderer", "Failed to mmap framebuffer");

    mMmap.offset = geometry.y() * mBytesPerLine + geometry.x() * mDepth / 8;
    mMmap.data = data + mMmap.offset;

    mFbScreenImage = QImage(mMmap.data, geometry.width(), geometry.height(), mBytesPerLine, mFormat);

    mTtyFd = openTtyDevice(ttyDevice);
    if (mTtyFd == -1) {
        qErrnoWarning(errno, "Failed to open tty");
    }

    switchToGraphicsMode(mTtyFd, &mOldTtyMode);
    blankScreen(mFramebufferFd, false);
}

RectangleDirectRenderer::~RectangleDirectRenderer() {
    if (mFramebufferFd != -1) {
        if (mMmap.data)
            munmap(mMmap.data - mMmap.offset, mMmap.size);
        close(mFramebufferFd);
    }

    if (mTtyFd != -1)
        resetTty(mTtyFd, mOldTtyMode);

    delete mBlitter;

    QTimer::singleShot(0, QCoreApplication::instance(), []
    {
        qDebug() << "Quitting.";
        QCoreApplication::instance()->quit();
    });
}

bool RectangleDirectRenderer::injectInstance(RectangleDirectRenderer *injectedInstance)
{
    if (sInstance == nullptr)
        sInstance = injectedInstance;
    return (sInstance == injectedInstance);
}

RectangleDirectRenderer* RectangleDirectRenderer::instance(QObject* parent)
{
    if (sInstance == nullptr)
        sInstance = new RectangleDirectRenderer(parent);
    return sInstance;
}

void RectangleDirectRenderer::fillAlternatingRectsQPainter(const QColor &color1, const QColor &color2,
                                                           const QRectF &rect, unsigned int nTimes,
                                                           unsigned long delay)
{
    if (mBlitter == nullptr)
        mBlitter = new QPainter(&mFbScreenImage);


    mBlitter->setCompositionMode(QPainter::CompositionMode_Source);

    for (unsigned int i = 0; nTimes == 0 || i < nTimes; i=i+2)
    {
        mBlitter->fillRect(rect, color1);
        QThread::msleep(delay);
        mBlitter->fillRect(rect, color2);
        QThread::msleep(delay);
    }

    delete this;
}
