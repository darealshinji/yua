#ifndef VIDEO_ENCODER_H
#define VIDEO_ENCODER_H

#include <QtGui>

extern "C" {
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/opt.h"
#include "libavutil/samplefmt.h"
}

#include <math.h>

#include "progress_type.h"
#include "video_information.h"
#include "frame.h"
#include "scaler.h"

class Video_Encoder : public QObject {
        Q_OBJECT
public:
        explicit Video_Encoder(QObject *parent = 0);
        ~Video_Encoder();
        QMutex *mutex;

signals:
        void finished(QString, int);
        void error_with_string(QString);
        void decode_to(int);
        void add_progress(enum Progress_Type, double);
        void request_second_pass();
        void write_to_log(QString, QString);
        
public slots:
        void start(Video_Information, QString, int);
        void finish(int total_frames_to_encode);
        void set_estimated_total_frames_to_encode(int);
        void set_aspect_ratio(int, int);
        void set_timebase(qint64, qint64);
        void encode_frame(Frame);
        void set_pass(int);

private:
        bool currently_encoding;
        QTemporaryFile temp_out_file;
        int total_frames_to_encode;
        double estimated_total_frames_to_encode;
        Video_Information video_information;
        QString current_job_name;
        int f_counter;
        int frames_encoded;
        QString path_to_encoder;
        SwsContext *encode_img_convert_ctx;
        AVPicture *input_avpicture;
        AVFrame *output_avframe;
        int input_length;
        void clean_up_ffmpeg_crap();
        void write_frame(Frame);
        void close_out();
        QByteArray stat_file_name_ba;
        int ignore_frames;
        int frames_to_ignore;
        int aspect_ratio_num, aspect_ratio_den;
        qint64 timebase_num, timebase_den;
        AVFormatContext *oc;
        AVStream *st;
        AVCodec *codec;
        AVCodecContext *c;
        QString filename_string;
        AVPicture dst_picture;
        int ffmpeg_encode_frame(AVFrame *frame_to_encode);
        qint64 video_monotonic_pts_increase;
        qint64 src_timebase_num, src_timebase_den;
        AVPixelFormat src_pix_fmt;
        int pass;
        QList<qint64> timestamps;
        bool print_timestamp_debug_info;
        int current_job_id;
        Scaler scaler;
        qint64 last_pts, last_good_pts_interval;
};

#endif // VIDEO_ENCODER_H
