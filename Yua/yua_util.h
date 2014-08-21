#ifndef YUA_UTIL_H
#define YUA_UTIL_H

#include <QDebug>

extern "C" {
#include "libavutil/opt.h"
#include "libavutil/channel_layout.h"
#include "libavutil/samplefmt.h"
#include "libavutil/channel_layout.h"
#include "libavresample/avresample.h"

#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavcodec/avcodec.h"
#include "libavutil/pixdesc.h"

#include "yv12toyuy2.h"
}

class Yua_Util {
public:
        Yua_Util();
        static SwsContext *GetSwsContext(int SrcW, int SrcH, PixelFormat SrcFormat, int SrcColorSpace, int SrcColorRange, int DstW, int DstH, PixelFormat DstFormat, int DstColorSpace, int DstColorRange, int64_t Flags);
};

#endif // YUA_UTIL_H
