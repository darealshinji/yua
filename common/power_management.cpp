#include "power_management.h"

Power_Management::Power_Management()
#ifdef Q_OS_MAC
        : reasonForActivity(CFSTR("Encoding video"))
        , sleep_currently_prevented(false)
#endif
{
}

void Power_Management::prevent_sleep() {
        if (sleep_currently_prevented) return;
#ifdef Q_OS_MAC
        //http://developer.apple.com/library/mac/#qa/qa1340/_index.html
        IOReturn success = IOPMAssertionCreateWithName(kIOPMAssertionTypeNoIdleSleep,
                                            kIOPMAssertionLevelOn, reasonForActivity, &assertionID);
        if (success != kIOReturnSuccess) {
                qDebug() << this << "prevent_sleep() failed:" << success;
                return;
        }
#endif
#ifdef Q_OS_WIN32
        //http://msdn.microsoft.com/en-us/library/windows/desktop/aa373208%28v=vs.85%29.aspx
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
#endif
#ifdef Q_OS_LINUX
        qDebug() << this << "unprevent_sleep(): not implemented";
        return;
#endif
        sleep_currently_prevented = true;
}

void Power_Management::unprevent_sleep() {
        if (!sleep_currently_prevented) return;
#ifdef Q_OS_MAC
        IOReturn success = IOPMAssertionRelease(assertionID);
        if (success != kIOReturnSuccess) {
                qDebug() << this << "unprevent_sleep() failed:" << success;
                return;
        }
#endif
#ifdef Q_OS_WIN32
        SetThreadExecutionState(ES_CONTINUOUS);
#endif
#ifdef Q_OS_LINUX
        qDebug() << this << "unprevent_sleep(): not implemented";
        return;
#endif
        sleep_currently_prevented = false;
}

#ifdef Q_OS_MAC
void Power_Management::suspend() {
        QProcess::startDetached("pmset", QStringList() << "sleepnow");
}

void Power_Management::shut_down() {
        QStringList args;
        args << "-e";
        args << "tell application \"Finder\" to shut down";
        QProcess::startDetached("osascript", args);
}

void Power_Management::reboot() {
        QStringList args;
        args << "-e";
        args << "tell application \"Finder\" to restart";
        QProcess::startDetached("osascript", args);
}

void Power_Management::hibernate() {
        return suspend(); //can't change the sleep mode without admin privileges (20130501)
}

void Power_Management::lock() {
        QProcess::startDetached("/System/Library/CoreServices/Menu Extras/User.menu/Contents/Resources/CGSession", QStringList() << "-suspend");
}
#endif


#ifdef Q_OS_WIN32
void Power_Management::suspend() {
        QProcess::startDetached("rundll32.exe powrprof.dll,SetSuspendState 0,1,0"); //apparently this hiberates rather than sleeps if hibernation is not disabled (20130501)
}

void Power_Management::shut_down() {
        QProcess::startDetached("shutdown -s -t 00");
}

void Power_Management::reboot() {
        QProcess::startDetached("shutdown -r -t 00");
}

void Power_Management::hibernate() {
        QProcess::startDetached("rundll32.exe PowrProf.dll,SetSuspendState");
}

void Power_Management::lock() {
        QProcess::startDetached("rundll32.exe user32.dll,LockWorkStation");
}
#endif


