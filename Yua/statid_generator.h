#ifndef STATID_GENERATOR_H
#define STATID_GENERATOR_H

#include <QtGui>

#include "decoder.h"

class Statid_Generator : public Decoder {
        Q_OBJECT
public:
        explicit Statid_Generator();

        //these are not called "slots" here because their slotness is inherited from Decoder (20130602)
        void seek_to(int, bool = false);
        void decoding_start(int, int, int);
        void set_decode_to(int);
        void force_interlaced(bool);
        void set_target_number_of_audio_channels(int);
        void set_decoder_abort();

        void reset_video_info();

public slots:
        void set_statid_line(int, QString);
        void set_statid_sda_logo(bool);
        void set_statid_audio_commentary(bool);
        void update_size(int, int);

private:
        QImage make_statid_image();
        bool use_sda_logo, use_audio_commentary;
        QString statid_line_1, statid_line_2, statid_line_3;
        void emit_frame_with_pts(qint64);
        void emit_audio_with_pts(qint64);
        qint64 frames_decoded, decode_to, frames_to_decode;
        void decode();
        int current_frame_number;
        QImage statid_image;
        qint64 rescale_ts(qint64 x, qint64 p, qint64 q, qint64 r, qint64 s);

};

#endif // STATID_GENERATOR_H
