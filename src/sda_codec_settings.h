#ifndef SDA_CODEC_SETTINGS_H
#define SDA_CODEC_SETTINGS_H


// yua.cpp
#define XQ_VIDEO_BITRATE 10000
#define IQ_VIDEO_BITRATE 5000
#define HQ_VIDEO_BITRATE 2048
#define MQ_VIDEO_BITRATE 512
#define LQ_VIDEO_BITRATE 128

#define XQ_AUDIO_BITRATE 320000 //320kb/s
#define IQ_AUDIO_BITRATE 320000 //320kb/s
#define HQ_AUDIO_BITRATE 128000 //128kb/s
#define MQ_AUDIO_BITRATE 64000 //64kb/s
#define LQ_AUDIO_BITRATE 64000 //64kb/s

#define AUDIO_COMMENTARY_BITRATE_SETTING "32k" //32kb/s

#define MQ_MAX_FRAMERATE 35 //changed from 31 due to an ff7 run (20131214)

#define D4_NATIVE_HEIGHT 240
#define LQ_HEIGHT 240
#define MQ_HEIGHT 240
#define HQ_HEIGHT 480
#define IQ_HEIGHT 768


// video_encoder.cpp
#define SDA_PRESET "veryslow"
#define SDA_REFS "4"
#define SDA_QPMIN "4"

#define SDA_PROFILE "high"
//#define SDA_MQ_PROFILE "baseline"

#define SDA_LEVEL "41" //bluray compatibility level
//#define SDA_MQ_LEVEL "13" //from anri

#define SDA_FIRSTPASS_CRF "17"
#define SDA_FASTFIRSTPASS "0" //slow first pass


#endif // SDA_CODEC_SETTINGS_H
