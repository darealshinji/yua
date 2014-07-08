#include "video_encoder.h"

Video_Encoder::Video_Encoder(QObject *parent) :
        QObject(parent)
      ,mutex(NULL)
      ,currently_encoding(false)
      ,total_frames_to_encode(0)
      ,encode_img_convert_ctx(NULL)
      ,input_avpicture(NULL)
      ,output_avframe(NULL)
      ,ignore_frames(0)
      ,src_pix_fmt(AV_PIX_FMT_BGRA)
      ,pass(1)
      ,print_timestamp_debug_info(false)
      ,current_job_id(-1) //frames are initialized with job_id 0 (20130609)
{
}

Video_Encoder::~Video_Encoder() {
        clean_up_ffmpeg_crap();
}

void Video_Encoder::clean_up_ffmpeg_crap() {
        if (input_avpicture) {
                avpicture_free(input_avpicture);
                input_avpicture = NULL;
        }
        if (output_avframe) {
                avpicture_free(&dst_picture);
                avcodec_free_frame(&output_avframe);
                output_avframe = NULL;
        }
        if (encode_img_convert_ctx) {
                sws_freeContext(encode_img_convert_ctx);
                encode_img_convert_ctx = NULL;
        }
}

void Video_Encoder::set_pass(int new_pass) {
        pass = new_pass;
}

void Video_Encoder::start(Video_Information new_video_information, QString name, int id) {
        if (currently_encoding && pass != 2) return;
        currently_encoding = true;

        video_information = new_video_information;
        current_job_name = name;
        current_job_id = id;

        frames_encoded = total_frames_to_encode = 0;
        f_counter = video_information.f - 1;
        timestamps.clear();
        last_pts = 0;


        if (!temp_out_file.open()) {
                emit error_with_string(tr("Video_Encoder: failed to open temporary file"));
                return;
        }
        filename_string = temp_out_file.fileName()+"v.mp4";
        QByteArray filename_ba = filename_string.toLocal8Bit();
        char *filename_pointer = filename_ba.data();
        avformat_alloc_output_context2(&oc, NULL, NULL, filename_pointer);
        Q_ASSERT(oc);


        codec = avcodec_find_encoder(AV_CODEC_ID_H264);
        if (!codec) {
                fprintf(stderr, "Codec not found\n");
                exit(1);
        }


        //add to output stream context
        st = avformat_new_stream(oc, codec);
        if (!st) {
                fprintf(stderr, "Could not allocate stream\n");
                exit(1);
        }
        st->id = oc->nb_streams-1;
        //c = avcodec_alloc_context3(codec);
        c = st->codec;
        if (!c) {
                fprintf(stderr, "Could not allocate video codec context\n");
                exit(1);
        }

        /* Some formats want stream headers to be separate. */
        if (oc->oformat->flags & AVFMT_GLOBALHEADER) {
                c->flags |= CODEC_FLAG_GLOBAL_HEADER;
        }


        AVDictionary *opts = NULL;


        //





        av_dict_set(&opts, "preset", "veryslow", 0);

        //http://mewiki.project357.com/wiki/X264_Encoding_Suggestions#QuickTime_Player_X_compatibility
        //this doesn't work because we set the profile and/or level for every quality below (20130330)
        av_dict_set(&opts, "refs", "4", 0);
        av_dict_set(&opts, "qpmin", "4", 0);
        if (current_job_name == "MQ") { //it's a shame how gimped mq is, but then again compatibility is the best it's been since vhs was king (20130211)
                av_dict_set(&opts, "profile", "baseline", 0);
                av_dict_set(&opts, "level", "13", 0); //from anri
        } else {
                if (video_information.colorspace == AV_PIX_FMT_YUV420P) {
                        av_dict_set(&opts, "profile", "high", 0);
                } //otherwise it is set automatically to high422 or high444 (20130526)
                av_dict_set(&opts, "level", "41", 0); //bluray compatibility level
        }

        c->pix_fmt = (AVPixelFormat)video_information.colorspace;

        QString stat_file_name = temp_out_file.fileName()+".stats";
        stat_file_name_ba = stat_file_name.toLocal8Bit();
        char *stat_file_name_pointer = stat_file_name_ba.data();

        av_dict_set(&opts, "stats", stat_file_name_pointer, 0);

        if (pass == 1) {
                av_dict_set(&opts, "crf", "17", 0);
                av_dict_set(&opts, "fastfirstpass", "0", 0);
                c->flags |= CODEC_FLAG_PASS1;
        } else if (pass == 2) {
                c->bit_rate = video_information.bitrate_kbit * 1000;
                c->flags |= CODEC_FLAG_PASS2;
        }

        c->thread_count = 0;

        c->width = video_information.width_after_cropping;
        c->height = video_information.height_after_cropping;

        c->sample_aspect_ratio.num = st->sample_aspect_ratio.num = aspect_ratio_num * video_information.height;
        c->sample_aspect_ratio.den = st->sample_aspect_ratio.den = aspect_ratio_den * video_information.width;

        c->time_base.num = timebase_num;
        c->time_base.den = timebase_den;

        if (mutex) mutex->lock();
        int ret = avcodec_open2(c, codec, &opts);
        if (mutex) mutex->unlock();
        if (ret < 0) {
                qDebug() <<  "Could not open video codec";
                exit(1);
        }


        /* open the output file, if needed */
        if (avio_open(&oc->pb, filename_pointer, AVIO_FLAG_WRITE) < 0) {
                fprintf(stderr, "Error occurred when opening video output file\n");
                return;
        }

        /* Write the stream header, if any. */
        if (avformat_write_header(oc, NULL) < 0) {
                fprintf(stderr, "Error occurred when writing video header\n");
                return;
        }
        qDebug() << this << "after avformat_write_header(): we wanted timebase" << timebase_num << "/" << timebase_den
                 << "; we got" << c->time_base.num << "/" << c->time_base.den << "in the video codec and"
                 << st->time_base.num << "/" << st->time_base.den << "in the container";


        if (pass == 2) emit request_second_pass(); //to restart decoding from the beginning (20130113)
}

