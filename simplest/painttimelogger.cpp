#include "painttimelogger.h"
#include <algorithm>

PaintTimeLogger::PaintTimeLogger(QQuickItem *parent): QQuickPaintedItem(parent)
{
    m_paintTimes = QVector<qint64>(m_numPaintTimes);
    setFlag(QQuickItem::ItemHasContents);
}

PaintTimeLogger::~PaintTimeLogger()
{
}

void PaintTimeLogger::calcAveragePaintTime()
{
    qint64 sum = std::accumulate(m_paintTimes.begin(), m_paintTimes.end(), 0);
    m_averagePaintTime = sum/m_numPaintTimes;
    emit averagePaintTimeChanged();
    qDebug() << "Average paint time: " << m_averagePaintTime << "ns";
}

void PaintTimeLogger::paint(QPainter *painter)
{
    Q_UNUSED(painter);
    m_elapsed.start();
    update();
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
