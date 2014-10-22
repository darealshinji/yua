#include "nnedi3_worker.h"

NNEDI3_Worker::NNEDI3_Worker(QObject *parent) :
        QObject(parent)
{
}


//taken from avisynth.cpp (20130202)
void NNEDI3_Worker::BitBlt(uchar *dstp, int dst_pitch, const uchar *srcp, int src_pitch, int row_size, int height) {
        if (height == 1 || (dst_pitch == src_pitch && src_pitch == row_size)) {
                memcpy(dstp, srcp, row_size*height);
        } else {
                for (int y=height; y>0; --y) {
                        memcpy(dstp, srcp, row_size);
                        dstp += dst_pitch;
                        srcp += src_pitch;
                }
        }
}


void elliott_C(float *data, const int n)
{
        for (int i=0; i<n; ++i)
                data[i] = data[i]/(1.0f+fabsf(data[i]));
}

void dotProd_C(const float *data, const float *weights,
        float *vals, const int n, const int len, const float *scale)
{
        for (int i=0; i<n; ++i)
        {
                float sum = 0.0f;
                for (int j=0; j<len; ++j)
                        sum += data[j]*weights[i*len+j];
                vals[i] = sum*scale[0]+weights[n*len+i];
        }
}

void dotProdS_C(const float *dataf, const float *weightsf,
        float *vals, const int n, const int len, const float *scale)
{
        const short *data = (short*)dataf;
        const short *weights = (short*)weightsf;
        const float *wf = (float*)&weights[n*len];
        for (int i=0; i<n; ++i)
        {
                int sum = 0, off = ((i>>2)<<3)+(i&3);
                for (int j=0; j<len; ++j)
                        sum += data[j]*weights[i*len+j];
                vals[i] = sum*wf[off]*scale[0]+wf[off+4];
        }
}

void computeNetwork0_C(const float *input, const float *weights, unsigned char *d)
{
        float temp[12], scale = 1.0f;
        dotProd_C(input,weights,temp,4,48,&scale);
        const float t = temp[0];
        elliott_C(temp,4);
        temp[0] = t;
        dotProd_C(temp,weights+4*49,temp+4,4,4,&scale);
        elliott_C(temp+4,4);
        dotProd_C(temp,weights+4*49+4*5,temp+8,4,8,&scale);
        if (std::max(temp[10],temp[11]) <= std::max(temp[8],temp[9]))
                d[0] = 1;
        else
                d[0] = 0;
}

void computeNetwork0_i16_C(const float *inputf, const float *weightsf, unsigned char *d)
{
        const float *wf = weightsf+2*48;
        float temp[12], scale = 1.0f;
        dotProdS_C(inputf,weightsf,temp,4,48,&scale);
        const float t = temp[0];
        elliott_C(temp,4);
        temp[0] = t;
        dotProd_C(temp,wf+8,temp+4,4,4,&scale);
        elliott_C(temp+4,4);
        dotProd_C(temp,wf+8+4*5,temp+8,4,8,&scale);
        if (std::max(temp[10],temp[11]) <= std::max(temp[8],temp[9]))
                d[0] = 1;
        else
                d[0] = 0;
}

void uc2f48_C(const unsigned char *t, const int pitch, float *p)
{
        for (int y=0; y<4; ++y)
                for (int x=0; x<12; ++x)
                        p[y*12+x] = t[y*pitch*2+x];
}

void uc2s48_C(const unsigned char *t, const int pitch, float *pf)
{
        short *p = (short*)pf;
        for (int y=0; y<4; ++y)
                for (int x=0; x<12; ++x)
                        p[y*12+x] = t[y*pitch*2+x];
}

int processLine0_C(const unsigned char *tempu, int width, unsigned char *dstp,
        const unsigned char *src3p, const int src_pitch)
{
        int count = 0;
        for (int x=0; x<width; ++x)
        {
                if (tempu[x])
                        dstp[x] = CB2((19*(src3p[x+src_pitch*2]+src3p[x+src_pitch*4])-
                                3*(src3p[x]+src3p[x+src_pitch*6])+16)>>5);
                else
                {
                        dstp[x] = 255;
                        ++count;
                }
        }
        return count;
}


