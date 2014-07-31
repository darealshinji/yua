#ifndef YUA_H
#define YUA_H

#include <QApplication>
#include <QMainWindow>
#include <QSlider>
#include <QRadioButton>
#include <QPushButton>
#include <QCheckBox>
#include <QMenu>
#include <QMenuBar>
#include <QButtonGroup>
#include <QDesktopServices>
#include <QProgressBar>
#include <QTextEdit>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QProcess>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QDesktopWidget>
#if QT_VERSION >= 0x050000
#include <QtConcurrent/QtConcurrent>
#endif

// needed for displayHelp() and displayVersion()
#include <iostream>

#include "webcam_display.h"
#include "decoder_mothership.h"
#include "deinterlacer.h"
#include "audio_encoder.h"
#include "audio_information.h"
#include "video_encoder.h"
#include "progress_type.h"
#include "video_information.h"
#include "job.h"
#include "frame.h"
#include "throttle.h"
#include "power_management.h"


#define ICON QIcon(":/yua_64x64.png")


class Yua : public QMainWindow {
        Q_OBJECT
public:
        Yua(QWidget *parent = 0);
        ~Yua();
        QMutex ffmpeg_mutex;

protected:
        void dragEnterEvent(QDragEnterEvent *event);
        void dragMoveEvent(QDragMoveEvent *event);
        void dragLeaveEvent(QDragLeaveEvent *event);
        void dropEvent(QDropEvent *event);

signals:
        void open_file(QString);
        void append_files(QStringList);
        void input_image_ready(QImage);
        void no_change_image_ready(QImage);
        void one_pixel_bob_image_ready(QImage);
        void alternate_one_pixel_bob_image_ready(QImage);
        void retard_bob_image_ready(QImage);
        void alternate_retard_bob_image_ready(QImage);
        void seek();
        void deinterlace(Frame, bool);
        void make_preview(Frame);
        void push_down_one_line(Frame, Frame);
        void retard_bob(Frame, Frame, bool);
        void audio_encoding_start(Audio_Information);
        void decoding_start(int,int,int);
        void queue_video_frame(Frame);
        void video_encoding_start(Video_Information, QString, int);
        void video_encoding_finish(int);
        void set_estimated_total_frames_to_encode(int);
        void set_preview_size(int, int);
        void set_statid_size(int, int);
        void force_interlaced(bool);
        void preview_ready(QImage);
        void preview_ready_d4(QImage);
        void double_height(Frame,bool);
        void set_aspect_ratio(int, int);
        void encode_frame(Frame);
        void audio_encoding_finish();
        void set_cli_force(bool, bool);
        void audio_data_out(QByteArray, qint64);
        void set_audio_source_information(Audio_Information);
        void set_video_timebase(qint64,qint64);
        void set_append_statid(bool);
        void set_statid_line(int, QString);
        void set_statid_sda_logo(bool);
        void set_statid_audio_commentary(bool);
        void delete_between(int,int);
        void set_target_number_of_audio_channels(int);
        void encode_frame_preview(QImage);
        void set_pass(int);
        void set_encoding_abort();

private slots:
        void open_dialog();
        void open(QString);
        void append_dialog();
        void append(QStringList);
        void new_image(Frame);
        void slider_moved(int);
        void top_bottom_field(Frame, Frame);
        void one_pixel_bobbed(Frame, Frame, Frame, Frame);
        void retard_bobbed(Frame, Frame, bool);
        void field_dominance_changed();
        void aspect_ratio_changed();
        void d_changed();
        void f_changed();
        void produce_filtered_images();
        void encoding_start();
        void decoding_finished(int);
        void audio_encoding_finished(QString);
        void video_encoding_finished(QString, int);

        void process_read_stderr();
        void process_read_stdout();
        void process_error(QProcess::ProcessError);
        void process_finished(int, QProcess::ExitStatus);

        void add_progress(enum Progress_Type, double);
        void restart_decoder();
        void set_status(QString);
        void about();
        void error_slot(QString);
        void interlaced_changed();
        void new_preview(Frame);
        void d4_filter_display_clicked();
        void write_to_log(QString, QString);
        void index_begun();
        void index_finished();
        void normalize_begun(QString filename);
        void normalize_finished(QString filename);
        void index_progress(qint64, qint64);
        void height_doubled(Frame);
        void open_successful(QStringList filenames, Video_Information new_video_info, bool new_has_audio, Audio_Information new_audio_info);
        void statid_toggled(bool);
        void process_current_image();
        void output_directory_choose();
        void queue_frame_slot(Frame);

        void trim_set_start_frame();
        void trim_set_end_frame();
        void go_to_trim_start_frame();
        void go_to_trim_end_frame();

        void audio_data_in(QByteArray, qint64);

        void screenshot_save();
        void set_pretty_info(QString,QString);
        void power_management_slot();
        void statid_changed();

        void delete_selection();
        void restore_ui();
        void go_to_frame_dialog();

        void display_clicked(QMouseEvent *);
        void crop_checkbox_clicked(bool);

        void save_settings();
        void load_settings();

        void stop_button_pressed();

        void add_audio_commentary();
        void ffmpeg_process_error(QProcess::ProcessError);
        void ffmpeg_process_finished(int, QProcess::ExitStatus);
        void audio_commentary_muxer_process_error(QProcess::ProcessError error);
        void audio_commentary_muxer_process_finished(int exit_code, QProcess::ExitStatus status);

        void exit_yua();
        void save_settings_before_exiting();

private:
        void closeEvent(QCloseEvent *event);

        QMenu *trayIconMenu;
        QAction *restoreAction;
        QAction *quitAction;

