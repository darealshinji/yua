#ifndef DECODER_H
#define DECODER_H

#include <QtGui>

#include "audio_information.h"
#include "video_information.h"
#include "frame.h"
#include "yua_util.h"

#include <math.h>
#include <limits>

class Decoder : public QObject {
        Q_OBJECT
public:
        bool has_audio;
        bool cli_force_interlaced, cli_force_progressive;
        Video_Information video_info;
        Audio_Information audio_info;

signals:
        void image_ready(Frame);
        void decoding_finished(int, int);
        void error_with_string(QString);
        void index_progress(qint64, qint64);
        void index_begin();
        void index_finish();
        void normalize_begin(QString);
        void normalize_finish(QString);
        void open_failure();
        void successfully_opened(QString);
        void audio_samples_ready(QByteArray, qint64, qint64, qint64);
        void pretty_info(QString,QString);
        void write_to_log(QString, QString);

public slots:
        virtual void seek_to(int, bool = false) = 0;
        virtual void decoding_start(int, int, int) = 0;
        virtual void set_decode_to(int) = 0;
        virtual void force_interlaced(bool) = 0;
        virtual void set_target_number_of_audio_channels(int) = 0;
        virtual void set_decoder_abort() = 0;

};

#endif // DECODER_H