void Video_Encoder::set_estimated_total_frames_to_encode(int new_estimated_total_frames_to_encode) {
        estimated_total_frames_to_encode = new_estimated_total_frames_to_encode;
        qDebug() << this << "estimated_total_frames_to_encode set to" << estimated_total_frames_to_encode;
}

void Video_Encoder::set_aspect_ratio(int new_aspect_ratio_num, int new_aspect_ratio_den) {
        aspect_ratio_num = new_aspect_ratio_num;
        aspect_ratio_den = new_aspect_ratio_den;
}

void Video_Encoder::set_timebase(qint64 num, qint64 den) {
        src_timebase_num = num;
        src_timebase_den = den;

        timebase_num = num;
        timebase_den = den;
}

void Video_Encoder::finish(int new_total_video_frames_to_encode) {
        if (!currently_encoding) return;

        total_frames_to_encode = int((new_total_video_frames_to_encode+(video_information.f-1)) / video_information.f);
        frames_to_ignore = (new_total_video_frames_to_encode-1) % video_information.f; //these frames will arrive after we close x264 and we need to ignore them so they don't end up as part of the second pass, etc (20130211)
        qDebug() << this << "total_frames_to_encode set to" << total_frames_to_encode << "video_information.f" << video_information.f << "frames_to_ignore" << frames_to_ignore;
        if (frames_encoded >= total_frames_to_encode) {
                qDebug() << "encoder: finish():" << frames_encoded << ">=" << total_frames_to_encode << ", closing x264 write channel";
                return close_out();
        }
}