#ifdef Q_OS_LINUX //from http://qt-project.org/forums/viewthread/5957
void Power_Management::suspend() {
        bool gnome_power1 = false;
        bool gnome_power2 = false;
        bool hal_works = false;
        QDBusMessage response;

        gnome_power1 = QProcess::startDetached("gnome-power-cmd.sh suspend");
        gnome_power2 = QProcess::startDetached("gnome-power-cmd suspend");
        if(!gnome_power1 && !gnome_power2 && verbose)
                qWarning() << "W: gnome-power-cmd and gnome-power-cmd.sh didn't work";

        if(!gnome_power1 && !gnome_power2){
                QDBusInterface powermanagement("org.freedesktop.Hal",
                                               "/org/freedesktop/Hal/devices/computer",
                                               "org.freedesktop.Hal.Device.SystemPowerManagement",
                                               QDBusConnection::systemBus());
                response = powermanagement.call("Suspend",0);
                if(response.type() == QDBusMessage::ErrorMessage){
                        if(verbose)
                                qWarning() << "W: " << response.errorName() << ":" << response.errorMessage();
                }
                else
                        hal_works = true;
        }

        if(!hal_works && !gnome_power1 && !gnome_power2){
                QDBusInterface powermanagement("org.freedesktop.DeviceKit.Power",
                                               "/org/freedesktop/DeviceKit/Power",
                                               "org.freedesktop.DeviceKit.Power", QDBusConnection::systemBus());
                response = powermanagement.call("Suspend");
                if(response.type() == QDBusMessage::ErrorMessage){
                        if(verbose)
                                qWarning() << "W: " << response.errorName() << ":" << response.errorMessage();
                }
        }
}

void Power_Management::shut_down() {
        bool shutdown_works = false;
        bool gnome_power1 = false;
        bool gnome_power2 = false;
        bool hal_works = false;
        QDBusMessage response;

        QDBusInterface gnomeSessionManager("org.gnome.SessionManager",
                                           "/org/gnome/SessionManager", "org.gnome.SessionManager",
                                           QDBusConnection::sessionBus());
        response = gnomeSessionManager.call("RequestShutdown");
        if(response.type() == QDBusMessage::ErrorMessage){
                if(verbose)
                        qWarning() << "W: " << response.errorName() << ":" << response.errorMessage();
                gnome_power1 = QProcess::startDetached("gnome-power-cmd.sh shutdown");
                gnome_power2 = QProcess::startDetached("gnome-power-cmd shutdown");
                if(verbose && !gnome_power1 && !gnome_power2)
                        qWarning() << "W: gnome-power-cmd and gnome-power-cmd.sh didn't work";
        }
        else
                shutdown_works = true;

        QDBusInterface kdeSessionManager("org.kde.ksmserver", "/KSMServer",
                                         "org.kde.KSMServerInterface", QDBusConnection::sessionBus());
        response = kdeSessionManager.call("logout", 0, 2, 2);
        if(response.type() == QDBusMessage::ErrorMessage){
                if(verbose)
                        qWarning() << "W: " << response.errorName() << ":" << response.errorMessage();
        }
        else
                shutdown_works = true;

        if(!shutdown_works && !gnome_power1 && !gnome_power2){
                QDBusInterface powermanagement("org.freedesktop.Hal",
                                               "/org/freedesktop/Hal/devices/computer",
                                               "org.freedesktop.Hal.Device.SystemPowerManagement",
                                               QDBusConnection::systemBus());
                response = powermanagement.call("Shutdown");
                if(response.type() == QDBusMessage::ErrorMessage){
                        if(verbose)
                                qWarning() << "W: " << response.errorName() << ":" << response.errorMessage();
                }
                else
                        hal_works = true;
        }

        if(!hal_works && !shutdown_works && !gnome_power1 && !gnome_power2){
                QDBusInterface powermanagement("org.freedesktop.ConsoleKit",
                                               "/org/freedesktop/ConsoleKit/Manager", "org.freedesktop.ConsoleKit.Manager",
                                               QDBusConnection::systemBus());
                response = powermanagement.call("Stop");
                if(response.type() == QDBusMessage::ErrorMessage){
                        if(verbose)
                                qWarning() << "W: " << response.errorName() << ":" << response.errorMessage();
                        QProcess::startDetached("sudo shutdown -P now");
                }
        }
}

