#include "throttle.h"

Throttle::Throttle(QObject *parent) : QObject(parent) {
        last_timestamp = 0.0;
        break_time_ms = 10.0; //half a frame at 50 fps
}

bool Throttle::busy() {
//        qDebug() << timer.elapsed() - last_timestamp << "elapsed";
        return (last_timestamp && timer.elapsed() - last_timestamp < break_time_ms);
}

void Throttle::done() {
        last_timestamp = timer.elapsed();
}