void Video_Encoder::close_out() {
        ignore_frames += frames_to_ignore;

        for (int got_output = 1; got_output;) {
                got_output = ffmpeg_encode_frame(NULL);
        }

        av_write_trailer(oc);

        if (mutex) mutex->lock();
        avcodec_close(st->codec);
        if (mutex) mutex->unlock();

        /* Free the streams. */
        for (uint i = 0; i < oc->nb_streams; i++) {
                av_freep(&oc->streams[i]->codec);
                av_freep(&oc->streams[i]);
        }

        avio_close(oc->pb);

        /* free the stream */
        av_free(oc);

        temp_out_file.close();


        qint64 bytes_written = QFileInfo(filename_string).size();
        double our_bitrate = (((double)bytes_written * 8.0) / 1024.0)   /   ((double)frames_encoded / (video_information.framerate / video_information.f));
        qDebug() << this << "our_bitrate" << our_bitrate << "video_information.bitrate" << video_information.bitrate_kbit;
        if (pass == 1 && our_bitrate > video_information.bitrate_kbit) {
                pass = 2;
                //output_avframe->pts = AV_NOPTS_VALUE; //required for pts autogeneration (e.g., statid) (20130316)
                start(video_information, current_job_name, current_job_id+1);
        } else {
                clean_up_ffmpeg_crap();
                currently_encoding = false;
                emit finished(filename_string, our_bitrate);
                pass = 1;
        }

        if (print_timestamp_debug_info) {
                qint64 last_timestamp = 0;
                qint64 i = 0;
                qint64 j = 0;
                qint64 last_d = -1;
                foreach (qint64 timestamp, timestamps) {
                        qint64 d = timestamp - last_timestamp;
                        if (d == last_d) {
                                ++j;
                        } else {
                                if (j) qDebug() << this << "... repeated" << j << "times";
                                j = 0;
                                if (i) qDebug() << this << "frame" << i << "delta" << d;
                        }
                        last_d = d;
                        last_timestamp = timestamp;
                        ++i;
                }
                if (j) qDebug() << this << "... repeated" << j << "times";
        }
}

void Video_Encoder::encode_frame(Frame frame) {
        if (!currently_encoding) {
                qDebug() << this << "ignoring field! this is a bug! (unless we just finished encoding all jobs?)";
                return;
        } else if (frame.job_id != current_job_id) {
                qDebug() << this << "ignoring field due frame job id" << frame.job_id << "!= current_job_id" << current_job_id;
                return;
        } else if (++f_counter != video_information.f) {
                //qDebug() << this << "ignoring field due to f_counter" << f_counter << "f" << video_information.f;
                return;
        }
        f_counter = 0;

        if (frame.isNull()) {
                qDebug() << this << "FATAL: null frame! this is a bug!";
                return;
        }
        if (frame.format() != QImage::Format_ARGB32) {
                qDebug() << this << "FATAL: frame.format()" << frame.format() << "!= QImage::Format_ARGB32";
                return;
        }
        if (frame.width() != video_information.width_after_cropping || frame.height() != video_information.height_after_cropping) {
                if (!frame.statid) {
                        qDebug() << this << "FATAL: frame.width()" << frame.width() << "!= video_information.width_after_cropping" << video_information.width_after_cropping << "|| frame.height()" << frame.height() << "!= video_information.height_after_cropping" << video_information.height_after_cropping;
                        exit(1);
                } else {
                        frame.image = scaler.scale(frame.image, video_information.width_after_cropping, video_information.height_after_cropping);
                }
        }

        write_frame(frame);

        emit add_progress(pass == 1 ? x264_firstpass_progress : x264_secondpass_progress, (double)frames_encoded / (double)estimated_total_frames_to_encode);
        if ((frames_encoded-1) % 10 == 0) {
                //qDebug() << this << "emit decode_to( 10 *" << video_information.f << ")";
                emit decode_to(10 * video_information.f);
        }

        if (total_frames_to_encode && frames_encoded >= total_frames_to_encode) {
                qDebug() << "encoder: encode_frame(): " << frames_encoded << ">=" << total_frames_to_encode << ", closing x264 write channel";
                return close_out();
        }
}

