#ifndef NNEDI3_WORKER_H
#define NNEDI3_WORKER_H

#include <QtGui>

#include <float.h>

#include "nnedi3/nnedi3_constants.h"


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


void uc2s64_C(const unsigned char *t, const int pitch, float *p);
void computeNetwork0new_C(const float *datai, const float *weights, unsigned char *d);

//assembly functions linked in at compile time
//extern "C" void yua_computeNetwork0_SSE2(const float *input, const float *weights, unsigned char *d);
//extern "C" void yua_computeNetwork0_i16_SSE2(const float *inputf, const float *weightsf, unsigned char *d);
//extern "C" void yua_uc2f48_SSE2(const unsigned char *t, const int pitch, float *p);
//extern "C" void yua_uc2s48_SSE2(const unsigned char *t, const int pitch, float *pf);
//extern "C" void yua_extract_m8_SSE2(const unsigned char *srcp, const int stride, const int xdia, const int ydia, float *mstd, float *input);
//extern "C" void yua_extract_m8_i16_SSE2(const unsigned char *srcp, const int stride, const int xdia, const int ydia, float *mstd, float *inputf);
//extern "C" void yua_dotProd_m32_m16_SSE2(const float *data, const float *weights, float *vals, const int n, const int len, const float *istd);
//extern "C" void yua_dotProd_m48_m16_SSE2(const float *data, const float *weights, float *vals, const int n, const int len, const float *istd);
//extern "C" void yua_dotProd_m32_m16_i16_SSE2(const float *dataf, const float *weightsf, float *vals, const int n, const int len, const float *istd);
//extern "C" void yua_dotProd_m48_m16_i16_SSE2(const float *dataf, const float *weightsf, float *vals, const int n, const int len, const float *istd);
//extern "C" void yua_e0_m16_SSE2(float *s, const int n);
//extern "C" void yua_e1_m16_SSE2(float *s, const int n);
//extern "C" void yua_e2_m16_SSE2(float *s, const int n);
//extern "C" void yua_weightedAvgElliottMul5_m16_SSE2(const float *w, const int n, float *mstd);
//extern "C" void yua_castScale_SSE(const float *val, const float *scale, unsigned char *dstp);
//extern "C" void yua_uc2s64_SSE2(const unsigned char *t, const int pitch, float *p);
//extern "C" void yua_computeNetwork0new_SSE2(const float *datai, const float *weights, unsigned char *d);


extern "C" int processLine0_SSE2(const unsigned char *tempu, int width, unsigned char *dstp, const unsigned char *src3p, const int src_pitch);
extern "C" void uc2s64_SSE2(const unsigned char *t, const int pitch, float *p);
extern "C" void computeNetwork0new_SSE2(const float *datai, const float *weights, unsigned char *d);
extern "C" void weightedAvgElliottMul5_m16_SSE2(const float *w, const int n, float *mstd);
extern "C" void extract_m8_i16_SSE2(const unsigned char *srcp, const int stride, const int xdia, const int ydia, float *mstd, float *inputf);
extern "C" void dotProd_m32_m16_i16_SSE2(const float *dataf, const float *weightsf, float *vals, const int n, const int len, const float *istd);
extern "C" void e0_m16_SSE2(float *s, const int n);
extern "C" void castScale_SSE(const float *val, const float *scale, unsigned char *dstp);


#endif // NNEDI3_WORKER_H
