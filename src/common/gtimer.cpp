#include "gtimer.h"

#ifdef Q_OS_MAC
double GTimer::mach_absolute_difference(uint64_t end, uint64_t start) {
        struct timespec tp;

        uint64_t difference = end - start;
        static mach_timebase_info_data_t info = {0,0};

        if (info.denom == 0)
                mach_timebase_info(&info);

        uint64_t elapsednano = difference * (info.numer / info.denom);

        tp.tv_sec = elapsednano * 1e-9;
        tp.tv_nsec = elapsednano - (tp.tv_sec * 1e9);

        return ((double)tp.tv_sec * 1000.0) + ((double)tp.tv_nsec / 1000000.0);

        //printf("%lu seconds, %lu nanoseconds\n", tp.tv_sec, tp.tv_nsec);

}
#endif

#ifdef Q_OS_LINUX
double GTimer::linux_absolute_difference(timespec endt, timespec start) { //from http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
        struct timespec temp;

        if ((endt.tv_nsec-start.tv_nsec)<0) {
                temp.tv_sec = endt.tv_sec-start.tv_sec-1;
                temp.tv_nsec = 1000000000+endt.tv_nsec-start.tv_nsec;
        } else {
                temp.tv_sec = endt.tv_sec-start.tv_sec;
                temp.tv_nsec = endt.tv_nsec-start.tv_nsec;
        }

        return ((double)temp.tv_sec * 1000.0) + ((double)temp.tv_nsec / 1000000.0);
}
#endif



GTimer::GTimer(QObject *parent) : QObject(parent) {
#ifdef Q_OS_WIN32
        QueryPerformanceFrequency(&freq);
#endif
}

void GTimer::start() {
#ifdef Q_OS_WIN32
        QueryPerformanceCounter(&startt);
#endif

#ifdef Q_OS_MAC
        startt = mach_absolute_time();
#endif

#ifdef Q_OS_LINUX
        clock_gettime(CLOCK_MONOTONIC, &startt);
#endif
}

double GTimer::elapsed() {
#ifdef Q_OS_WIN32
        QueryPerformanceCounter(&endt);
        double retval = (double(endt.QuadPart - startt.QuadPart) / freq.QuadPart) * 1000.0;
        return retval;
#endif

#ifdef Q_OS_MAC
        endt = mach_absolute_time();
        return mach_absolute_difference(endt, startt);
#endif

#ifdef Q_OS_LINUX
        timespec endt;
        clock_gettime(CLOCK_MONOTONIC, &endt);
        return linux_absolute_difference(endt, startt);
#endif
}

double GTimer::restart() {
#ifdef Q_OS_WIN32
        QueryPerformanceCounter(&endt);
        double retval = (double(endt.QuadPart - startt.QuadPart) / freq.QuadPart) * 1000.0;
        startt = endt;
        return retval;
#endif

#ifdef Q_OS_MAC
        endt = mach_absolute_time();
        double retval = mach_absolute_difference(endt, startt);
        startt = endt;
        return retval;
#endif

#ifdef Q_OS_LINUX
        timespec endt;
        clock_gettime(CLOCK_MONOTONIC, &endt);
        double retval = linux_absolute_difference(endt, startt);
        startt = endt;
        return retval;
#endif
}
