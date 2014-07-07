#ifndef VIDEO_INFORMATION_H
#define VIDEO_INFORMATION_H

#include <QtGlobal>

class Video_Information {
public:
        Video_Information(int = 0,
                          double = 0,
                          int = 1,

                          int = 0,
                          int = 0,

                          int = 0,
                          int = 0,

                          int = 0,

                          bool = false,
                          qint64 = 1,
                          qint64 = 1,
                          qint64 = 1,
                          bool = true,
                          qint64 = 0
                          );

        int bitrate_kbit;
        double framerate; //frames per second; for fields per second, double this
        int f;
        int width, height;
        int width_after_cropping, height_after_cropping;
        int colorspace;

        bool interlaced;
        qint64 monotonic_pts_increase;
        qint64 timebase_num, timebase_den;
        bool tff;
        qint64 number_of_frames;
};

#endif // VIDEO_INFORMATION_H
