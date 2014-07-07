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

#include "nnedi3.h"

int roundds(const double f)
{
	if (f-floor(f) >= 0.5)
                return std::min((int)ceil(f),32767);
        return std::max((int)floor(f),-32768);
}

void shufflePreScrnL2L3(float *wf, float *rf)
{
        for (int j=0; j<4; ++j)
                for (int k=0; k<4; ++k)
                        wf[k*4+j] = rf[j*4+k];
        rf += 4*5;
        wf += 4*5;
        const int jtable[4] = { 0, 2, 1, 3 };
        for (int j=0; j<4; ++j)
        {
                for (int k=0; k<8; ++k)
                        wf[k*4+j] = rf[jtable[j]*8+k];
                wf[4*8+j] = rf[4*8+jtable[j]];
        }
}


NNEDI3::NNEDI3(int width, int half_height, QObject *parent)
        : QObject(parent)
        ,width(width)
        ,half_height(half_height)
        ,currently_processing_image(false)
{
        //qDebug() << "NNEDI3(" << width << half_height << ")";
        weights0 = NULL;

        //note that i only maintain the assembly for one codepath, so changing the below values (except opt) will likely result in a crash (20130216)
        nsize = 6;
        nns = 1;
        qual = 1;
        fapprox = 15;
        pscrn = 2;
        etype = 0;

        opt = 2; //0 was autodetect, 1 is no assembly optimizations (c only) and 2 is sse2 (20130214)
        number_of_threads = QThread::idealThreadCount();

        stride = width + 64;
        int padded_height = half_height*2 + 12;
        int padded_plane_length = stride * padded_height;
        for (int i = 0; i < 3; ++i) {
#ifndef Q_OS_WIN32
                posix_memalign((void **)&src_planar[i], 16, padded_plane_length);
                posix_memalign((void **)&dst_planar[i], 16, padded_plane_length);
#else
                src_planar[i] = (uchar *)__mingw_aligned_malloc(padded_plane_length, 16);
                dst_planar[i] = (uchar *)__mingw_aligned_malloc(padded_plane_length, 16);
#endif
        }
	const int dims0 = 49*4+5*4+9*4;
	const int dims0new = 4*65+4*5;
	const int dims1 = nnsTable[nns]*2*(xdiaTable[nsize]*ydiaTable[nsize]+1);
        int dims1tsize = 0, dims1offset = 0;
	for (int j=0; j<NUM_NNS; ++j)
	{
		for (int i=0; i<NUM_NSIZE; ++i)
		{
			if (i == nsize && j == nns)
				dims1offset = dims1tsize;
			dims1tsize += nnsTable[j]*2*(xdiaTable[i]*ydiaTable[i]+1)*2;
		}
	}
#ifndef Q_OS_WIN32
        posix_memalign((void **)&weights0, 16, std::max(dims0,dims0new)*sizeof(float));
#else
        weights0 = (float *)__mingw_aligned_malloc(std::max(dims0,dims0new)*sizeof(float), 16);
#endif
        Q_ASSERT(weights0);
        for (int i=0; i<3; ++i)
	{
                if (i < 2) {
                        //weights1[i] = (float*)_aligned_malloc(dims1*sizeof(float),16);
#ifndef Q_OS_WIN32
                        posix_memalign((void **)&weights1[i], 16, dims1*sizeof(float));
#else
                        weights1[i] = (float *)__mingw_aligned_malloc(dims1*sizeof(float), 16);
#endif
                }

                //lcount[i] = (int*)_aligned_malloc(dstPF->GetHeight(i)*sizeof(int),16);
#ifndef Q_OS_WIN32
                posix_memalign((void **)&lcount[i], 16, (half_height*2)*sizeof(int));
#else
                lcount[i] = (int *)__mingw_aligned_malloc((half_height*2)*sizeof(int), 16);
#endif
        }

        float *bdata = NULL;
        QByteArray weights_data;
        QString resource_name("nnedi3_weights.bin");
        QFile resource_in_file(":/" + resource_name);
        if (resource_in_file.exists()) {
                resource_in_file.open(QIODevice::ReadOnly);
                weights_data.append(resource_in_file.readAll());
                resource_in_file.close();
        } else { //file in parts
                int resource_part = 1;
                resource_in_file.setFileName(QString(":/%1.%2").arg(resource_name).arg(resource_part));
                if (!resource_in_file.exists()) {
                        qDebug() << "NNEDI3: could not open weights file in resource section";
                } else {
                        while (resource_in_file.exists()) {
                                resource_in_file.open(QIODevice::ReadOnly);
                                weights_data.append(resource_in_file.readAll());
                                resource_in_file.close();

                                ++resource_part;
                                resource_in_file.setFileName(QString(":/%1.%2").arg(resource_name).arg(resource_part));
                        }
                }
        }
        if (weights_data.size()) {
                bdata = (float*)weights_data.data();
        }



	// Adjust prescreener weights
        if (pscrn >= 2) { // using new prescreener
		int *offt = (int*)calloc(4*64,sizeof(int));
		for (int j=0; j<4; ++j)
			for (int k=0; k<64; ++k)
				offt[j*64+k] = ((k>>3)<<5)+((j&3)<<3)+(k&7);
		const float *bdw = bdata+dims0+dims0new*(pscrn-2);
		short *ws = (short*)weights0;
		float *wf = (float*)&ws[4*64];
		double mean[4] = { 0.0, 0.0, 0.0, 0.0 };
		// Calculate mean weight of each first layer neuron
                for (int j=0; j<4; ++j) {
			double cmean = 0.0;
			for (int k=0; k<64; ++k)
				cmean += bdw[offt[j*64+k]];
			mean[j] = cmean/64.0;
		}
		// Factor mean removal and 1.0/127.5 scaling 
		// into first layer weights. scale to int16 range
                for (int j=0; j<4; ++j) {
			double mval = 0.0;
			for (int k=0; k<64; ++k)
                                mval = std::max(mval,fabs((bdw[offt[j*64+k]]-mean[j])/127.5));
			const double scale = 32767.0/mval;
			for (int k=0; k<64; ++k)
				ws[offt[j*64+k]] = roundds(((bdw[offt[j*64+k]]-mean[j])/127.5)*scale);
			wf[j] = (float)(mval/32767.0);
		}
		memcpy(wf+4,bdw+4*64,(dims0new-4*64)*sizeof(float));
		free(offt);
        } else { // using old prescreener
		double mean[4] = { 0.0, 0.0, 0.0, 0.0 };
		// Calculate mean weight of each first layer neuron
                for (int j=0; j<4; ++j) {
			double cmean = 0.0;
			for (int k=0; k<48; ++k)
				cmean += bdata[j*48+k];
			mean[j] = cmean/48.0;
		}
                if (fapprox&1) { // use int16 dot products in first layer
			short *ws = (short*)weights0;
			float *wf = (float*)&ws[4*48];
			// Factor mean removal and 1.0/127.5 scaling 
			// into first layer weights. scale to int16 range
                        for (int j=0; j<4; ++j) {
				double mval = 0.0;
				for (int k=0; k<48; ++k)
                                        mval = std::max(mval,fabs((bdata[j*48+k]-mean[j])/127.5));
				const double scale = 32767.0/mval;
				for (int k=0; k<48; ++k)
					ws[j*48+k] = roundds(((bdata[j*48+k]-mean[j])/127.5)*scale);
				wf[j] = (float)(mval/32767.0);
			}
                        memcpy(wf+4,bdata+4*48,(dims0-4*48)*sizeof(float));
                        if (opt > 1) { // shuffle weight order for asm
                                short *rs = (short*)malloc(dims0*sizeof(float));
                                memcpy(rs,weights0,dims0*sizeof(float));
                                for (int j=0; j<4; ++j)
                                        for (int k=0; k<48; ++k)
                                                ws[(k>>3)*32+j*8+(k&7)] = rs[j*48+k];
                                shufflePreScrnL2L3(wf+8,((float*)&rs[4*48])+8);
                                free(rs);
                        }
                } else { // use float dot products in first layer
			// Factor mean removal and 1.0/127.5 scaling 
			// into first layer weights.
			for (int j=0; j<4; ++j)
				for (int k=0; k<48; ++k)
					weights0[j*48+k] = (bdata[j*48+k]-mean[j])/127.5;
			memcpy(weights0+4*48,bdata+4*48,(dims0-4*48)*sizeof(float));
                        if (opt > 1) { // shuffle weight order for asm
                                float *wf = weights0;
                                float *rf = (float*)malloc(dims0*sizeof(float));
                                memcpy(rf,weights0,dims0*sizeof(float));
                                for (int j=0; j<4; ++j)
                                        for (int k=0; k<48; ++k)
                                                wf[(k>>2)*16+j*4+(k&3)] = rf[j*48+k];
                                shufflePreScrnL2L3(wf+4*49,rf+4*49);
                                free(rf);
                        }
                }
	}
	// Adjust prediction weights
        for (int i=0; i<2; ++i) {
		const float *bdataT = bdata+dims0+dims0new*3+dims1tsize*etype+dims1offset+i*dims1;
		const int nnst = nnsTable[nns];
		const int asize = xdiaTable[nsize]*ydiaTable[nsize];
		const int boff = nnst*2*asize;
		double *mean = (double*)calloc(asize+1+nnst*2,sizeof(double));
		// Calculate mean weight of each neuron (ignore bias)
                for (int j=0; j<nnst*2; ++j) {
			double cmean = 0.0;
			for (int k=0; k<asize; ++k)
				cmean += bdataT[j*asize+k];
			mean[asize+1+j] = cmean/(double)asize;
		}
		// Calculate mean softmax neuron
                for (int j=0; j<nnst; ++j) {
			for (int k=0; k<asize; ++k)
				mean[k] += bdataT[j*asize+k]-mean[asize+1+j];
			mean[asize] += bdataT[boff+j];
		}
		for (int j=0; j<asize+1; ++j)
			mean[j] /= (double)(nnst);
                if (fapprox&2) { // use int16 dot products
			short *ws = (short*)weights1[i];
			float *wf = (float*)&ws[nnst*2*asize];
			// Factor mean removal into weights, remove global offset from
			// softmax neurons, and scale weights to int16 range.
                        for (int j=0; j<nnst; ++j) { // softmax neurons
				double mval = 0.0;
				for (int k=0; k<asize; ++k)
                                        mval = std::max(mval,fabs(bdataT[j*asize+k]-mean[asize+1+j]-mean[k]));
				const double scale = 32767.0/mval;
				for (int k=0; k<asize; ++k)
					ws[j*asize+k] = roundds((bdataT[j*asize+k]-mean[asize+1+j]-mean[k])*scale);
				wf[(j>>2)*8+(j&3)] = (float)(mval/32767.0);
				wf[(j>>2)*8+(j&3)+4] = bdataT[boff+j]-mean[asize];
			}
                        for (int j=nnst; j<nnst*2; ++j) { // elliott neurons
				double mval = 0.0;
				for (int k=0; k<asize; ++k)
                                        mval = std::max(mval,fabs(bdataT[j*asize+k]-mean[asize+1+j]));
				const double scale = 32767.0/mval;
				for (int k=0; k<asize; ++k)
					ws[j*asize+k] = roundds((bdataT[j*asize+k]-mean[asize+1+j])*scale);
				wf[(j>>2)*8+(j&3)] = (float)(mval/32767.0);
				wf[(j>>2)*8+(j&3)+4] = bdataT[boff+j];
			}
                        if (opt > 1) { // shuffle weight order for asm
                                short *rs = (short*)malloc(nnst*2*asize*sizeof(short));
                                memcpy(rs,ws,nnst*2*asize*sizeof(short));
                                for (int j=0; j<nnst*2; ++j)
                                        for (int k=0; k<asize; ++k)
                                                ws[(j>>2)*asize*4+(k>>3)*32+(j&3)*8+(k&7)] = rs[j*asize+k];
                                free(rs);
                        }
                } else { // use float dot products
			// Factor mean removal into weights, and remove global
			// offset from softmax neurons.
                        for (int j=0; j<nnst*2; ++j) {
                                for (int k=0; k<asize; ++k) {
					const double q = j < nnst ? mean[k] : 0.0;
                                        if (opt > 1) // shuffle weight order for asm
                                                weights1[i][(j>>2)*asize*4+(k>>2)*16+(j&3)*4+(k&3)] =
                                                        bdataT[j*asize+k]-mean[asize+1+j]-q;
                                        else
                                                weights1[i][j*asize+k] = bdataT[j*asize+k]-mean[asize+1+j]-q;
				}
				weights1[i][boff+j] = bdataT[boff+j]-(j<nnst?mean[asize]:0.0);
			}
		}
		free(mean);
	}


        pssInfo = (PS_INFO**)malloc(number_of_threads*sizeof(PS_INFO*));
	int hslice[3], hremain[3];
	int srow[3] = { 6, 6, 6 };
        for (int i=0; i<3; ++i) {
                const int height = half_height*2;
                hslice[i] = height/number_of_threads;
                hremain[i] = height%number_of_threads;
	}
        for (int i=0; i<number_of_threads; ++i) {
                QThread *thread = new QThread;
                NNEDI3_Worker *worker = new NNEDI3_Worker;
                worker->moveToThread(thread);
                connect(this, SIGNAL(run_step_1()), worker, SLOT(step_1()));
                connect(worker, SIGNAL(step_1_finished()), this, SLOT(step_1_finished()));
                connect(this, SIGNAL(run_step_2()), worker, SLOT(step_2()));
                connect(worker, SIGNAL(step_2_finished()), this, SLOT(step_2_finished()));
                thread->start();
                threads << thread;
                workers << worker;

                pssInfo[i] = (PS_INFO*)malloc(sizeof(PS_INFO));
#ifndef Q_OS_WIN32
                posix_memalign((void **)&pssInfo[i]->input, 16, 512*sizeof(float));
#else
                pssInfo[i]->input = (float *)__mingw_aligned_malloc(512*sizeof(float), 16);
#endif
#ifndef Q_OS_WIN32
                posix_memalign((void **)&pssInfo[i]->temp, 16, 2048*sizeof(float));
#else
                pssInfo[i]->temp = (float *)__mingw_aligned_malloc(2048*sizeof(float), 16);
#endif
                pssInfo[i]->weights0 = weights0;
		pssInfo[i]->weights1 = weights1;
		pssInfo[i]->qual = qual;
		pssInfo[i]->pscrn = pscrn;
                pssInfo[i]->opt = opt;
		pssInfo[i]->nns = nnsTable[nns];
		pssInfo[i]->xdia = xdiaTable[nsize];
		pssInfo[i]->ydia = ydiaTable[nsize];
		pssInfo[i]->asize = xdiaTable[nsize]*ydiaTable[nsize];
		pssInfo[i]->fapprox = fapprox;
		for (int b=0; b<3; ++b)
		{
			pssInfo[i]->lcount[b] = lcount[b];
                        pssInfo[i]->dstp[b] = dst_planar[b];
                        pssInfo[i]->srcp[b] = src_planar[b];
                        pssInfo[i]->dst_pitch[b] = stride;
                        pssInfo[i]->src_pitch[b] = stride;
                        pssInfo[i]->height[b] = padded_height;
                        pssInfo[i]->width[b] = width + 64;
			pssInfo[i]->sheight[b] = srow[b];
			srow[b] += i == 0 ? hslice[b]+hremain[b] : hslice[b];
			pssInfo[i]->eheight[b] = srow[b];
		}

                worker->pss = pssInfo[i];
	}
}

