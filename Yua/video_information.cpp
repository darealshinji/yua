#include "video_information.h"

Video_Information::Video_Information(int bitrate,
                                     double framerate,
                                     int f,
                                     int width,
                                     int height,
                                     int width_after_cropping,
                                     int height_after_cropping,
                                     int colorspace,
                                     bool interlaced,
                                     qint64 monotonic_pts_increase,
                                     qint64 timebase_num,
                                     qint64 timebase_den,
                                     bool tff,
                                     qint64 number_of_frames) :
        bitrate_kbit(bitrate)
      ,framerate(framerate)
      ,f(f)
      ,width(width)
      ,height(height)
      ,width_after_cropping(width_after_cropping)
      ,height_after_cropping(height_after_cropping)
      ,colorspace(colorspace)
      ,interlaced(interlaced)
      ,monotonic_pts_increase(monotonic_pts_increase)
      ,timebase_num(timebase_num)
      ,timebase_den(timebase_den)
      ,tff(tff)
      ,number_of_frames(number_of_frames)
{
}
