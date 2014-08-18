#include "decoder_mothership.h"

Decoder_Mothership::Decoder_Mothership(QObject *parent) :
        QObject(parent)
      ,number_of_open_decoders(0)
      ,currently_encoding(false)
      ,append_statid(false)
      ,using_statid(false)
      ,statid_width(0)
      ,statid_height(0)
      ,print_timestamp_debug_info(false)
      ,original_number_of_frames(0)
      ,need_to_adjust_trims_for_statid(false)
      ,current_job_id(-2)
      ,using_temporary_trims(0)
{
}

int Decoder_Mothership::get_number_of_open_files() {
        return number_of_open_decoders;
}

void Decoder_Mothership::make_new_decoder(QString filename) {
        Nasty_Decoder *decoder = new Nasty_Decoder;

        decoder->cli_force_interlaced = cli_force_interlaced;
        decoder->cli_force_progressive = cli_force_progressive;

        connect(decoder, SIGNAL(error_with_string(QString)), this, SIGNAL(error_with_string(QString)));
        connect(decoder, SIGNAL(index_begin()), this, SIGNAL(index_begin()));
        connect(decoder, SIGNAL(index_finish()), this, SIGNAL(index_finish()));
        connect(decoder, SIGNAL(normalize_begin(QString)), this, SIGNAL(normalize_begin(QString)));
        connect(decoder, SIGNAL(normalize_finish(QString)), this, SIGNAL(normalize_finish(QString)));
        connect(decoder, SIGNAL(index_progress(qint64,qint64)), this, SIGNAL(index_progress(qint64,qint64)));

        connect(decoder, SIGNAL(pretty_info(QString,QString)), this, SIGNAL(pretty_info(QString,QString)));

        connect(this, SIGNAL(open_file(QString)), decoder, SLOT(open(QString)));
        connect(decoder, SIGNAL(open_failure()), this, SLOT(open_failure()));
        connect(decoder, SIGNAL(successfully_opened(QString)), this, SLOT(successfully_opened_slot(QString)));
        emit open_file(filename);
}

void Decoder_Mothership::open(QString filename) {
        foreach (Decoder *decoder, decoders) {
                decoder->deleteLater();
        }
        decoders.clear();
        using_statid = false;
        number_of_open_decoders = 0;
        open_file_names.clear();
        trims.clear();

        return make_new_decoder(filename);
}

void Decoder_Mothership::append_next_file() {
        if (filenames_to_append.isEmpty()) {
                if (number_of_open_decoders) {
                        emit restore_ui();
                }
                return;
        }
        QString filename = filenames_to_append.takeFirst();
        return make_new_decoder(filename);
}

void Decoder_Mothership::append(QStringList filenames) {
        if (!filenames.isEmpty()) {
                filenames_to_append = filenames;
                return append_next_file();
        }
}

void Decoder_Mothership::open_failure() {
        Nasty_Decoder *decoder = qobject_cast<Nasty_Decoder *>(QObject::sender());
        delete decoder;
        filenames_to_append.clear();
        if (number_of_open_decoders) emit restore_ui(); //we tried to append and the user still has at least one file open so we need to reactivate the ui (20130607)
}

void Decoder_Mothership::successfully_opened_slot(QString filename) {
        Nasty_Decoder *new_decoder = qobject_cast<Nasty_Decoder *>(QObject::sender());

        disconnect(this, SIGNAL(open_file(QString)), new_decoder, SLOT(open(QString)));

        connect(new_decoder, SIGNAL(image_ready(Frame)), this, SLOT(video_frame_in(Frame)));
        connect(new_decoder, SIGNAL(audio_samples_ready(QByteArray, qint64, qint64, qint64)), this, SLOT(audio_data_in(QByteArray, qint64, qint64, qint64)));
        connect(new_decoder, SIGNAL(decoding_finished(int,int)), this, SLOT(decoding_finished_slot(int,int)));
        connect(this, SIGNAL(set_decoder_abort()), new_decoder, SLOT(set_decoder_abort()));

        open_file_names << QFileInfo(filename).fileName();

        if (using_statid) {
                decoders.insert(decoders.size()-1, new_decoder);
        } else {
                decoders << new_decoder;
        }
        ++number_of_open_decoders;
        if (number_of_open_decoders == 1 || (using_statid && number_of_open_decoders == 3)) {
                video_info = new_decoder->video_info;
                audio_info = new_decoder->audio_info;
        }
        if (append_statid && !using_statid) {
                vate_statid(true);
        }
        calculate_globals();

        return append_next_file();
}

