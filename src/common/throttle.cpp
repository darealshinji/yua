#include "throttle.h"

Throttle::Throttle(QObject *parent) : QObject(parent) {
        last_timestamp = 0.0;
        break_time_ms = 10.0; //half a frame at 50 fps
        timer.start(); //added 20140125 to correct bug apparently triggered by changing from double to qreal
}

bool Throttle::busy() {
        return (last_timestamp && timer.elapsed() - last_timestamp < break_time_ms);
}

void Throttle::done() {
        last_timestamp = timer.elapsed();
}
