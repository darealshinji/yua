#include "yua_util.h"

Yua_Util::Yua_Util()
{
}

//stolen from ffms2 videoutils.cpp (20140706)
SwsContext *Yua_Util::GetSwsContext(int SrcW, int SrcH, PixelFormat SrcFormat, int SrcColorSpace, int SrcColorRange,
                                         int DstW, int DstH, PixelFormat DstFormat, int DstColorSpace, int DstColorRange, int64_t Flags) {
        Flags |= SWS_FULL_CHR_H_INT | SWS_FULL_CHR_H_INP | SWS_ACCURATE_RND | SWS_BITEXACT;
        SwsContext *Context = sws_alloc_context();
        if (!Context) return 0;

        // 0 = limited range, 1 = full range
        int SrcRange = SrcColorRange == AVCOL_RANGE_JPEG;
        int DstRange = DstColorRange == AVCOL_RANGE_JPEG;
        qDebug() << "Yua_Util::GetSwsContext(): srcrange dstrange:" << SrcRange << DstRange;

        av_opt_set_int(Context, "sws_flags",  Flags, 0);
        av_opt_set_int(Context, "srcw",       SrcW, 0);
        av_opt_set_int(Context, "srch",       SrcH, 0);
        av_opt_set_int(Context, "dstw",       DstW, 0);
        av_opt_set_int(Context, "dsth",       DstH, 0);
        av_opt_set_int(Context, "src_range",  SrcRange, 0);
        av_opt_set_int(Context, "dst_range",  DstRange, 0);
        av_opt_set_int(Context, "src_format", SrcFormat, 0);
        av_opt_set_int(Context, "dst_format", DstFormat, 0);

        sws_setColorspaceDetails(Context,
                sws_getCoefficients(SrcColorSpace), SrcRange,
                sws_getCoefficients(DstColorSpace), DstRange,
                0, 1<<16, 1<<16);

        if(sws_init_context(Context, 0, 0) < 0){
                sws_freeContext(Context);
                return 0;
        }

        return Context;
}