void Decoder_Mothership::calculate_globals() {
        if (using_statid) {
                Statid_Generator *generator = qobject_cast<Statid_Generator *>(decoders.front());
                generator->reset_video_info();
        }
        video_info.number_of_frames = 0;
        video_info.timebase_num = video_info.timebase_den = audio_info.timebase_num = audio_info.timebase_den = 1;
        double audio_min_m = std::numeric_limits<double>::max();
        double video_min_m = std::numeric_limits<double>::max();
        qint64 video_winning_timebase_num, video_winning_timebase_den, audio_winning_timebase_num, audio_winning_timebase_den;
        video_winning_timebase_num = video_winning_timebase_den = audio_winning_timebase_num = audio_winning_timebase_den = 1;
        has_audio = false;
        video_info.interlaced = false;
        foreach (Decoder *decoder, decoders) {
                video_info.number_of_frames += decoder->video_info.number_of_frames;

                video_info.timebase_den = lcm(video_info.timebase_den, decoder->video_info.timebase_den);
                audio_info.timebase_den = lcm(audio_info.timebase_den, decoder->audio_info.timebase_den);
                //
                video_info.timebase_num = lcm(video_info.timebase_num, decoder->video_info.timebase_num);
                audio_info.timebase_num = lcm(audio_info.timebase_num, decoder->audio_info.timebase_num);
                //

                double video_m = (double)decoder->video_info.monotonic_pts_increase * (double)decoder->video_info.timebase_num / (double)decoder->video_info.timebase_den;
                if (video_m < video_min_m) {
                        video_min_m = video_m;
                        video_info.monotonic_pts_increase = decoder->video_info.monotonic_pts_increase;
                        video_winning_timebase_num = decoder->video_info.timebase_num;
                        video_winning_timebase_den = decoder->video_info.timebase_den;
                }

                video_info.width = qMax(video_info.width, decoder->video_info.width);
                video_info.height = qMax(video_info.height, decoder->video_info.height);

                if (decoder->has_audio) {
                        has_audio = true;

                        double audio_m = (double)decoder->audio_info.monotonic_pts_increase * (double)decoder->audio_info.timebase_num / (double)decoder->audio_info.timebase_den;
                        if (audio_m < audio_min_m) {
                                audio_min_m = audio_m;
                                audio_info.monotonic_pts_increase = decoder->audio_info.monotonic_pts_increase;
                                audio_winning_timebase_num = decoder->audio_info.timebase_num;
                                audio_winning_timebase_den = decoder->audio_info.timebase_den;
                        }

                        //qDebug() << this << decoder << "decoder->audio_info.number_of_channels" << decoder->audio_info.number_of_channels;
                        if (decoder->audio_info.number_of_channels > audio_info.number_of_channels) {
                                audio_info.number_of_channels = decoder->audio_info.number_of_channels;
                                audio_info.channel_layout = decoder->audio_info.channel_layout;
                        }
                }

                if (decoder->video_info.interlaced) {
                        video_info.interlaced = true;
                        video_info.tff = decoder->video_info.tff;
                }
        }
        if (has_audio) {
                qDebug() << "decided on" << audio_info.number_of_channels << "audio channels";
                set_target_number_of_audio_channels(audio_info.number_of_channels);
        }

        //
        if (!(decoders.size() == 1 || (using_statid && decoders.size() == 3))) { //no need to do this again since it was already run through the avisynth continuing fractions library in the Nasty_Decoder (20130914)
                unsigned num, den;
                FPS_Conversion::float_to_fps((double)video_info.timebase_den / video_info.timebase_num, den, num);

                qDebug() << this << "changed timebase" << video_info.timebase_num << "/" << video_info.timebase_den
                         << "to" << num << "/" << den << "with avisynth library";

                video_info.timebase_num = num;
                video_info.timebase_den = den;
        }
        //

        video_info.monotonic_pts_increase = rescale_ts(video_info.monotonic_pts_increase, video_winning_timebase_num,video_winning_timebase_den, video_info.timebase_num,video_info.timebase_den);
        //if (!video_info.interlaced) video_info.monotonic_pts_increase /= 2;
        if (has_audio) audio_info.monotonic_pts_increase = rescale_ts(audio_info.monotonic_pts_increase, audio_winning_timebase_num,audio_winning_timebase_den, audio_info.timebase_num,audio_info.timebase_den);
        video_info.framerate = 1.0 / ((double)video_info.monotonic_pts_increase * (double)video_info.timebase_num / video_info.timebase_den);

        if (using_statid) {
                Statid_Generator *generator = qobject_cast<Statid_Generator *>(decoders.front());

                video_info.number_of_frames -= generator->video_info.number_of_frames * 2; // *2 because there are two statid generators

                generator->video_info = video_info;
                if (!video_info.interlaced) generator->video_info.monotonic_pts_increase /= 2;
                generator->video_info.number_of_frames = 5.0 / (((double)generator->video_info.monotonic_pts_increase*2.0) * (double)generator->video_info.timebase_num / generator->video_info.timebase_den);
                qDebug() << this << "gave" << generator->video_info.number_of_frames << "frames to each generator; this video_info.number_of_frames is" << video_info.number_of_frames;

                video_info.number_of_frames += generator->video_info.number_of_frames * 2; // *2 because there are two statid generators

                if (need_to_adjust_trims_for_statid) {
                        add_to_trims(generator->video_info.number_of_frames);
                }

                if (has_audio) generator->audio_info = audio_info;
        }
        need_to_adjust_trims_for_statid = false;

        qDebug() << this << QSize(video_info.width, video_info.height)
                 << "video timebase den" << video_info.timebase_den << "audio timebase den" << audio_info.timebase_den
                 << "number of frames" << video_info.number_of_frames
                 << "has_audio" << has_audio
                 << "monotonic_pts_increase video and audio" << video_info.monotonic_pts_increase << audio_info.monotonic_pts_increase
                 << "video equal to" << ((double)video_info.monotonic_pts_increase * (double)video_info.timebase_num / video_info.timebase_den) * 1000.0 << "ms"
                 << video_info.framerate << "fps"
                 << "interlaced" << video_info.interlaced
        ;

        reference_timebase_num = 1;
        reference_timebase_den = qMin((qint64)std::numeric_limits<int>::max(), lcm(video_info.timebase_den, audio_info.timebase_den));
        qDebug() << this << "reference_timebase_den set to" << reference_timebase_den << "based on video_info.timebase_den" << video_info.timebase_den << "and audio_info.timebase_den" << audio_info.timebase_den;

        globalframe_decoder.clear();
        decoder_frameoffset.clear();
        int decoder_number = 0;
        int frameoffset = 0;
        foreach (Decoder *decoder, decoders) {
                decoder_frameoffset.insert(decoder_number, frameoffset);
                qDebug() << this << "decoder" << decoder_number << "frameoffset" << frameoffset;
                for (int decoderframe = 0; decoderframe < decoder->video_info.number_of_frames; ++decoderframe) {
                        globalframe_decoder << decoder_number;
                }
                frameoffset += decoder->video_info.number_of_frames;
                ++decoder_number;
        }
        original_number_of_frames = video_info.number_of_frames;
        apply_trims();

        emit successfully_opened(open_file_names, video_info, has_audio, audio_info);
}

