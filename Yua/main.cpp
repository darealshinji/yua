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

        Yua w;
        w.show();

        return a.exec();
}
