#ifndef GTIMER_H
#define GTIMER_H

#include <QObject>
#include <QDebug>


#ifdef Q_OS_WIN32
#include <windows.h>
#endif

#ifdef Q_OS_MAC
#include <mach/mach_time.h>
#include <time.h>
#include <unistd.h>
#endif

#ifdef Q_OS_LINUX
#include <sys/time.h>
#endif


class GTimer : public QObject {
        Q_OBJECT
public:
        explicit GTimer(QObject *parent = 0);
        void start();
        double elapsed();
        double restart();

signals:
        
public slots:
        
private:
#ifdef Q_OS_WIN32
        LARGE_INTEGER freq, startt, endt;
#endif
#ifdef Q_OS_MAC
        uint64_t startt, endt;
        double mach_absolute_difference(uint64_t, uint64_t);
#endif
#ifdef Q_OS_LINUX
        timespec startt;
        double linux_absolute_difference(timespec, timespec);
#endif
};

#endif // GTIMER_H