void Decoder_Mothership::set_append_statid(bool boolean) {
        append_statid = boolean;
        return vate_statid(append_statid);
}

void Decoder_Mothership::add_to_trims(int number) {
        QMutableListIterator<QPair<int, int> > i(trims);
        while (i.hasNext()) {
                QPair<int, int> pair = i.next();
                qDebug() << this << "considering trim pair" << pair.first << pair.second;
//                if (number < 0 && (pair.first < -number || pair.second < -number)) {
//                        qDebug() << this << "removing trim points" << pair.first << pair.second << "because one or more of them is <" << -number;
//                        i.remove();
//                        continue;
//                }
                pair.first += number;
                pair.second += number;
                qDebug() << this << "trim points are now" << pair.first << pair.second;
                i.setValue(pair);
        }
}

void Decoder_Mothership::vate_statid(bool boolean) {
        if (number_of_open_decoders == 0) return;
        if (using_statid == boolean) return;
        qDebug() << this << "vate_statid:" << boolean;
        if (!boolean) {
                Decoder *statid_generator = decoders.takeFirst();

                add_to_trims(-statid_generator->video_info.number_of_frames);

                statid_generator->deleteLater();
                statid_generator = decoders.takeLast(); //this is actually the same generator, but there's no harm in calling deleteLater() on it twice (20130605)
                statid_generator->deleteLater();
                number_of_open_decoders -= 2;
        } else {
                Statid_Generator *generator = new Statid_Generator;
                connect(this, SIGNAL(set_statid_line(int,QString)), generator, SLOT(set_statid_line(int,QString)));
                connect(this, SIGNAL(set_statid_sda_logo(bool)), generator, SLOT(set_statid_sda_logo(bool)));
                connect(this, SIGNAL(set_statid_audio_commentary(bool)), generator, SLOT(set_statid_audio_commentary(bool)));
                connect(this, SIGNAL(update_statid_size(int,int)), generator, SLOT(update_size(int,int)));
                connect(generator, SIGNAL(image_ready(Frame)), this, SLOT(video_frame_in(Frame)));
                connect(generator, SIGNAL(audio_samples_ready(QByteArray, qint64, qint64, qint64)), this, SLOT(audio_data_in(QByteArray, qint64, qint64, qint64)));
                connect(generator, SIGNAL(decoding_finished(int,int)), this, SLOT(decoding_finished_slot(int,int)));

                need_to_adjust_trims_for_statid = true; //we don't know the number of frames we need to add to our trims yet (20130607)

                decoders.push_front(generator);
                decoders.push_back(generator);
                number_of_open_decoders += 2;
        }
        using_statid = boolean;
        calculate_globals();
        emit update_statid_size(statid_width, statid_height);
}

