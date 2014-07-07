#include "statid_generator.h"

Statid_Generator::Statid_Generator() {
        has_audio = false;
        reset_video_info();
}

void Statid_Generator::reset_video_info() {
        video_info.monotonic_pts_increase = 1;
        video_info.number_of_frames = 5;
        video_info.timebase_num = 1;
        video_info.timebase_den = 1;
        current_frame_number = 0;
}

qint64 Statid_Generator::rescale_ts(qint64 x, qint64 p, qint64 q, qint64 r, qint64 s) {
        return av_rescale_q(x, (AVRational){p,q}, (AVRational){r,s});
        return (x*p*s)/(r*q);
}

void Statid_Generator::emit_audio_with_pts(qint64 pts) {
        qint64 amount = rescale_ts(video_info.monotonic_pts_increase*2, video_info.timebase_num,video_info.timebase_den, audio_info.timebase_num,audio_info.timebase_den);
        //qDebug() << this << "emit_frame_with_pts() audio" << amount << "(" << ((double)amount * (double)audio_info.timebase_num / audio_info.timebase_den) * 1000.0 << "ms )";
        //qint64 frame_length = rescale_ts(audio_info.monotonic_pts_increase, audio_info.timebase_num,audio_info.timebase_den, video_info.timebase_num,video_info.timebase_den);
        qint64 frame_length = audio_info.monotonic_pts_increase;
        qint64 number_of_frames_to_add = amount / frame_length;
        qint64 leftover = amount % frame_length;
        //qDebug() << this << "emit_frame_with_pts(): adding" << number_of_frames_to_add << "frames of" << frame_length << "with" << leftover << "left over";
        qint64 audio_pts = rescale_ts(pts, video_info.timebase_num,video_info.timebase_den, audio_info.timebase_num,audio_info.timebase_den);
        if (!number_of_frames_to_add && leftover) { //emit a short frame - otherwise if audio frames are longer than video frames we will never emit any audio - THIS IS UNTESTED (20130609)
                number_of_frames_to_add = 1;
                frame_length = leftover;
                leftover = 0;
        }
        while (number_of_frames_to_add) {
                qint64 this_frame_length = frame_length;
                if (number_of_frames_to_add == 1) { //this is the last frame so the leftover needs to go in too or else players that don't pay attention to non strictly monotonic timestamps will slowly desync over successive splices with leftover (20130601)
                        this_frame_length += leftover;
                }
                qint64 samples_to_add = audio_info.number_of_channels * rescale_ts(this_frame_length, audio_info.timebase_num,audio_info.timebase_den, 1,48000);
                qint64 size_to_add = samples_to_add*2; //assumes 16-bit samples
                QByteArray ba(size_to_add, 0);
                //qDebug() << this << "emit_frame_with_pts(): adding audio frame with pts" << audio_pts << "frame length" << this_frame_length;

                //qint64 out_pts = rescale_ts(pts, reference_timebase_num,reference_timebase_den, audio_info.timebase_num,audio_info.timebase_den);
                qint64 out_pts = audio_pts;
                emit audio_samples_ready(ba, out_pts, audio_info.timebase_num, audio_info.timebase_den);

                --number_of_frames_to_add;

                audio_pts += frame_length;
        }
}

void Statid_Generator::emit_frame_with_pts(qint64 pts) {
        Frame frame(make_statid_image());
        frame.timebase_num = video_info.timebase_num;
        frame.timebase_den = video_info.timebase_den;
        frame.pts = pts;
        frame.second_field_pts = frame.pts + video_info.monotonic_pts_increase;
        frame.statid = true;
        //if (frame.timebase_den != 0) qDebug() << this << "emitting frame with timestamp" << ((double)frame.pts * (double)frame.timebase_num / frame.timebase_den);
        emit image_ready(frame);
}

void Statid_Generator::seek_to(int seek_to_frame, bool) {
        qint64 pts = seek_to_frame*video_info.monotonic_pts_increase*2;
        emit_frame_with_pts(pts);
        emit_audio_with_pts(pts);
        current_frame_number = seek_to_frame;
}

void Statid_Generator::decode() {
        while (decode_to && frames_decoded < frames_to_decode && current_frame_number < video_info.number_of_frames) {
                qint64 pts = current_frame_number*video_info.monotonic_pts_increase*2;
                emit_frame_with_pts(pts);
                emit_audio_with_pts(pts);
                ++current_frame_number;
                ++frames_decoded;
                --decode_to;
        }
        if (frames_decoded == frames_to_decode || current_frame_number == video_info.number_of_frames) {
                emit decoding_finished(frames_decoded, decode_to);
        }
}

void Statid_Generator::decoding_start(int start, int stop, int decode_to_initial_value) {
        frames_decoded = 0;
        decode_to = decode_to_initial_value+1;
        frames_to_decode = stop-start+1;
        if (stop == 0) {
                frames_to_decode = video_info.number_of_frames-start+1;
        }
        current_frame_number = start;
        decode();
}

