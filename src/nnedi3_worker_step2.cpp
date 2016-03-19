#include "nnedi3_worker.h"

void NNEDI3_Worker::step_2() {
        float *input = pss->input;
        float *temp = pss->temp;
        float **weights1 = pss->weights1;
        //const int opt = pss->opt;
        const int qual = pss->qual;
        const int asize = pss->asize;
        const int nns = pss->nns;
        const int xdia = pss->xdia;
        const int xdiad2m1 = (xdia>>1)-1;
        const int ydia = pss->ydia;
        const int fapprox = pss->fapprox;
        const float scale = 1.0f/(float)qual;
        void (*extract)(const unsigned char*, const int, const int, const int, float*, float*);
        void (*dotProd)(const float*, const float*, float*, const int, const int, const float*);
        void (*expf)(float *, const int);
        void (*wae5)(const float*, const int, float*);

        if (fapprox&2) { // use int16 dot products
#if NNEDI3_OPT == 1
                extract = extract_m8_i16_C;
                dotProd = dotProdS_C;
#else
                extract = extract_m8_i16_SSE2;
                dotProd = dotProd_m32_m16_i16_SSE2;
#endif //NNEDI3_OPT
        }
#if NNEDI3_OPT == 1
        expf = e0_m16_C;
        wae5 = weightedAvgElliottMul5_m16_C;
#else
        expf = e0_m16_SSE2;
        wae5 = weightedAvgElliottMul5_m16_SSE2;
#endif //NNEDI3_OPT

        for (int b=0; b<3; ++b) {
                const unsigned char *srcp = pss->srcp[b];
                const int src_pitch = pss->src_pitch[b];
                const int width = pss->width[b];
                unsigned char *dstp = pss->dstp[b];
                const int dst_pitch = pss->dst_pitch[b];
                const int ystart = pss->sheight2[b];
                const int ystop = pss->eheight2[b];
                srcp += (ystart+6)*src_pitch;
                dstp += ystart*dst_pitch-32;
                const unsigned char *srcpp = srcp-(ydia-1)*src_pitch-xdiad2m1;
                for (int y=ystart; y<ystop; y+=2) {
                        for (int x=32; x<width-32; ++x) {
                                if (dstp[x] != 255)
                                        continue;
                                float mstd[4];
                                extract(srcpp+x,src_pitch,xdia,ydia,mstd,input); //problem is on this line (20130401)
                                for (int i=0; i<qual; ++i) {
                                        dotProd(input,weights1[i],temp,nns*2,asize,mstd+2);
                                        expf(temp,nns);
                                        wae5(temp,nns,mstd);
                                }
#if NNEDI3_OPT > 1
                                        castScale_SSE(mstd,&scale,dstp+x);
#else
                                        dstp[x] = std::min(std::max((int)(mstd[3]*scale+0.5f),0),255);
#endif //NNEDI3_OPT
                        }
                        srcpp += src_pitch*2;
                        dstp += dst_pitch*2;
                }
        }

        emit step_2_finished();
}