void Decoder_Mothership::seek() {
        if (seek_jobs->isEmpty() || !number_of_open_decoders) return;

        int seek_to_trimmedframe = 0;
        while (!seek_jobs->isEmpty()) { //other threads only add to this queue, never take away from it, so this should be safe? (20130120)
                seek_to_trimmedframe = seek_jobs->dequeue();
        }

        int seek_to_globalframe = trimmedframe_globalframe.at(seek_to_trimmedframe);

        int decoder_number = globalframe_decoder.value(seek_to_globalframe);
        int seek_to_frame = seek_to_globalframe - decoder_frameoffset.value(decoder_number);
        //qDebug() << this << "decoder number" << decoder_number << "seek to" << seek_to_frame;
        decoders.at(decoder_number)->seek_to(seek_to_frame);
}

void Decoder_Mothership::decoding_start(int job_id, int trim_start_frame, int trim_end_frame) {
        current_job_id = job_id;

        video_pts_offset = audio_pts_offset = next_video_pts = next_audio_pts = 0;
        last_video_pts = last_audio_pts = -1;
        need_audio_splice = need_video_splice = force_audio_splice = force_video_splice = false;
        encoding_abort = false;

        total_frames_decoded = 0;
        video_frames_emitted = 0;
        current_section_number = 0;
        leftover_decode_to = 0;
        last_frame_image = QImage();

        if (trim_end_frame) {
                int cut_end_frame = video_info.number_of_frames-1;
                if (using_statid) cut_end_frame -= decoders.at(0)->video_info.number_of_frames; //don't cut into the statid
                if (cut_end_frame > trim_end_frame) {
                        trims << QPair<int, int>(trim_end_frame, cut_end_frame);
                        qDebug() << this << "adding temporary trim range" << trim_end_frame << "," << cut_end_frame;
                        ++using_temporary_trims;
                }
        }
        if (trim_start_frame) {
                int cut_start_frame = 0;
                if (using_statid) cut_start_frame = decoders.at(0)->video_info.number_of_frames; //don't cut into the statid (20130802)
                if (cut_start_frame < trim_start_frame) {
                        trims << QPair<int, int>(cut_start_frame, trim_start_frame);
                        qDebug() << this << "adding temporary trim range " << cut_start_frame << "," << trim_start_frame;
                        ++using_temporary_trims;
                }
        }
        apply_trims();

        currently_encoding = true;

        decode_next_section();
}

void Decoder_Mothership::decode_next_section() {
        int decoder_number = decoder_trims.at(current_section_number).first;
        int start_frame = decoder_trims.at(current_section_number).second.first;
        int end_frame = decoder_trims.at(current_section_number).second.second;
        at_segment_break = true;
        decoders.at(decoder_number)->decoding_start(start_frame, end_frame, leftover_decode_to);
        leftover_decode_to = 0;
}

void Decoder_Mothership::decoding_finished_slot(int frames_decoded, int leftover) {
        qDebug() << this << "decoding_finished_slot(): frames_decoded" << frames_decoded << "leftover" << leftover;
        if (!currently_encoding) return;
        ++current_section_number;
        total_frames_decoded += frames_decoded;
        leftover_decode_to += leftover;
        if (current_section_number >= decoder_trims.size() || encoding_abort) { //we are done
                encoding_abort = false;

                current_section_number = 0; //so that spurious calls to set_decode_to() don't crash us (20130601)

                while (using_temporary_trims) {
                        trims.removeLast();
                        --using_temporary_trims;
                }
                apply_trims();

                currently_encoding = false;
                qDebug() << this << "emit decoding_finished(): total_frames_decoded" << total_frames_decoded << "video_frames_emitted" << video_frames_emitted;
                emit decoding_finished(total_frames_decoded);
        } else {
                decode_next_section();
        }
}

