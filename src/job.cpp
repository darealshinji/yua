#include "job.h"

Job::Job() {}

Job::Job(QString name, Video_Information video, Audio_Information audio) :
        name(name)
      ,id(qrand())
      ,video(video)
      ,audio(audio)
{
}
