#ifndef AUDIO_ENCODER_H
#define AUDIO_ENCODER_H

#include <QObject>
#include <QDir>
#include <QTemporaryFile>
#include <QDebug>
#include <QMutex>

extern "C" {
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libavutil/channel_layout.h>
#include <libavresample/avresample.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include "audio_information.h"

class Audio_Encoder : public QObject {
        Q_OBJECT
public:
        explicit Audio_Encoder(QObject *parent = 0);
        ~Audio_Encoder();
        QMutex *mutex;
        
signals:
        void error_with_string(QString);
        void write_to_log(QString, QString);
        void finished(QString);

public slots:
        void start(Audio_Information);
        void encode_samples(QByteArray, qint64);
        void set_source_information(Audio_Information);
        void finish();
        void set_target_number_of_audio_channels(int);

private:
        QTemporaryFile temp_out_file;
        bool currently_encoding;
        Audio_Information source_info, target_info;

        AVCodec *codec;
        AVCodecContext *c;
        AVFrame *output_frame;
        int output_frame_size;
        uchar *output_frame_data;
        void encode_data();
        bool audio_is_planar;
        AVFormatContext *oc;
        AVStream *st;
        QString filename_string;
        QList<QByteArray> input_frames;
        QList<qint64> input_timestamps;
        int output_frame_pos;
        int encode_frame(AVFrame *);
        AVBitStreamFilterContext *bsfc;
        void convert_headers(AVPacket *);
        void encode_packet(AVPacket *);
        double leader_seconds;
        QList<qint64> timestamps;
        bool print_timestamp_debug_info;
        qint64 last_pts, last_good_pts_interval;
};

#endif // AUDIO_ENCODER_H