void Decoder_Mothership::set_decode_to(int decode_to) {
        int decoder_number = decoder_trims.at(current_section_number).first;
        //qDebug() << this << "increment decoder number" << decoder_number << "decode_to by" << decode_to;
        decoders.at(decoder_number)->set_decode_to(decode_to);
}

void Decoder_Mothership::force_interlaced(bool boolean) {
        foreach (Decoder *decoder, decoders) {
                decoder->force_interlaced(boolean);
        }
        calculate_globals();
}

void Decoder_Mothership::set_cli_force(bool new_cli_force_interlaced, bool new_cli_force_progressive) {
        cli_force_interlaced = new_cli_force_interlaced;
        cli_force_progressive = new_cli_force_progressive;
}

void Decoder_Mothership::set_target_number_of_audio_channels(int number) {
        current_number_of_audio_channels = number ? number : audio_info.number_of_channels;
        foreach (Decoder *decoder, decoders) {
                qDebug() << "setting decoder" << decoder << "number of audio channels to" << number << "||" << audio_info.number_of_channels;
                decoder->set_target_number_of_audio_channels(current_number_of_audio_channels);
        }
}

qint64 Decoder_Mothership::rescale_ts(qint64 x, qint64 p, qint64 q, qint64 r, qint64 s) {
        return av_rescale_q(x, (AVRational){(int)p,(int)q}, (AVRational){(int)r,(int)s});
        return (x*p*s)/(r*q);
}

void Decoder_Mothership::video_frame_in(Frame frame) {
        if (need_video_splice) --need_video_splice;

        qint64 video_pts = rescale_ts(frame.pts, frame.timebase_num,frame.timebase_den, reference_timebase_num,reference_timebase_den);
        qint64 video_pts_2 = rescale_ts(frame.second_field_pts, frame.timebase_num,frame.timebase_den, reference_timebase_num,reference_timebase_den);

        if (print_timestamp_debug_info) if (currently_encoding) qDebug() << "v in" << frame.pts << frame.second_field_pts << "(reference timebase" << video_pts << video_pts_2 << ")";

        qint64 offset_video_pts = video_pts + video_pts_offset;
        if (at_segment_break || (offset_video_pts <= last_video_pts && !need_video_splice)) {
                if (!at_segment_break) if (currently_encoding) qDebug() << "v" << offset_video_pts << "<=" << last_video_pts;
                force_audio_splice = at_segment_break;
                at_segment_break = false;
                need_audio_splice = 100; //see comment next to need_video_splice = whatever (20130831)
                if (0) {
                        if (offset_video_pts != last_video_pts) {
                                video_pts_offset = next_video_pts - video_pts;
                                if (currently_encoding) qDebug() << "video_pts_offset is now" << video_pts_offset;
                        }
                } else {
                        video_pts_offset = next_video_pts - video_pts;
                        if (currently_encoding) qDebug() << "video_pts_offset is now" << video_pts_offset;
                }
        }
        else if (force_video_splice || (need_video_splice && offset_video_pts <= last_video_pts)) {
                if (currently_encoding) qDebug() << "need_video_splice";
                need_video_splice = force_video_splice = false;
                offset_video_pts = video_pts + audio_pts_offset;
                if (offset_video_pts < next_video_pts) {
                        qDebug() << "offset_video_pts" << offset_video_pts << "< next_video_pts" << next_video_pts;
                        if (currently_encoding) pad_audio(next_video_pts - offset_video_pts);
                        audio_pts_offset += next_video_pts - offset_video_pts;
                }
                if (offset_video_pts > next_video_pts) {
                        if (currently_encoding) pad_video(offset_video_pts - next_video_pts);
                }
                video_pts_offset = audio_pts_offset;
        }
        video_pts += video_pts_offset;
        video_pts_2 += video_pts_offset;
        last_video_pts = video_pts;
        if (video_info.interlaced) {
                last_video_pts = video_pts_2;
                next_video_pts = video_pts + rescale_ts(video_info.monotonic_pts_increase*2, video_info.timebase_num,video_info.timebase_den, reference_timebase_num,reference_timebase_den);
        } else {
                next_video_pts = video_pts + rescale_ts(video_info.monotonic_pts_increase, video_info.timebase_num,video_info.timebase_den, reference_timebase_num,reference_timebase_den);
        }
        if (print_timestamp_debug_info) if (currently_encoding) qDebug() << "v out"
                                         << video_pts << video_pts_2
                                         << "("
                                         << av_rescale_q(video_pts, (AVRational){(int)reference_timebase_num,(int)reference_timebase_den}, (AVRational){(int)video_info.timebase_num,(int)video_info.timebase_den})
                                         //<< ((double)av_rescale_q(video_pts, (AVRational){reference_timebase_num,reference_timebase_den}, (AVRational){video_info.timebase_num,video_info.timebase_den}) * (double)video_info.timebase_num / video_info.timebase_den) * 1000.0 << "ms"
                                         //<< rescale_ts(video_pts, reference_timebase_num,reference_timebase_den, video_info.timebase_num,video_info.timebase_den)
                                         //<< ((double)rescale_ts(video_pts, reference_timebase_num,reference_timebase_den, video_info.timebase_num,video_info.timebase_den) * (double)video_info.timebase_num / video_info.timebase_den) * 1000.0 << "ms"
                                         << av_rescale_q(video_pts_2, (AVRational){(int)reference_timebase_num,(int)reference_timebase_den}, (AVRational){(int)video_info.timebase_num,(int)video_info.timebase_den})
                                         //<< rescale_ts(video_pts_2, reference_timebase_num,reference_timebase_den, video_info.timebase_num,video_info.timebase_den)
                                         << ")"
                                         << ((double)video_pts * (double)reference_timebase_num / reference_timebase_den) * 1000.0 << "ms"
                                         << "need_video_splice" << need_video_splice;

        frame.pts = rescale_ts(video_pts, reference_timebase_num,reference_timebase_den, video_info.timebase_num,video_info.timebase_den);
        frame.second_field_pts = rescale_ts(video_pts_2, reference_timebase_num,reference_timebase_den, video_info.timebase_num,video_info.timebase_den);

        frame.job_id = current_job_id;

        if (!frame.statid) frame.image = scaler.scale(frame.image, video_info.width, video_info.height);

        last_frame_image = frame.image.copy();
        emit video_frame_out(frame);
        ++video_frames_emitted;
}

