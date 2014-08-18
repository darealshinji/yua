#ifndef GTIMER_H
#define GTIMER_H

#include <QObject>
#include <QElapsedTimer>

class GTimer : public QObject {
        Q_OBJECT
public:
        explicit GTimer(QObject *parent = 0);
        void start();
        qreal elapsed();
        qreal restart();
private:
        QElapsedTimer timer;
};

#endif // GTIMER_H
