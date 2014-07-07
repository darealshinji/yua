#ifndef JOB_H
#define JOB_H

#include "audio_information.h"
#include "video_information.h"

#include <QString>

class Job {
public:
        Job();
        Job(QString name, Video_Information, Audio_Information);

        QString name;
        int id;
        Video_Information video;
        Audio_Information audio;
};

#endif // JOB_H
