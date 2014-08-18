#include "gtimer.h"

GTimer::GTimer(QObject *parent) : QObject(parent) {
}

void GTimer::start() {
        return timer.start();
}

qreal GTimer::elapsed() {
        qint64 elapsed_nanoseconds = timer.nsecsElapsed();
        return elapsed_nanoseconds / 1000000.0;
}

qreal GTimer::restart() {
        qreal retval = elapsed();
        timer.start();
        return retval;
}