void Video_Encoder::write_frame(Frame frame) {
        AVPixelFormat output_colorspace = (AVPixelFormat)video_information.colorspace;

        if (!encode_img_convert_ctx)
                encode_img_convert_ctx = sws_getCachedContext(encode_img_convert_ctx,
                                                              video_information.width_after_cropping, video_information.height_after_cropping, src_pix_fmt,
                                                              video_information.width_after_cropping, video_information.height_after_cropping, output_colorspace,
                                                              SWS_LANCZOS, NULL, NULL, NULL);
        Q_ASSERT(encode_img_convert_ctx);

        if (!input_avpicture) {
                input_avpicture = (AVPicture *)av_malloc(sizeof(AVPicture));
                avpicture_alloc(input_avpicture, src_pix_fmt, video_information.width_after_cropping, video_information.height_after_cropping);
                input_length = avpicture_get_size(src_pix_fmt, video_information.width_after_cropping, video_information.height_after_cropping);
        }
        if (!output_avframe) {
                output_avframe = avcodec_alloc_frame();
                avpicture_alloc(&dst_picture, output_colorspace, video_information.width_after_cropping, video_information.height_after_cropping);
                /* copy data and linesize picture pointers to frame */
                *((AVPicture *)output_avframe) = dst_picture;
        }


        memcpy(input_avpicture->data[0], frame.bits(), input_length);

        sws_scale(encode_img_convert_ctx, input_avpicture->data, input_avpicture->linesize, 0, frame.height(),
                  output_avframe->data, output_avframe->linesize);

        AVRational src_timebase = {src_timebase_num, src_timebase_den};
        output_avframe->pts = av_rescale_q(frame.pts, src_timebase, st->codec->time_base); //was av_rescale_q(frame.pts, src_timebase, st->time_base);

        ffmpeg_encode_frame(output_avframe);

        ++frames_encoded;
}

int Video_Encoder::ffmpeg_encode_frame(AVFrame *frame_to_encode) {
        AVPacket pkt;
        av_init_packet(&pkt);
        pkt.data = NULL;    // packet data will be allocated by the encoder
        pkt.size = 0;

        if (frame_to_encode) {
                if (last_pts && frame_to_encode->pts <= last_pts) {
                        qDebug() << this << "ffmpeg_encode_frame(): frame_to_encode->pts adjusted forward from" << frame_to_encode->pts << "to" << last_pts+last_good_pts_interval;
                        frame_to_encode->pts = last_pts+last_good_pts_interval;
                } else {
                        last_good_pts_interval = frame_to_encode->pts - last_pts;
                }
                last_pts = frame_to_encode->pts;
        }
        if (print_timestamp_debug_info && frame_to_encode) {
                timestamps << frame_to_encode->pts;
                qDebug() << this << "encoding frame with timestamp" << frame_to_encode->pts;
        }

        int got_output;
        if (avcodec_encode_video2(c, &pkt, frame_to_encode, &got_output) < 0) {
                qDebug() << "avcodec_encode_video2 error";
                fflush(stdout);
                fflush(stderr);
                exit(1);
        }
        if (got_output) {
                if (c->coded_frame->key_frame)
                        pkt.flags |= AV_PKT_FLAG_KEY;

                pkt.stream_index = st->index;
                pkt.pts = av_rescale_q(pkt.pts, st->codec->time_base, st->time_base);
                pkt.dts = av_rescale_q(pkt.dts, st->codec->time_base, st->time_base);

                /* Write the compressed frame to the media file. */
                if (av_interleaved_write_frame(oc, &pkt) != 0) {
                        qDebug() << "av_interleaved_write_frame error";
                        exit(1);
                }
        }

        return got_output;
}
