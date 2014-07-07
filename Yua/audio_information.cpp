#include "audio_information.h"

Audio_Information::Audio_Information(int bitrate, int number_of_channels,
                                     int sample_format, int sample_rate, int channel_layout,
                                     int bytes_per_sample,
                                     qint64 timebase_num, qint64 timebase_den,
                                     qint64 monotonic_pts_increase) :
        bitrate(bitrate)
      ,number_of_channels(number_of_channels)
      ,sample_format(sample_format)
      ,sample_rate(sample_rate)
      ,channel_layout(channel_layout)
      ,bytes_per_sample(bytes_per_sample)
      ,timebase_num(timebase_num)
      ,timebase_den(timebase_den)
      ,monotonic_pts_increase(monotonic_pts_increase)
{
}
