#ifndef NASTY_DECODER_H
#define NASTY_DECODER_H

#include <QtGui>

#include "decoder.h"
#include "fps_conversion.h"

class Nasty_Decoder : public Decoder {
        Q_OBJECT
public:
        explicit Nasty_Decoder();
        ~Nasty_Decoder();

        //these are not called "slots" here because their slotness is inherited from Decoder (20130602)
        void seek_to(int, bool = false);
        void decoding_start(int, int, int);
        void set_decode_to(int);
        void force_interlaced(bool);
        void set_target_number_of_audio_channels(int);
        void set_decoder_abort();

signals:

public slots:
        void open(QString);

private:
        AVFormatContext *pFormatCtx;
        int videoStream, audioStream;
        SwsContext *encode_img_convert_ctx, *interlaced_convert_ctx;
        AVCodecContext *vCodecCtx, *aCodecCtx;
        AVCodec *vCodec, *aCodec;
        AVFrame *pFrame, *pFrameRGB;
        uint8_t *buffer;
        int numBytes;

        bool currently_encoding;

        void clean_up();
        void set_ffmpeg_crap_null();

        qint64 video_frames_decoded;
        bool at_end;

        QList<qint64> frame_keyframe;
        QHash<qint64, qint64> keyframe_dts;
        QHash<qint64, qint64> keyframe_pos;
        int decode_to;
        bool interlaced_yv12_conversion_needed;
        bool determined_interlacing;
        void emit_decoding_finished();
        void yv12_deinterlacing_check_and_set();
        int number_of_audio_planes, audio_bytes_per_sample;
        bool audio_is_planar;
        int video_frames_to_decode;

        int seeking_method;
        enum {
                seek_by_byte,
                seek_by_dts,
                seek_by_frame,
        };

        double audio_normalization_factor;
        bool determined_audio_normalization_factor;
        bool audio_sample_format_is_float, audio_sample_format_is_double, audio_sample_format_is_floating_point,
                        audio_sample_format_is_uint8, audio_sample_format_is_int16, audio_sample_format_is_int32, audio_sample_format_is_integer;
        AVPixelFormat dest_pix_fmt;

        AVAudioResampleContext *avr;
        Audio_Information target_info;
        void audio_resampler_initialize();
        void decode();
        qint64 video_monotonic_pts_increase;
        qint64 rescale_ts(qint64 x, qint64 p, qint64 q, qint64 r, qint64 s);
        bool abort;

        void determine_audio_normalization_factor();
        qint64 file_length;
        QString currently_open_filename;
        void determine_audio_normalization_process_avframe(AVFrame *avframe);
        double audio_level_max_double;
        quint8 audio_level_max_uint8;
        qint16 audio_level_max_int16;
        qint32 audio_level_max_int32;
        void determine_audio_normalization_factor_finish();
};

#endif // NASTY_DECODER_H