NNEDI3::~NNEDI3() {
        //qDebug() << "~NNEDI3()";
        foreach (QThread *thread, threads) {
                thread->quit();
                thread->wait();
                delete thread;
        }
        foreach (NNEDI3_Worker *worker, workers) {
                delete worker;
        }

#ifndef Q_OS_WIN32
        for (int i = 0; i < 3; ++i) {
                free(src_planar[i]);
                free(dst_planar[i]);
        }

        free(weights0);
        for (int i=0; i<2; ++i)
                free(weights1[i]);
        for (int i=0; i<3; ++i)
                free(lcount[i]);
        for (int i=0; i<number_of_threads; ++i) {
                free(pssInfo[i]->input);
                free(pssInfo[i]->temp);
                free(pssInfo[i]);
        }
#else
        for (int i = 0; i < 3; ++i) {
                __mingw_aligned_free(src_planar[i]);
                __mingw_aligned_free(dst_planar[i]);
        }

        __mingw_aligned_free(weights0);
        for (int i=0; i<2; ++i)
                __mingw_aligned_free(weights1[i]);
        for (int i=0; i<3; ++i)
                __mingw_aligned_free(lcount[i]);
        for (int i=0; i<number_of_threads; ++i) {
                __mingw_aligned_free(pssInfo[i]->input);
                __mingw_aligned_free(pssInfo[i]->temp);
                free(pssInfo[i]);
        }
#endif
        free(pssInfo);
}

