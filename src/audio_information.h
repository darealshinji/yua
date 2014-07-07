#ifndef AUDIO_INFORMATION_H
#define AUDIO_INFORMATION_H

#include <QtGlobal>

class Audio_Information {
public:
        Audio_Information(int=0, int=0, int=0, int=48000, int=0, int=0, qint64=1, qint64=1, qint64=1);

        int bitrate;
        int number_of_channels;
        int sample_format;
        int sample_rate;
        int channel_layout;
        int bytes_per_sample;
        qint64 timebase_num;
        qint64 timebase_den;
        qint64 monotonic_pts_increase;
};

#endif // AUDIO_INFORMATION_H