void NNEDI3_Worker::step_1() {
        //PS_INFO *pss = (PS_INFO*)ps;
        float *input = pss->input;
        const float *weights0 = pss->weights0;
        float *temp = pss->temp;
        unsigned char *tempu = (unsigned char*)temp;
        const int opt = pss->opt;
        const int pscrn = pss->pscrn;
        const int fapprox = pss->fapprox;
        void (*uc2s)(const unsigned char*, const int, float*);
        void (*computeNetwork0)(const float*, const float*, unsigned char *d);
        int (*processLine0)(const unsigned char*, int, unsigned char*, const unsigned char*, const int);
        if (opt == 1)
                processLine0 = processLine0_C;
        else
                processLine0 = processLine0_SSE2;
        if (pscrn < 2) { // original prescreener
                if (fapprox&1) { // int16 dot products
                        if (opt == 1) {
                                uc2s = uc2s48_C;
                                computeNetwork0 = computeNetwork0_i16_C;
                        } else {
//                                uc2s = uc2s48_SSE2;
//                                computeNetwork0 = computeNetwork0_i16_SSE2;
                        }
                } else {
                        if (opt == 1) {
                                uc2s = uc2f48_C;
                                computeNetwork0 = computeNetwork0_C;
                        } else {
//                                uc2s = uc2f48_SSE2;
//                                computeNetwork0 = computeNetwork0_SSE2;
                        }
                }
        } else { // new prescreener
                // only int16 dot products
                //it uses this one
                if (opt == 1) {
                        uc2s = uc2s64_C;
                        computeNetwork0 = computeNetwork0new_C;
                } else {
                        uc2s = uc2s64_SSE2;
                        computeNetwork0 = computeNetwork0new_SSE2;
                }
        }

        for (int b=0; b<3; ++b) {
                const unsigned char *srcp = pss->srcp[b];
                const int src_pitch = pss->src_pitch[b];
                const int width = pss->width[b];
                unsigned char *dstp = pss->dstp[b];
                const int dst_pitch = pss->dst_pitch[b];
                BitBlt(dstp+(pss->sheight[b]-5-pss->field[b])*dst_pitch,
                        dst_pitch*2,srcp+(pss->sheight[b]+1-pss->field[b])*src_pitch+32,
                        src_pitch*2,width-64,(pss->eheight[b]-pss->sheight[b]+pss->field[b])>>1);
                const int ystart = pss->sheight[b]+pss->field[b];
                const int ystop = pss->eheight[b];
                srcp += ystart*src_pitch;
                dstp += (ystart-6)*dst_pitch-32;
                const unsigned char *src3p = srcp-src_pitch*3;
                int *lcount = pss->lcount[b]-6;
                if (pss->pscrn == 1) { // original
                        for (int y=ystart; y<ystop; y+=2) {
                                for (int x=32; x<width-32; ++x) {
                                        uc2s(src3p+x-5,src_pitch,input);
                                        computeNetwork0(input,weights0,tempu+x);
                                }
                                lcount[y] += processLine0(tempu+32,width-64,dstp+32,src3p+32,src_pitch);
                                src3p += src_pitch*2;
                                dstp += dst_pitch*2;
                        }
                } else if (pss->pscrn >= 2) { // new
                        for (int y=ystart; y<ystop; y+=2) {
                                for (int x=32; x<width-32; x+=4) {
                                        uc2s(src3p+x-6,src_pitch,input);
                                        computeNetwork0(input,weights0,tempu+x);
                                }
                                lcount[y] += processLine0(tempu+32,width-64,dstp+32,src3p+32,src_pitch);
                                src3p += src_pitch*2;
                                dstp += dst_pitch*2;
                        }
                } else { // no prescreening
                        for (int y=ystart; y<ystop; y+=2) {
                                memset(dstp+32,255,width-64);
                                lcount[y] += width-64;
                                dstp += dst_pitch*2;
                        }
                }
        }

        emit step_1_finished();
}

