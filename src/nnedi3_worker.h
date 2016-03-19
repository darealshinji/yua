#ifndef NNEDI3_WORKER_H
#define NNEDI3_WORKER_H

#include <QtGui>

#include <float.h>

#include "nnedi3/nnedi3_constants.h"

#ifndef NNEDI3_OPT
// 0 autodetect
// 1 no assembly (c only)
// 2 SSE2
#define NNEDI3_OPT 2
#endif


struct PS_INFO {
        int field[3];
        const unsigned char *srcp[3];
        unsigned char *dstp[3];
        int src_pitch[3], dst_pitch[3];
        int height[3], width[3];
        int sheight[3], eheight[3];
        int sheight2[3], eheight2[3];
        int *lcount[3], opt, qual;
        float *input, *temp;
        float *weights0, **weights1;
        int asize, nns, xdia, ydia, fapprox;
        int pscrn;
};


class NNEDI3_Worker : public QObject {
        Q_OBJECT
public:
        explicit NNEDI3_Worker(QObject *parent = 0);
        PS_INFO *pss;
        static void BitBlt(uchar *dstp, int dst_pitch, const uchar *srcp, int src_pitch, int row_size, int height);
        void (*uc2s)(const unsigned char*, const int, float*);
        void (*computeNetwork0)(const float*, const float*, unsigned char *d);
        
signals:
        void step_1_finished();
        void step_2_finished();
        
public slots:
        void step_1();
        void step_2();
};

extern "C" int processLine0_C(const unsigned char *tempu, int width, unsigned char *dstp, const unsigned char *src3p, const int src_pitch);
extern "C" void uc2s64_C(const unsigned char *t, const int pitch, float *p);
extern "C" void computeNetwork0new_C(const float *datai, const float *weights, unsigned char *d);
extern "C" void weightedAvgElliottMul5_m16_C(const float *w, const int n, float *mstd);
extern "C" void extract_m8_i16_C(const unsigned char *srcp, const int stride, const int xdia, const int ydia, float *mstd, float *inputf);
extern "C" void dotProdS_C(const float *dataf, const float *weightsf, float *vals, const int n, const int len, const float *scale);
extern "C" void e0_m16_C(float *s, const int n);

#if NNEDI3_OPT > 1
extern "C" int processLine0_SSE2(const unsigned char *tempu, int width, unsigned char *dstp, const unsigned char *src3p, const int src_pitch);
extern "C" void uc2s64_SSE2(const unsigned char *t, const int pitch, float *p);
extern "C" void computeNetwork0new_SSE2(const float *datai, const float *weights, unsigned char *d);
extern "C" void weightedAvgElliottMul5_m16_SSE2(const float *w, const int n, float *mstd);
extern "C" void extract_m8_i16_SSE2(const unsigned char *srcp, const int stride, const int xdia, const int ydia, float *mstd, float *inputf);
extern "C" void dotProd_m32_m16_i16_SSE2(const float *dataf, const float *weightsf, float *vals, const int n, const int len, const float *istd);
extern "C" void e0_m16_SSE2(float *s, const int n);
extern "C" void castScale_SSE(const float *val, const float *scale, unsigned char *dstp);
#endif // NNEDI3_OPT


#endif // NNEDI3_WORKER_H
