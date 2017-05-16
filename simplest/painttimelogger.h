#ifndef PAINTTIMELOGGER_H
#define PAINTTIMELOGGER_H

#include <QQuickPaintedItem>
#include <QElapsedTimer>
#include <QVector>

class PaintTimeLogger : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qint64 averagePaintTime READ averagePaintTime NOTIFY averagePaintTimeChanged)
    Q_PROPERTY(qint64 lastPaintTime READ lastPaintTime NOTIFY lastPaintTimeChanged)
public:
    PaintTimeLogger(QQuickItem *parent = 0);
    ~PaintTimeLogger();

    qint64 averagePaintTime() { return m_averagePaintTime; }
    qint64 lastPaintTime() { return m_paintTimes[m_paintTimeIdx]; }

    void paint(QPainter *);

signals:
    void averagePaintTimeChanged();
    void lastPaintTimeChanged();

private:
    Q_INVOKABLE void calcAveragePaintTime();

protected:
    QElapsedTimer m_elapsed;
    static const size_t m_numPaintTimes = 30;
    size_t m_paintTimeIdx = 0;
    QVector<qint64> m_paintTimes;
    qint64 m_averagePaintTime = 0;
};

#endif // PAINTTIMELOGGER_H