void extract_m8_C(const unsigned char *srcp, const int stride,
        const int xdia, const int ydia, float *mstd, float *input)
{
        int sum = 0, sumsq = 0;
        for (int y=0; y<ydia; ++y)
        {
                const unsigned char *srcpT = srcp+y*stride*2;
                for (int x=0; x<xdia; ++x, ++input)
                {
                        sum += srcpT[x];
                        sumsq += srcpT[x]*srcpT[x];
                        input[0] = srcpT[x];
                }
        }
        const float scale = 1.0f/(float)(xdia*ydia);
        mstd[0] = sum*scale;
        mstd[1] = sumsq*scale-mstd[0]*mstd[0];
        mstd[3] = 0.0f;
        if (mstd[1] <= FLT_EPSILON)
                mstd[1] = mstd[2] = 0.0f;
        else
        {
                mstd[1] = sqrtf(mstd[1]);
                mstd[2] = 1.0f/mstd[1];
        }
}

void extract_m8_i16_C(const unsigned char *srcp, const int stride,
        const int xdia, const int ydia, float *mstd, float *inputf)
{
        short *input = (short*)inputf;
        int sum = 0, sumsq = 0;
        for (int y=0; y<ydia; ++y)
        {
                const unsigned char *srcpT = srcp+y*stride*2;
                for (int x=0; x<xdia; ++x, ++input)
                {
                        sum += srcpT[x];
                        sumsq += srcpT[x]*srcpT[x];
                        input[0] = srcpT[x];
                }
        }
        const float scale = 1.0f/(float)(xdia*ydia);
        mstd[0] = sum*scale;
        mstd[1] = sumsq*scale-mstd[0]*mstd[0];
        mstd[3] = 0.0f;
        if (mstd[1] <= FLT_EPSILON)
                mstd[1] = mstd[2] = 0.0f;
        else
        {
                mstd[1] = sqrtf(mstd[1]);
                mstd[2] = 1.0f/mstd[1];
        }
}

void e0_m16_C(float *s, const int n)
{
        for (int i=0; i<n; ++i)
        {
                s[i] = (float)(std::max(std::min(s[i],exp_hi[0]),exp_lo[0])*e0_mult[0]+e0_bias[0]);;
        }
}

// exp from Loren Merritt

void e1_m16_C(float *s, const int n)
{
        for (int q=0; q<n; ++q)
        {
                float x = std::max(std::min(s[q],exp_hi[0]),exp_lo[0])*e1_scale[0];
                int i = (int)(x + 128.5f) - 128;
                x -= i;
                x = e1_c0[0] + e1_c1[0]*x + e1_c2[0]*x*x;
                i = (i+127)<<23;
                float* _i = ((float*)&i);
                s[q] = x * *_i;
        }
}

void e2_m16_C(float *s, const int n)
{
        for (int i=0; i<n; ++i)
                s[i] = expf(std::max(std::min(s[i],exp_hi[0]),exp_lo[0]));
}

void weightedAvgElliottMul5_m16_C(const float *w, const int n, float *mstd)
{
        float vsum = 0.0f, wsum = 0.0f;
        for (int i=0; i<n; ++i)
        {
                vsum += w[i]*(w[n+i]/(1.0f+fabsf(w[n+i])));
                wsum += w[i];
        }
        if (wsum > min_weight_sum[0])
                mstd[3] += ((5.0f*vsum)/wsum)*mstd[1]+mstd[0];
        else
                mstd[3] += mstd[0];
}




void uc2s64_C(const unsigned char *t, const int pitch, float *p)
{
        short *ps = (short*)p;
        for (int y=0; y<4; ++y)
                for (int x=0; x<16; ++x)
                        ps[y*16+x] = t[y*pitch*2+x];
}

void computeNetwork0new_C(const float *datai, const float *weights, unsigned char *d)
{
        short *data = (short*)datai;
        short *ws = (short*)weights;
        float *wf = (float*)&ws[4*64];
        float vals[8];
        for (int i=0; i<4; ++i)
        {
                int sum = 0;
                for (int j=0; j<64; ++j)
                        sum += data[j]*ws[(i<<3)+((j>>3)<<5)+(j&7)];
                const float t = sum*wf[i]+wf[4+i];
                vals[i] = t/(1.0f+fabsf(t));
        }
        for (int i=0; i<4; ++i)
        {
                float sum = 0.0f;
                for (int j=0; j<4; ++j)
                        sum += vals[j]*wf[8+i+(j<<2)];
                vals[4+i] = sum+wf[8+16+i];
        }
        int mask = 0;
        for (int i=0; i<4; ++i)
        {
                if (vals[4+i]>0.0f)
                        mask |= (0x1<<(i<<3));
        }
        ((int*)d)[0] = mask;
}