QImage Statid_Generator::make_statid_image() {
        if (!statid_image.isNull()) return statid_image.copy();

        int windows_difference = 0;
#ifndef Q_OS_MAC
        windows_difference = 6;
#endif

        QImage out_image = QImage(":/statid_4x3.png");
        double old_height = out_image.height();
        double height_factor = 1.0;
        bool widescreen = ((double)video_info.width / video_info.height > 1.55);
        QDir application_dir;
        application_dir.cd(QCoreApplication::applicationDirPath());
#ifdef Q_OS_MAC
        application_dir.cd("../../.."); //get out of the bundle
#endif
        QFileInfo alternate_statid_info(application_dir, "statid.png");
        if (alternate_statid_info.exists()) {
                out_image = QImage(alternate_statid_info.filePath());
                height_factor = out_image.height() / old_height;
        } else {
                if (video_info.width > 640) {
                        out_image = QImage(":/statid_4x3_large.png");
                        height_factor = out_image.height() / old_height;
                }
                if (widescreen) {
                        out_image = QImage(":/statid_16x9_large.png");
                        height_factor = out_image.height() / old_height;
                }

                if (!use_sda_logo) {
                        out_image.fill(Qt::black);
                }
        }

        out_image = out_image.convertToFormat(QImage::Format_ARGB32);

        QPainter painter(&out_image);
        painter.setPen(Qt::white);

        QFont line1_font("Verdana", (32-windows_difference)*height_factor);
        line1_font.setBold(true);
        QFontMetrics line1_metrics(line1_font);
        while (line1_metrics.width(statid_line_1) > out_image.width()-10) {
                line1_font.setPointSize(line1_font.pointSize()-1);
                line1_metrics = QFontMetrics(line1_font);
        }
        painter.setFont(line1_font);
        painter.drawText(0, widescreen ? (240-10-36-10+40)*height_factor : (240-36-10   +30)*height_factor, out_image.width(), out_image.height(), Qt::AlignHCenter, statid_line_1);

        QFont line2_font("Verdana", (32-windows_difference)*height_factor);
        line2_font.setBold(true);
        QFontMetrics line2_metrics(line2_font);
        while (line2_metrics.width(statid_line_2) > out_image.width()-10) {
                line2_font.setPointSize(line2_font.pointSize()-1);
                line2_metrics = QFontMetrics(line2_font);
        }
        painter.setFont(line2_font);
        painter.drawText(0, widescreen ? (360-72-36-10+90-20)*height_factor : (360-36-36-10+30)*height_factor, out_image.width(), out_image.height(), Qt::AlignHCenter, statid_line_2);

        QFont line3_font("Verdana", (32-windows_difference)*height_factor);
        line3_font.setBold(true);
        QFontMetrics line3_metrics(line3_font);
        while (line3_metrics.width(statid_line_3) > out_image.width()-10) {
                line3_font.setPointSize(line3_font.pointSize()-1);
                line3_metrics = QFontMetrics(line3_font);
        }
        painter.setFont(line3_font);
        painter.drawText(0, widescreen ? (400-72-36-10+90-10)*height_factor : (400-36-36   +30)*height_factor, out_image.width(), out_image.height(), Qt::AlignHCenter, statid_line_3);

        painter.setPen(QColor(0xe1, 0xce, 0x8b));
        QFont commentary_font("Verdana", (24-windows_difference)*height_factor);
        painter.setFont(commentary_font);
        if (use_audio_commentary) {
                painter.drawText(0, out_image.height()-40*height_factor, out_image.width(), out_image.height(), Qt::AlignHCenter, "Audio commentary on track 2");
        }
        painter.end();

        out_image = out_image.scaled(QSize(video_info.width, video_info.height), Qt::IgnoreAspectRatio, Qt::SmoothTransformation).convertToFormat(QImage::Format_ARGB32);
        statid_image = out_image.copy();

        return out_image;
}

void Statid_Generator::set_statid_line(int line, QString string) {
        if (line == 1) statid_line_1 = string;
        else if (line == 2) statid_line_2 = string;
        else if (line == 3) statid_line_3 = string;
        else qDebug() << this << "set_statid_line(): unknown line number" << line << "received! this is a bug!";
        statid_image = QImage();
}

void Statid_Generator::set_statid_sda_logo(bool boolean) {
        use_sda_logo = boolean;
        statid_image = QImage();
}

void Statid_Generator::set_statid_audio_commentary(bool boolean) {
        use_audio_commentary = boolean;
        statid_image = QImage();
}

void Statid_Generator::force_interlaced(bool boolean) {
        qDebug() << this << "force_interlaced:" << boolean;
        video_info.interlaced = boolean;
        statid_image = QImage();
}

void Statid_Generator::set_decode_to(int new_decode_to) {
        if (video_info.interlaced) new_decode_to = (new_decode_to+1) / 2;
        decode_to += new_decode_to;
        decode();
}

void Statid_Generator::set_target_number_of_audio_channels(int number) {
        audio_info.number_of_channels = number;
}

void Statid_Generator::update_size(int width, int height) {
        video_info.width = width;
        video_info.height = height;
        statid_image = QImage();
}

void Statid_Generator::set_decoder_abort() {
        //meh - we'll be done soon enough anyway (20130623)
}