void Decoder_Mothership::audio_data_in(QByteArray data, qint64 audio_pts, qint64 audio_timebase_num, qint64 audio_timebase_den) {
        Statid_Generator *generator = qobject_cast<Statid_Generator *>(QObject::sender());
        if (generator && current_section_number != 0) {
                //qDebug() << this << "ignoring audio from final statid";
                return;
        }

        if (print_timestamp_debug_info) if (currently_encoding) qDebug() << "a in" << audio_pts;
        //qDebug() << this << "rescaling" << audio_pts << "from timebase" << audio_timebase_num << audio_timebase_den << "to" << reference_timebase_num << reference_timebase_den;
        audio_pts = rescale_ts(audio_pts, audio_timebase_num,audio_timebase_den, reference_timebase_num,reference_timebase_den);
        //qDebug() << this << " ->" << audio_pts;

        if (need_audio_splice) --need_audio_splice;

        qint64 offset_audio_pts = audio_pts + audio_pts_offset;
        if (at_segment_break || (offset_audio_pts <= last_audio_pts && !need_audio_splice)) {
                if (!at_segment_break) if (currently_encoding) qDebug() << "a" << offset_audio_pts << "<=" << last_audio_pts;
                force_video_splice = at_segment_break;
                at_segment_break = false;
                need_video_splice = 100; //if we don't see a video timewarp in this number of frames then give up - one's not coming - and if there is one then it isn't associated with this audio timewarp (20130831)
                audio_pts_offset = next_audio_pts - audio_pts;
                if (currently_encoding) qDebug() << "audio_pts_offset is now" << audio_pts_offset;
        }
        else if (force_audio_splice || (need_audio_splice && offset_audio_pts <= last_audio_pts)) {
                if (currently_encoding) qDebug() << "need_audio_splice";
                need_audio_splice = force_audio_splice = false;
                offset_audio_pts = audio_pts + video_pts_offset;
                if (offset_audio_pts < next_audio_pts) {
                        if (currently_encoding) pad_video(next_audio_pts - offset_audio_pts);
                        video_pts_offset += next_audio_pts - offset_audio_pts;
                }
                if (offset_audio_pts > next_audio_pts) {
                        if (currently_encoding) pad_audio(offset_audio_pts - next_audio_pts);
                }
                audio_pts_offset = video_pts_offset;
        }
        audio_pts += audio_pts_offset;
        last_audio_pts = audio_pts;
        next_audio_pts = audio_pts + rescale_ts(audio_info.monotonic_pts_increase, audio_info.timebase_num,audio_info.timebase_den, reference_timebase_num,reference_timebase_den);

        qint64 out_pts = rescale_ts(audio_pts, reference_timebase_num,reference_timebase_den, audio_info.timebase_num,audio_info.timebase_den);

        if (print_timestamp_debug_info) if (currently_encoding) qDebug() << "a out" << audio_pts << ((double)audio_pts * (double)reference_timebase_num / reference_timebase_den) * 1000.0 << "ms  -> " << out_pts;

        //
        //if (((double)audio_pts * (double)reference_timebase_num / reference_timebase_den) > 9) return;
        //

        emit audio_data_out(data, out_pts);
}