        bool is_even_field();

        Webcam_Display preview_display, no_change_display, one_pixel_bob_display, alternate_one_pixel_bob_display, retard_bob_display, alternate_retard_bob_display;

        Decoder_Mothership decoder;
        QThread decoder_thread;

        Deinterlacer deinterlacer, deinterlacer2;
        QThread deinterlacer_thread, deinterlacer2_thread;

        QSlider slider;

        QRadioButton *progressive_button, *interlaced_button,
        *tff_button, *bff_button,
        *standard_button, *widescreen_button,
        *no_change_button, *one_pixel_bob_button, *alternate_one_pixel_bob_button, *retard_bob_button, *alternate_retard_bob_button,
        *f1_button, *f2_button, *f3_button,
        *fdp_2d_button, *fdp_3d_button,
        *d4_button, *d1_button;
        QPushButton *stop_button;
        QList<QRadioButton *> buttons;

        QCheckBox *xq_button, *iq_button, *hq_button, *mq_button, *lq_button;
        QList<QCheckBox *> checkboxes;

        QMenu *file_menu, *trim_menu, *screenshot_menu, *tools_menu, *help_menu;
        QList<QAction *> actions;
        QAction *append_action, *encode_action, *save_settings_action, *load_settings_action;
        QAction *shut_down_action, *reboot_action, *sleep_action, *hibernate_action, *lock_action;


        Frame top_field_image, bottom_field_image;

        Audio_Encoder audio_encoder;
        QThread audio_encoder_thread;

        Video_Encoder video_encoder;
        QThread video_encoder_thread;

        bool currently_encoding, audio_finished, video_finished;

        QString video_temp_filename, audio_temp_filename;

        void mux();
        void encode_jobs();

        QProcess muxer_process;

        QProgressBar progress_bar;
        QTextEdit status_textedit;

        QRegExp mp4box_regex_1, mp4box_regex_2;

        QList<Job> jobs;
        Job current_job, previous_job;

        ThreadSafeQueue<int> seek_jobs;
        void seek_to(int);
        void vate_ui(bool);
        QStringList resource_outpaths;
        QHash<QString, QString> resource_pathname;
        int native_width, native_height;
        double aspect_ratio;
        int lq_width, lq_height,
        mq_width, mq_height,
        hq_width, hq_height,
        iq_width, iq_height,
        xq_width, xq_height;
        int round_up_to_mod_4(int);
        int round_up_to_mod_2(int);
        void set_width_from_height_clamped_to_native_width_and_mod_2_height(int &, int &);
        void set_sizes();
        Frame current_image;
        void set_slider_max();
        bool has_audio;
        QWidget *d4_options_container_widget;
        QGroupBox *field_dominance_group_box, *aspect_ratio_group_box;
        void append_file(QFile &, QFile &);
        QImage make_selected_image(Frame);
        QImage add_d4_caption_to_image(QString, Frame);
        void process_muxer_output(QString);
        QTemporaryFile temporary_directory_file;
        QString temp_dir;
//        QDialog *index_progress_widget;
        qint64 slider_resume_value;
        QString temp_out_filename, audio_commentary_temp_out_filename;
        void show_in_finder(QString);
        QGroupBox *statid_group_box;
        QLineEdit *statid_line_1, *statid_line_2, *statid_line_3;
        QCheckBox *audio_commentary_checkbox, *sda_logo_checkbox;
        QSize preview_size;
        QSettings *settings;
        QLabel *output_path_current_label;
        QLineEdit *output_name_edit;
        void set_output_path_current_label();
        QGroupBox *output_path_group_box;
        QCheckBox *crop_checkbox, *mono_checkbox, *shut_down_when_done_checkbox;
        QString output_path;
        bool cli_jobs_to_process;
        bool cli_files_to_append;
        QStringList cli_filenames_to_append;
        QStringList cli_qualities;
        bool force_field_dominance;
        Throttle preview_throttle, progress_throttle;
        int trim_start_frame, trim_end_frame;
        QLabel trimpoints_label;
        void make_trimpoints_label();
        QImage screenshot_image;
        QLabel pretty_video_info_label, pretty_audio_info_label;
        QDateTime display_timestamp_datetime;
        QImage draw_frame_number_and_timestamp_on_image(Frame);
        bool cli_aspect_ratio;

        Video_Information video_info;
        Audio_Information audio_info;

        QQueue<qint64> frameid_queue;
        QMap<qint64, Frame> frameid_frame;

        QStringList set_recommended_qualities();

        bool cli_set_qualities_automatically;
        void go_to_frame(int);
        QRect cropping_rect;
        QSize cropping_rect_set_from_size;
        bool crop_moving_left, crop_moving_right, crop_moving_top, crop_moving_bottom;
        Frame current_preview_image;
        void draw_preview();
        void handle_d4_cropping_overlay(Frame &);
        QRect calculate_new_cropping_rect(QSize);
        QSize scale_size_cropped(QSize);
        QImage draw_cropping_rect_on_image(QImage);
        bool encoding_audio;

        Power_Management power_management;

        int cli_trim_start_frame, cli_trim_end_frame;
        bool encoding_abort;
        void set_quality_checkboxes(QStringList qualities);

        QProcess ffmpeg_process, audio_commentary_muxer_process;
        QStringList audio_commentary_mp4s_to_mux;
        void mux_next_audio_commentary_mp4();

        int aspect_ratio_num, aspect_ratio_den;
        void emit_set_statid_size();

        QString desktop_path_string;

        int xq_bitrate_kbit;
};

#endif // YUA_H
