#include <QtWidgets>
#include "rectangle.h"

#if defined(WITH_LTTNG)
#include "hello-tp.h"
#endif

Rectangle::Rectangle(QWidget *parent)
    : QWidget(parent)
{
    m_paintTimes = QVector<qint64>(m_numPaintTimes);

    m_colors.push_back(QColor("lightgrey"));
    m_colors.push_back(QColor("darkgrey"));

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start(100);

    setWindowTitle(tr("Rectangle"));
    resize(1280, 240);
}

void Rectangle::calcAveragePaintTime()
{
    qint64 sum = std::accumulate(m_paintTimes.begin(), m_paintTimes.end(), 0);
    m_averagePaintTime = sum/m_numPaintTimes;
    emit averagePaintTimeChanged();
    qDebug() << "Average paint time: " << m_averagePaintTime << "ns";
}

void Rectangle::paintEvent(QPaintEvent *)
{
    m_elapsed.start();
#if defined(WITH_LTTNG)
    tracepoint(hello_world, my_first_tracepoint,
               m_timer->remainingTime(), const_cast<char*>(qPrintable(m_colors[m_colorIdx].name())));
#endif

    // Do the work to paint the rectangle
    QPainter painter(this);
    m_colorIdx = (m_colorIdx + 1) % m_colors.length();
//    qDebug() << "Painting rect of size" << this->rect() << "with color" << m_colors[m_colorIdx];
    painter.fillRect(this->rect(), m_colors[m_colorIdx]);

    // Figure out timing.
    m_paintTimes[m_paintTimeIdx] = m_elapsed.nsecsElapsed();
//    qDebug() << "last paint time: " << m_paintTimes[m_paintTimeIdx];
    m_paintTimeIdx++;
    m_elapsed.invalidate();
    emit lastPaintTimeChanged();

    if (m_paintTimeIdx >= m_numPaintTimes)
    {
        m_paintTimeIdx = 0;
//        calcAveragePaintTime();
        QMetaObject::invokeMethod(this, "calcAveragePaintTime", Qt::QueuedConnection);
    }
}