void NNEDI3::double_height(Frame in_image, bool top_field) {
        //qDebug() << this << "double_height()" << in_image.width() << "x" << in_image.height();
        if (in_image.width() % 4 != 0) { //FIXME can i solve this by just bumping the size up to mod 4? (20130429)
                qDebug() << this << "WIDTH IS NOT DIVISIBLE BY 4! THIS WILL CORRUPT THE IMAGE DUE TO QImage::convertToFormat(QImage::Format_RGB888) HAVING ALIGNMENT ISSUES";
        }
        in_image.image = in_image.image.convertToFormat(QImage::Format_RGB888);
        queue << QPair<Frame, bool>(in_image, top_field);
        return process_images();
}

void NNEDI3::process_images() {
        //qDebug() << this << "process_images()";
        if (currently_processing_image || !queue.size()) return;
        currently_processing_image = true;
        QPair<Frame, bool> pair = queue.dequeue();
        return process_image(pair.first, pair.second);
}

void NNEDI3::process_image(Frame in_image, bool top_field) {
        //qDebug() << this << "process_image()" << in_image.width() << "x" << in_image.height();
        current_frame = in_image; //so we save the timestamp etc information (20130302)
        int field_n = top_field ? 0 : 1;


        const int off = 1-field_n;

        uchar *src = in_image.bits() + in_image.bytesPerLine()*(in_image.height()-1);
        int src_stride = in_image.bytesPerLine();
        uchar *pr = src_planar[0]+stride*(6+off)+32;
        uchar *pg = src_planar[1]+stride*(6+off)+32;
        uchar *pb = src_planar[2]+stride*(6+off)+32;

        for (int y = 0; y < half_height; ++y) {
                for (int x = 0; x < width; ++x) {
                        pr[x] = src[x*3+0];
                        pg[x] = src[x*3+1];
                        pb[x] = src[x*3+2];
                }
                src -= src_stride;
                pr += stride*2;
                pg += stride*2;
                pb += stride*2;
        }



        for (int i=0; i<3; ++i)
                memset(lcount[i],0,(half_height*2)*sizeof(int));
        for (int i=0; i<number_of_threads; ++i) {
                for (int b=0; b<3; ++b) {
			const int srow = pssInfo[i]->sheight[b];
			pssInfo[i]->field[b] = (srow&1) ? 1-field_n : field_n;
		}
	}

        workers_finished = 0;
        emit run_step_1();
}