void Decoder_Mothership::pad_audio(qint64 amount) {
        //
        //return;
        //
        qDebug() << "pad audio" << amount << "(" << ((double)amount * (double)reference_timebase_num / reference_timebase_den) * 1000.0 << "ms )";
        qint64 frame_length = rescale_ts(audio_info.monotonic_pts_increase, audio_info.timebase_num,audio_info.timebase_den, reference_timebase_num,reference_timebase_den);
        qint64 number_of_frames_to_add = amount / frame_length;
        qint64 leftover = amount % frame_length;
        qDebug() << this << "pad_audio(): adding" << number_of_frames_to_add << "frames of" << frame_length << "with" << leftover << "left over";
        qint64 pts = last_audio_pts;
        if (pts == -1) pts = 0 - frame_length; //we are padding the beginning of the stream - last_audio_pts hasn't been set yet so back up one frame (20130601)
        while (number_of_frames_to_add) {
                pts += frame_length;

                qint64 this_frame_length = frame_length;
                if (number_of_frames_to_add == 1) this_frame_length += leftover; //this is the last frame so the leftover needs to go in too or else players that don't pay attention to non strictly monotonic timestamps will slowly desync over successive splices with leftover (20130601)
                qint64 samples_to_add = current_number_of_audio_channels * rescale_ts(this_frame_length, reference_timebase_num,reference_timebase_den, 1,48000);
                qint64 size_to_add = samples_to_add*2; //assumes 16-bit samples
                QByteArray ba(size_to_add, 0);
                qDebug() << this << "pad_audio(): adding audio frame with pts" << pts << "frame length" << this_frame_length << "size in bytes" << ba.size() << "current_number_of_audio_channels" << current_number_of_audio_channels;

                qint64 out_pts = rescale_ts(pts, reference_timebase_num,reference_timebase_den, audio_info.timebase_num,audio_info.timebase_den);
                emit audio_data_out(ba, out_pts);

                --number_of_frames_to_add;
        }
}

void Decoder_Mothership::pad_video(qint64 amount) {
        qDebug() << "pad video" << amount << "(" << ((double)amount * (double)reference_timebase_num / reference_timebase_den) * 1000.0 << "ms )";
        qint64 monotonic_pts_increase = video_info.monotonic_pts_increase;
        if (video_info.interlaced) monotonic_pts_increase *= 2; //we can only emit whole frames, not fields (20130607)
        qint64 frame_length = rescale_ts(monotonic_pts_increase, video_info.timebase_num,video_info.timebase_den, reference_timebase_num,reference_timebase_den);
        qint64 number_of_frames_to_add = amount / frame_length;
        qint64 leftover = amount % frame_length;
        qDebug() << this << "pad_video(): adding" << number_of_frames_to_add << "frames of" << frame_length << "with" << leftover << "left over";
        qint64 pts = last_video_pts;
        if (pts == -1) pts = 0 - frame_length; //we are padding the beginning of the stream - last_video_pts hasn't been set yet so back up one frame (20130607)
        while (number_of_frames_to_add) {
                pts += frame_length;

                if (last_frame_image.isNull()) {
                        last_frame_image = QImage(320, 240, QImage::Format_ARGB32);
                        last_frame_image.fill(Qt::black);
                        last_frame_image = scaler.scale(last_frame_image, video_info.width, video_info.height);
                }
                Frame frame(last_frame_image);

                qint64 out_pts = rescale_ts(pts, reference_timebase_num,reference_timebase_den, video_info.timebase_num,video_info.timebase_den);
                frame.pts = out_pts;
                frame.second_field_pts = frame.pts + video_info.monotonic_pts_increase; //if we are not in interlaced mode then this will never be used (20130607)

                frame.job_id = current_job_id;

                ++total_frames_decoded;
                emit video_frame_out(frame);
                ++video_frames_emitted;

                --number_of_frames_to_add;
        }
}

