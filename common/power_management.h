#ifndef POWER_MANAGEMENT_H
#define POWER_MANAGEMENT_H

#include <QtGui>

#ifdef Q_OS_LINUX
#include <QtDBus>
#endif
#ifdef Q_OS_MAC
#import <IOKit/pwr_mgt/IOPMLib.h>
#endif
#ifdef Q_OS_WIN32
#include <windows.h>
#endif

class Power_Management {
public:
        static void suspend();
        static void shut_down();
        static void reboot();
        static void hibernate();
        static void lock();
        static const bool verbose = true;

        explicit Power_Management();
#ifdef Q_OS_MAC
        const CFStringRef reasonForActivity;
        IOPMAssertionID assertionID;
#endif
        bool sleep_currently_prevented;
        void prevent_sleep();
        void unprevent_sleep();
};

#endif // POWER_MANAGEMENT_H