void NNEDI3::step_1_finished() {
        if (++workers_finished < number_of_threads) return;
        workers_finished = 0;
        calcStartEnd2();
        emit run_step_2();
}
void NNEDI3::step_2_finished() {
        if (++workers_finished < number_of_threads) return;
        workers_finished = 0;

        QImage out_image(width, half_height*2, QImage::Format_RGB888);
        uchar *dst = out_image.bits();
        int dst_stride = out_image.bytesPerLine();
        int height = out_image.height();
        uchar *pr = dst_planar[0];
        uchar *pg = dst_planar[1];
        uchar *pb = dst_planar[2];

        dst += (out_image.height()-1)*dst_stride;
        for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                        dst[x*3+0] = pr[x];
                        dst[x*3+1] = pg[x];
                        dst[x*3+2] = pb[x];
                }
                pr += stride;
                pg += stride;
                pb += stride;
                dst -= dst_stride;
        }

        Frame out_frame = current_frame;
        out_frame.image = out_image.convertToFormat(QImage::Format_ARGB32);
        emit height_doubled_image(out_frame);
        currently_processing_image = false;
        return process_images();
}

void NNEDI3::calcStartEnd2() {
        for (int b=0; b<3; ++b) {
                const int height = half_height*2;
		int total = 0, fl = -1, ll = 0;
                for (int j=0; j<height; ++j) {
			total += lcount[b][j];
			if (fl < 0 && lcount[b][j] > 0) fl = j;
		}
		if (total == 0)
			fl = height;
                else {
                        for (int j=height-1; j>=0; --j) {
				if (lcount[b][j])
					break;
				++ll;
			}
		}
                int tslice = int(total/double(number_of_threads)+0.95);
		int count=0, countt=0, y=fl, yl=fl, th=0;
                while (y < height-ll) {
			count += lcount[b][y++];
                        if (count >= tslice) {
				pssInfo[th]->sheight2[b] = yl;
				countt += count;
				if (countt == total)
					y = height-ll;
				pssInfo[th]->eheight2[b] = y;
				while (y < height-ll && lcount[b][y] == 0)
					++y;
				yl = y;
				count = 0;
				++th;
			}
		}
                if (yl != y) {
			pssInfo[th]->sheight2[b] = yl;
			countt += count;
			if (countt == total)
				y = height-ll;
			pssInfo[th]->eheight2[b] = y;
			++th;
		}
                for (; th<number_of_threads; ++th)
			pssInfo[th]->sheight2[b] = pssInfo[th]->eheight2[b] = height;
	}
}