void Decoder_Mothership::set_statid_size(int width, int height) {
        qDebug() << this << "set_statid_size():" << width << height;
        statid_width = width;
        statid_height = height;
        emit update_statid_size(statid_width, statid_height);
}

void Decoder_Mothership::apply_trims() {
        QList<int> trimmedframe_globalframe_temp;
        trimmedframe_globalframe.clear();
        video_info.number_of_frames = original_number_of_frames;
        for (int frame = 0; frame < video_info.number_of_frames; ++frame) {
                trimmedframe_globalframe_temp << frame;
                trimmedframe_globalframe << frame;
        }
        for (int i = 0; i < trims.size(); ++i) {
                QPair<int, int> pair = trims.at(i);
                if (pair.first < 0 || pair.second < 0) continue;
                trimmedframe_globalframe.clear();
                for (int trimmedframe = 0; trimmedframe < trimmedframe_globalframe_temp.size(); ++trimmedframe) {
                        if (trimmedframe < pair.first || trimmedframe > pair.second) {
                                trimmedframe_globalframe << trimmedframe_globalframe_temp.at(trimmedframe);
                                //qDebug() << this << trimmedframe_globalframe_temp.at(trimmedframe);
                        } else {
                                --video_info.number_of_frames;
                        }
                }
                trimmedframe_globalframe_temp = trimmedframe_globalframe;
        }


        decoder_trims.clear();
        QList<QPair<int, int> > decoder_startframes, decoder_endframes;
        int last_globalframe = -2;
        int globalframe = 0;
        for (int trimmedframe = 0; trimmedframe < trimmedframe_globalframe.size(); ++trimmedframe) {
                globalframe = trimmedframe_globalframe.at(trimmedframe);
                if (globalframe != last_globalframe+1) {
                        int decoder_number = globalframe_decoder.value(globalframe);
                        int localframe = globalframe - decoder_frameoffset.value(decoder_number);
                        int last_decoder_number = globalframe_decoder.value(last_globalframe);
                        int last_localframe = last_globalframe - decoder_frameoffset.value(last_decoder_number);
                        decoder_startframes << QPair<int, int>(decoder_number, localframe);
                        if (last_globalframe != -2) decoder_endframes << QPair<int, int>(last_decoder_number, last_localframe);
                }
                last_globalframe = globalframe;
        }
        int final_decoder_number = globalframe_decoder.value(globalframe);
        int final_localframe = globalframe - decoder_frameoffset.value(final_decoder_number);
        decoder_endframes << QPair<int, int>(final_decoder_number, final_localframe);
        qDebug() << decoder_startframes;
        qDebug() << decoder_endframes;

        Q_ASSERT(decoder_startframes.size() == decoder_endframes.size());

        for (int i = 0; i < decoder_startframes.size(); ++i) {
                int start_decoder = decoder_startframes.at(i).first;
                int end_decoder = decoder_endframes.at(i).first;

                int start_frame = decoder_startframes.at(i).second;
                int end_frame = decoder_endframes.at(i).second;

                if (start_decoder == end_decoder) {
                        decoder_trims << QPair<int, QPair<int, int> >(start_decoder, QPair<int, int>(start_frame, end_frame));
                } else {
                        decoder_trims << QPair<int, QPair<int, int> >(start_decoder, QPair<int, int>(start_frame, 0));
                        for (int decoder_inc = start_decoder+1; decoder_inc < end_decoder; ++decoder_inc) {
                                decoder_trims << QPair<int, QPair<int, int> >(decoder_inc, QPair<int, int>(0, 0));
                        }
                        decoder_trims << QPair<int, QPair<int, int> >(end_decoder, QPair<int, int>(0, end_frame));
                }
        }

        qDebug() << "decoder_trims:" << decoder_trims;
}

void Decoder_Mothership::delete_between(int start, int end) {
        trims << QPair<int, int>(start, end);
        return apply_trims();
}

void Decoder_Mothership::set_encoding_abort() {
        encoding_abort = true;
        emit set_decoder_abort();
}