void Power_Management::reboot() {
        bool reboot_works = false;
        bool gnome_power1 = false;
        bool gnome_power2 = false;
        bool hal_works = false;
        QDBusMessage response;

        QDBusInterface gnomeSessionManager("org.gnome.SessionManager",
                                           "/org/gnome/SessionManager", "org.gnome.SessionManager",
                                           QDBusConnection::sessionBus());
        response = gnomeSessionManager.call("RequestReboot");
        if(response.type() == QDBusMessage::ErrorMessage){
                if(verbose)
                        qWarning() << "W: " << response.errorName() << ":" << response.errorMessage();
                gnome_power1 = QProcess::startDetached("gnome-power-cmd.sh reboot");
                gnome_power2 = QProcess::startDetached("gnome-power-cmd reboot");
                if(verbose && !gnome_power1 && !gnome_power2)
                        qWarning() << "W: gnome-power-cmd and gnome-power-cmd.sh didn't work";
        }
        else
                reboot_works = true;

        QDBusInterface kdeSessionManager("org.kde.ksmserver", "/KSMServer",
                                         "org.kde.KSMServerInterface", QDBusConnection::sessionBus());
        response = kdeSessionManager.call("logout", 0, 2, 1);
        if(response.type() == QDBusMessage::ErrorMessage){
                if(verbose)
                        qWarning() << "W: " << response.errorName() << ":" << response.errorMessage();
        }
        else
                reboot_works = true;

        if(!reboot_works && !gnome_power1 && !gnome_power2){
                QDBusInterface powermanagement("org.freedesktop.Hal",
                                               "/org/freedesktop/Hal/devices/computer",
                                               "org.freedesktop.Hal.Device.SystemPowerManagement",
                                               QDBusConnection::systemBus());
                response = powermanagement.call("Reboot");
                if(response.type() == QDBusMessage::ErrorMessage){
                        if(verbose)
                                qWarning() << "W: " << response.errorName() << ":" << response.errorMessage();
                }
                else
                        hal_works = true;
        }

        if(!hal_works && !reboot_works && !gnome_power1 && !gnome_power2){
                QDBusInterface powermanagement("org.freedesktop.ConsoleKit",
                                               "/org/freedesktop/ConsoleKit/Manager", "org.freedesktop.ConsoleKit.Manager",
                                               QDBusConnection::systemBus());
                response = powermanagement.call("Restart");
                if(response.type() == QDBusMessage::ErrorMessage){
                        if(verbose)
                                qWarning() << "W: " << response.errorName() << ":" << response.errorMessage();
                        QProcess::startDetached("sudo shutdown -r now");
                }
        }
}

void Power_Management::hibernate() {
        bool gnome_power1 = false;
        bool gnome_power2 = false;
        bool hal_works = false;
        QDBusMessage response;

        gnome_power1 = QProcess::startDetached("gnome-power-cmd.sh hibernate");
        gnome_power2 = QProcess::startDetached("gnome-power-cmd hibernate");
        if(!gnome_power1 && !gnome_power2 && verbose)
                qWarning() << "W: gnome-power-cmd and gnome-power-cmd.sh didn't work";

        if(!gnome_power1 && !gnome_power2){
                QDBusInterface powermanagement("org.freedesktop.Hal",
                                               "/org/freedesktop/Hal/devices/computer",
                                               "org.freedesktop.Hal.Device.SystemPowerManagement",
                                               QDBusConnection::systemBus());
                response = powermanagement.call("Hibernate");
                if(response.type() == QDBusMessage::ErrorMessage){
                        if(verbose)
                                qWarning() << "W: " << response.errorName() << ":" << response.errorMessage();
                }
                else
                        hal_works = true;
        }

        if(!hal_works && !gnome_power1 && !gnome_power2){
                QDBusInterface powermanagement("org.freedesktop.DeviceKit.Power", "/org/freedesktop/DeviceKit/Power",
                                               "org.freedesktop.DeviceKit.Power", QDBusConnection::systemBus());
                if(response.type() == QDBusMessage::ErrorMessage){
                        if(verbose)
                                qWarning() << "W: " << response.errorName() << ":" << response.errorMessage();
                }
        }
}

void Power_Management::lock() {
        if(system("gnome-screensaver-command -l") && system("dbus-send --type=method_call --dest=org.gnome.ScreenSaver /org/gnome/ScreenSaver org.gnome.ScreenSaver.Lock"))
                QMessageBox::warning(0, "Error", "Unable to lock computer.");

}
#endif
