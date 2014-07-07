#ifndef DECODER_MOTHERSHIP_H
#define DECODER_MOTHERSHIP_H

#include <QtGui>

#include "nasty_decoder.h"
#include "statid_generator.h"
#include "threadsafequeue.h"
#include "scaler.h"

class Decoder_Mothership : public QObject {
        Q_OBJECT
public:
        explicit Decoder_Mothership(QObject *parent = 0);
        ThreadSafeQueue<int> *seek_jobs;
        int get_number_of_open_files();
        bool has_audio;

signals:
        void decoding_finished(int);
        void error_with_string(QString);
        void index_progress(qint64, qint64);
        void index_begin();
        void index_finish();
        void normalize_begin(QString);
        void normalize_finish(QString);
        void successfully_opened(QStringList, Video_Information, bool, Audio_Information);

        void pretty_info(QString,QString);
        void write_to_log(QString, QString);

        void open_file(QString);
        void video_frame_out(Frame);
        void audio_data_out(QByteArray, qint64);
        void set_decoder_abort();

        void set_statid_line(int, QString);
        void set_statid_sda_logo(bool);
        void set_statid_audio_commentary(bool);

        void restore_ui();
        void update_statid_size(int, int);

public slots:
        void open(QString);
        void append(QStringList);
        void seek();
        void decoding_start(int, int, int);
        void set_decode_to(int);
        void force_interlaced(bool);
        void set_cli_force(bool, bool);
        void set_target_number_of_audio_channels(int);
        void set_append_statid(bool);
        void set_statid_size(int, int);
        void delete_between(int, int);
        void set_encoding_abort();

private slots:
        void successfully_opened_slot(QString);
        void open_failure();
        void video_frame_in(Frame);
        void audio_data_in(QByteArray, qint64, qint64, qint64);
        void decoding_finished_slot(int, int);

private:
        QList<Decoder *> decoders;
        qint64 reference_timebase_num, reference_timebase_den, video_pts_offset, audio_pts_offset, next_video_pts, next_audio_pts, last_video_pts, last_audio_pts;
        bool at_segment_break, force_audio_splice, force_video_splice;
        int need_audio_splice, need_video_splice;
        Video_Information video_info;
        Audio_Information audio_info;
        qint64 rescale_ts(qint64 x, qint64 p, qint64 q, qint64 r, qint64 s);

        //from http://stackoverflow.com/a/4229930/272601
        template <class T>
        T gcd(T a, T b) {
                forever {
                        if (a == 0) return b;
                        b %= a;
                        if (b == 0) return a;
                        a %= b;
                }
        }

        template <class T>
        T lcm(T a, T b) {
                T temp = gcd(a, b);
                return temp ? (a / temp * b) : 0;
        }

        bool cli_force_interlaced, cli_force_progressive;
        void make_new_decoder(QString);
        int number_of_open_decoders;
        Scaler scaler;
        QHash<int, int> decoder_frameoffset;
        int total_frames_decoded;
        int current_section_number;
        bool currently_encoding;
        void pad_audio(qint64 amount);
        void pad_video(qint64 amount);
        bool append_statid, using_statid;
        void vate_statid(bool);
        void calculate_globals();
        int statid_width, statid_height;
        bool print_timestamp_debug_info;
        QList<QPair<int, int> > trims;
        void apply_trims();
        QList<int> globalframe_decoder;
        QList<int> trimmedframe_globalframe;
        QList<QPair<int, QPair<int, int> > > decoder_trims;
        void decode_next_section();
        int leftover_decode_to;
        qint64 original_number_of_frames;
        int current_number_of_audio_channels;
        bool need_to_adjust_trims_for_statid;
        void add_to_trims(int number);
        QImage last_frame_image;
        QStringList open_file_names;
        QStringList filenames_to_append;
        void append_next_file();
        int video_frames_emitted;
        int current_job_id;
        int using_temporary_trims;
        bool encoding_abort;

};

#endif // DECODER_MOTHERSHIP_H
