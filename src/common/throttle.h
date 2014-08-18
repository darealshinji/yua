#ifndef THROTTLE_H
#define THROTTLE_H

#include <QObject>
#include <QDebug>

#include "gtimer.h"

class Throttle : public QObject {
        Q_OBJECT
public:
        explicit Throttle(QObject *parent = 0);
        bool busy();
        void done();
        qreal break_time_ms;
        
private:
        GTimer timer;
        qreal last_timestamp;
};

#endif // THROTTLE_H
