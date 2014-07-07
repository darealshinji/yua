/*
**                    nnedi3 v0.9.4 for Avisynth 2.5.x
**
**   Copyright (C) 2010-2011 Kevin Stone
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

#ifndef NNEDI3_H
#define NNEDI3_H

#include <QtGui>

//#include <windows.h>
//#define _USE_MATH_DEFINES
#include <math.h>
//#include <tchar.h>
//#include <process.h>
#include <stdio.h>
//#include "avisynth.h"
//#include "PlanarFrame.h"

#include <stdlib.h>

#ifdef Q_OS_WIN32 //only windows needs this and it's only present in mingw (20120208)
#include <malloc.h>
#endif

#include "nnedi3_worker.h"
#include "frame.h"

#define NUM_NSIZE 7
#define NUM_NNS 5
const int xdiaTable[NUM_NSIZE] = { 8, 16, 32, 48, 8, 16, 32 };
const int ydiaTable[NUM_NSIZE] = { 6, 6, 6, 6, 4, 4, 4 };
const int nnsTable[NUM_NNS] = { 16, 32, 64, 128, 256 };

class NNEDI3 : public QObject {
        Q_OBJECT
public:
        NNEDI3(int, int, QObject * = 0);
        ~NNEDI3();

signals:
        void run_step_1();
        void run_step_2();
        void height_doubled_image(Frame);

public slots:
        void double_height(Frame, bool);

private slots:
        void step_1_finished();
        void step_2_finished();

private:
        int pscrn;
        int number_of_threads, nns, etype, opt;
        int *lcount[3], qual, nsize, fapprox;
        //PlanarFrame *srcPF, *dstPF;
        PS_INFO **pssInfo;
        //unsigned *tids;
        float *weights0, *weights1[2];
        void calcStartEnd2();

        QList<QThread *> threads;
        QList<NNEDI3_Worker *> workers;
        int workers_finished;
        uchar *src_planar[3], *dst_planar[3];
        int stride;
        int width, half_height;
        QQueue<QPair<Frame, bool> > queue;
        void process_image(Frame, bool);
        void process_images();
        bool currently_processing_image;
        Frame current_frame;
};

#endif
