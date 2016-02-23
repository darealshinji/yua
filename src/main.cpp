/*
**                    Yua
**
**   Copyright 2014 Taiga Software LLC
**
**   This program is free software; you can redistribute it and/or modify
**   it under the terms of the GNU General Public License as published by
**   the Free Software Foundation; either version 2 of the License, or
**   (at your option) any later version.
**
**   This program is distributed in the hope that it will be useful,
**   but WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**   GNU General Public License for more details.
**
**   You should have received a copy of the GNU General Public License
**   along with this program; if not, write to the Free Software
**   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "yua.h"
#include <QApplication>

int main(int argc, char *argv[]) {
        QApplication a(argc, argv);

        //http://stackoverflow.com/questions/14606396/gtk-critical-ia-gtk-widget-style-get-assertion-gtk-is-widget-widget-fa
        //#ifdef Q_OS_LINUX
        //        a.setStyle(new QPlastiqueStyle());
        //#endif
#ifdef Q_WS_X11
        qputenv("LIBOVERLAY_SCROLLBAR", 0);
#endif

#ifndef DATAROOTDIR
#define DATAROOTDIR /usr/local/share
#endif

#ifdef EXTERNAL_NNEDI3_WEIGHTS
        QString resource_name("nnedi3_weights.bin");
        QString path1 = QString("%1/%2").arg(qApp->applicationDirPath()).arg(resource_name);
#ifdef Q_OS_WIN32
        if (!(QFile(path1).exists())) {
            qDebug() << "NNEDI3: could not find weights file in" << path1;
#else
        QString path2 = QString(DATAROOTDIR "/nnedi3/%2").arg(resource_name);
        if (!(QFile(path1).exists()) && !(QFile(path2).exists())) {
            qDebug() << "NNEDI3: could not find weights file in" << path2 << "or" << path1;
#endif //Q_OS_WIN32
            std::exit(1);
        }
#endif //EXTERNAL_NNEDI3_WEIGHTS

        Yua w;
        w.show();

        return a.exec();
}
