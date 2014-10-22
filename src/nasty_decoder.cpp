#include "nasty_decoder.h"

Nasty_Decoder::Nasty_Decoder() :
       currently_encoding(false)
      ,dest_pix_fmt(AV_PIX_FMT_RGB32) //must match the constant set in video_encoder.cpp - was AV_PIX_FMT_RGB24 but i had alignment problems with non-mod 4 width input (20130429))
      ,avr(NULL)
{
        set_ffmpeg_crap_null();

        avcodec_register_all();
        av_register_all();

        clean_up();
}

void Nasty_Decoder::set_ffmpeg_crap_null() {
        pFormatCtx = NULL; //REQUIRED for use with avformat_open_input() (20120317)
        encode_img_convert_ctx = interlaced_convert_ctx = NULL;
        buffer = NULL;
        pFrameRGB = NULL;
        pFrame = NULL;
        vCodecCtx = NULL;
        aCodecCtx = NULL;
        avr = NULL;
}

void Nasty_Decoder::clean_up() {
        if (buffer) av_free(buffer);
        if (pFrameRGB) av_frame_free(&pFrameRGB);
        if (pFrame) av_frame_free(&pFrame);

        if (vCodecCtx) avcodec_close(vCodecCtx);
        if (aCodecCtx) avcodec_close(aCodecCtx);

        if (pFormatCtx) avformat_close_input(&pFormatCtx);

        if (encode_img_convert_ctx) sws_freeContext(encode_img_convert_ctx);
        if (interlaced_convert_ctx) sws_freeContext(interlaced_convert_ctx);

        if (avr) avresample_free(&avr);

        set_ffmpeg_crap_null();

        has_audio = at_end = interlaced_yv12_conversion_needed = determined_interlacing = false;
}

Nasty_Decoder::~Nasty_Decoder() {
        clean_up();
}

qint64 Nasty_Decoder::rescale_ts(qint64 x, qint64 p, qint64 q, qint64 r, qint64 s) {
        return av_rescale_q(x, (AVRational){(int)p,(int)q}, (AVRational){(int)r,(int)s});
        return (x*p*s)/(r*q);
}

void Nasty_Decoder::set_target_number_of_audio_channels(int number) {
        qDebug() << this << "target_info.number_of_channels set to" << number;
        target_info.number_of_channels = number;
        return audio_resampler_initialize();
}

void Nasty_Decoder::audio_resampler_initialize() {
        if (avr) avresample_free(&avr);
        avr = avresample_alloc_context();
        Q_ASSERT(avr);

        //keep this block in sync with the code in Audio_Encoder::start() (20130831)
        if (target_info.number_of_channels == 1) target_info.channel_layout = AV_CH_LAYOUT_MONO;
        else if (target_info.number_of_channels == 2) target_info.channel_layout = AV_CH_LAYOUT_STEREO;
        else { //passthrough
                target_info.number_of_channels = audio_info.number_of_channels;
                target_info.channel_layout = audio_info.channel_layout;
        }

        //all of the channel layouts supported by the aac encoder as of 20130831
        if (target_info.channel_layout != AV_CH_LAYOUT_MONO
                        && target_info.channel_layout != AV_CH_LAYOUT_STEREO
                        && target_info.channel_layout != AV_CH_LAYOUT_SURROUND
                        && target_info.channel_layout != AV_CH_LAYOUT_4POINT0
                        && target_info.channel_layout != AV_CH_LAYOUT_5POINT0_BACK
                        && target_info.channel_layout != AV_CH_LAYOUT_5POINT1_BACK
                        ) {
                if (target_info.number_of_channels == 1) {
                        target_info.channel_layout = AV_CH_LAYOUT_MONO;
                } else if (target_info.number_of_channels == 2) {
                        target_info.channel_layout = AV_CH_LAYOUT_STEREO;
                } else if (target_info.number_of_channels == 4) {
                        target_info.channel_layout = AV_CH_LAYOUT_4POINT0;
                } else if (target_info.number_of_channels == 5) {
                        target_info.channel_layout = AV_CH_LAYOUT_5POINT0_BACK;
                } else if (target_info.number_of_channels == 6) {
                        target_info.channel_layout = AV_CH_LAYOUT_5POINT1_BACK;
                } else { //if we have no idea what it is then just downmix to stereo (20130831)
                        target_info.number_of_channels = 2;
                        target_info.channel_layout = AV_CH_LAYOUT_STEREO;
                }
                qDebug() << this << "unsupported channel layout" << audio_info.channel_layout << "in input; converting to" << target_info.channel_layout;
        }

        qDebug() << this << "input channel layout is" << audio_info.channel_layout << "; output channel layout is" << target_info.channel_layout;
        av_opt_set_int(avr, "in_channel_layout",  audio_info.channel_layout, 0);
        av_opt_set_int(avr, "out_channel_layout", target_info.channel_layout, 0);
        av_opt_set_int(avr, "in_sample_rate",     audio_info.sample_rate, 0);
        av_opt_set_int(avr, "out_sample_rate",    48000, 0);
        av_opt_set_int(avr, "in_sample_fmt",      audio_info.sample_format, 0);
        av_opt_set_int(avr, "out_sample_fmt",     AV_SAMPLE_FMT_S16, 0);
        avresample_open(avr);
}

void Nasty_Decoder::force_interlaced(bool boolean) {
        //qDebug() << this << "force_interlaced(" << boolean << ")";
        video_info.interlaced = boolean;
        video_info.monotonic_pts_increase = video_monotonic_pts_increase;
        if (video_info.interlaced) {
                video_info.monotonic_pts_increase /= 2;
                yv12_deinterlacing_check_and_set();
        }
}

void Nasty_Decoder::yv12_deinterlacing_check_and_set() {
        if (vCodecCtx && src_pix_fmt == AV_PIX_FMT_YUV420P) { //we are going to convert to yuy2 using a different library first (20130123)
                interlaced_yv12_conversion_needed = true;
                if (interlaced_convert_ctx) sws_freeContext(interlaced_convert_ctx);
                interlaced_convert_ctx = Yua_Util::GetSwsContext(
                                        vCodecCtx->width, vCodecCtx->height/2, AV_PIX_FMT_YUYV422, vCodecCtx->colorspace, src_is_mjpeg_color_range ? AVCOL_RANGE_JPEG : AVCOL_RANGE_MPEG,
                                        vCodecCtx->width, vCodecCtx->height/2, dest_pix_fmt, vCodecCtx->colorspace, AVCOL_RANGE_MPEG,
                                        SWS_LANCZOS);
        }
}

//void Nasty_Decoder::emit_duration() {
//        int number_of_fields = number_of_frames;
//        if (interlaced) number_of_fields *= 2;
//        emit set_duration(number_of_fields);
//}

void Nasty_Decoder::emit_decoding_finished() {
        //qDebug() << this << "emit decoding_finished(" << fields_decoded << ", " << decode_to << ")";
        //FIXME purge avresample buffers http://libav.org/doxygen/master/group__lavr.html#_details
        abort = false;
        currently_encoding = false;
        emit decoding_finished(video_frames_decoded, decode_to);
}

//void Nasty_Decoder::emit_set_framerate() {
//        double fields_per_second = framerate;
//        if (interlaced) fields_per_second *= 2;
//        emit set_framerate(fields_per_second);
//}

void Nasty_Decoder::decoding_start(int start, int stop, int decode_to_initial_value) {
        currently_encoding = true;
        abort = false;
        video_frames_decoded = 0;
        decode_to = decode_to_initial_value+1;
        video_frames_to_decode = stop-start+1;
        if (stop == 0) {
                video_frames_to_decode = video_info.number_of_frames-start+1;
        }
        qDebug() << this << "decoding_start():" << start << "-" << stop << "; video_frames_to_decode" << video_frames_to_decode;
        determine_audio_normalization_factor();
        audio_resampler_initialize();
        seek_to(start);
}

void Nasty_Decoder::set_decode_to(int new_decode_to) {
        if (video_info.interlaced) new_decode_to = (new_decode_to+1) / 2;
        //qDebug() << this << "adding" << new_decode_to << "to decode_to" << decode_to;
        decode_to += new_decode_to;
        decode();
}

void Nasty_Decoder::determine_audio_normalization_process_avframe(AVFrame *avframe) {
        int number_of_samples = avframe->nb_samples;
        if (!audio_is_planar) number_of_samples *= avframe->channels;
        if (audio_sample_format_is_floating_point) {
                if (audio_sample_format_is_float) {
                        for (int plane_number = 0; plane_number < number_of_audio_planes; ++plane_number) {
                                float *sample = (float *)avframe->data[plane_number];
                                for (int i = 0; i < number_of_samples; ++i) audio_level_max_double = qMax(audio_level_max_double, qFabs(sample[i]));
                        }
                } else if (audio_sample_format_is_double) {
                        for (int plane_number = 0; plane_number < number_of_audio_planes; ++plane_number) {
                                double *sample = (double *)avframe->data[plane_number];
                                for (int i = 0; i < number_of_samples; ++i) audio_level_max_double = qMax(audio_level_max_double, qFabs(sample[i]));
                        }
                }
        } else if (audio_sample_format_is_integer) {
                if (audio_sample_format_is_uint8) {
                        for (int plane_number = 0; plane_number < number_of_audio_planes; ++plane_number) {
                                quint8 *sample = (quint8 *)avframe->data[plane_number];
                                for (int i = 0; i < number_of_samples; ++i) audio_level_max_uint8 = qMax(audio_level_max_uint8, sample[i]);
                        }
                } else if (audio_sample_format_is_int16) {
                        for (int plane_number = 0; plane_number < number_of_audio_planes; ++plane_number) {
                                qint16 *sample = (qint16 *)avframe->data[plane_number];
                                for (int i = 0; i < number_of_samples; ++i) audio_level_max_int16 = qMax(audio_level_max_int16, qAbs(sample[i]));
                        }
                } else if (audio_sample_format_is_int32) {
                        for (int plane_number = 0; plane_number < number_of_audio_planes; ++plane_number) {
                                qint32 *sample = (qint32 *)avframe->data[plane_number];
                                for (int i = 0; i < number_of_samples; ++i) audio_level_max_int32 = qMax(audio_level_max_int32, qAbs(sample[i]));
                        }
                }
        }
}

void Nasty_Decoder::determine_audio_normalization_factor() {
        if (determined_audio_normalization_factor || !has_audio) return;

        audio_level_max_double = std::numeric_limits<double>::min();
        audio_level_max_uint8 = std::numeric_limits<quint8>::min();
        audio_level_max_int16 = std::numeric_limits<qint16>::min();
        audio_level_max_int32 = std::numeric_limits<qint32>::min();

        emit normalize_begin(currently_open_filename);
        seek_to(0, true);
        qint64 frames_read = 0;
        forever {
                AVPacket packet;

                if (av_read_frame(pFormatCtx, &packet) < 0) {
                        break;
                }

                if (audioStream == packet.stream_index) {
                        if (++frames_read % 100 == 0) {
                                emit index_progress(pFormatCtx->pb->pos, file_length);
                        }

                        int frameFinished;
                        int ret = avcodec_decode_audio4(aCodecCtx, pFrame, &frameFinished, &packet);
                        if (ret > 0 && frameFinished) {
                                determine_audio_normalization_process_avframe(pFrame);
                        }
                }

                av_free_packet(&packet);
        }
        emit normalize_finish(currently_open_filename);

        return determine_audio_normalization_factor_finish();
}

void Nasty_Decoder::determine_audio_normalization_factor_finish() {
        if (audio_sample_format_is_floating_point) {
                audio_normalization_factor = 1.0 / audio_level_max_double;
        } else if (audio_sample_format_is_integer) {
                if (audio_sample_format_is_uint8) {
                        audio_normalization_factor = std::numeric_limits<quint8>::max() / (double)audio_level_max_uint8;
                } else if (audio_sample_format_is_int16) {
                        audio_normalization_factor = std::numeric_limits<qint16>::max() / (double)audio_level_max_int16;
                } else if (audio_sample_format_is_int32) {
                        audio_normalization_factor = std::numeric_limits<qint32>::max() / (double)audio_level_max_int32;
                }
        }
        qDebug() << this << "determined audio_normalization_factor" << audio_normalization_factor;
        determined_audio_normalization_factor = true;
}

void Nasty_Decoder::open(QString filename) {
        clean_up();

        QByteArray filename_ba = filename.toUtf8();
        char *filename_pointer = filename_ba.data();

        if (avformat_open_input(&pFormatCtx, filename_pointer, NULL, NULL) < 0) {
                emit error_with_string(tr("decoder: could not open file: %1").arg(filename));
                emit open_failure();
                return;
        }
        if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
                emit error_with_string(tr("decoder: could not find stream information in the file"));
                emit open_failure();
                return;
        }
        av_dump_format(pFormatCtx, 0, filename_pointer, 0);

        videoStream = -1; audioStream = -1;
        for (uint i=0; i < pFormatCtx->nb_streams; i++) {
                if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO && videoStream < 0) {
                        videoStream = i;
                }
                if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO && audioStream < 0) {
                        audioStream = i;
                }
        }

        if (videoStream == -1) {
                emit error_with_string(tr("didn't find a video stream"));
                emit open_failure();
                return;
        } else {
                vCodecCtx = pFormatCtx->streams[videoStream]->codec;
                vCodec = avcodec_find_decoder(vCodecCtx->codec_id);
                if (vCodec == NULL) {
                        emit error_with_string(tr("unsupported video codec: %1").arg(avcodec_get_name(vCodecCtx->codec_id)));
                        emit open_failure();
                        return;
                } else if (avcodec_open2(vCodecCtx, vCodec, NULL) < 0) {
                        emit error_with_string(tr("could not open video codec %1").arg(vCodec->long_name));
                        emit open_failure();
                        return;
                } else {
                        video_info.framerate = av_q2d(pFormatCtx->streams[videoStream]->r_frame_rate);
                        //thanks avisynth - this is like changefps(last.framerate) at the end of the script (20130704)
                        unsigned num, den;
                        //float n = (float)pFormatCtx->streams[videoStream]->time_base.den / pFormatCtx->streams[videoStream]->time_base.num;
                        if (pFormatCtx->streams[videoStream]->time_base.num == 83333 && pFormatCtx->streams[videoStream]->time_base.den == 5000000) { //one fraps special case added 20130831
                                num = 1;
                                den = 60;
                        } else {
                                FPS_Conversion::float_to_fps(video_info.framerate, den, num);

                                qDebug() << this << "changed timebase" << pFormatCtx->streams[videoStream]->time_base.num << "/" << pFormatCtx->streams[videoStream]->time_base.den
                                         << "to" << num << "/" << den << "with avisynth library";
                        }
                        video_info.timebase_num = num;
                        video_info.timebase_den = den*2;

                        //handle deprecated colorspaces (20140706)
                        switch (vCodecCtx->pix_fmt) {
                        case AV_PIX_FMT_YUVJ420P:
                                src_pix_fmt = AV_PIX_FMT_YUV420P;
                                src_is_mjpeg_color_range = true;
                                break;
                        case AV_PIX_FMT_YUVJ422P:
                                src_pix_fmt = AV_PIX_FMT_YUV422P;
                                src_is_mjpeg_color_range = true;
                                break;
                        case AV_PIX_FMT_YUVJ444P:
                                src_pix_fmt = AV_PIX_FMT_YUV444P;
                                src_is_mjpeg_color_range = true;
                                break;
                        case AV_PIX_FMT_YUVJ440P:
                                src_pix_fmt = AV_PIX_FMT_YUV440P;
                                src_is_mjpeg_color_range = true;
                                break;
                        case AV_PIX_FMT_YUVJ411P:
                                src_pix_fmt = PIX_FMT_YUV411P;
                                src_is_mjpeg_color_range = true;
                                break;
                        default:
                                src_pix_fmt = vCodecCtx->pix_fmt;
                                src_is_mjpeg_color_range = false;
                        }
                        qDebug() << this << "src_is_mjpeg_color_range set to" << src_is_mjpeg_color_range;

                        video_info.colorspace_standard = vCodecCtx->colorspace;
                }
        }

        if (audioStream == -1) {
                emit error_with_string(tr("didn't find an audio stream"));
        } else {
                aCodecCtx = pFormatCtx->streams[audioStream]->codec;
                aCodec = avcodec_find_decoder(aCodecCtx->codec_id);
                if (aCodec == NULL) {
                        emit error_with_string(tr("unsupported audio codec: %1").arg(avcodec_get_name(aCodecCtx->codec_id)));
                } else if (avcodec_open2(aCodecCtx, aCodec, NULL) < 0) {
                        emit error_with_string(tr("could not open audio codec %1").arg(aCodec->long_name));
                } else {
                        has_audio = true;

                        audio_is_planar = av_sample_fmt_is_planar(aCodecCtx->sample_fmt);
                        number_of_audio_planes = audio_is_planar ? aCodecCtx->channels : 1;

                        audio_sample_format_is_float = (AV_SAMPLE_FMT_FLT == aCodecCtx->sample_fmt || AV_SAMPLE_FMT_FLTP == aCodecCtx->sample_fmt);
                        audio_sample_format_is_double = (AV_SAMPLE_FMT_DBL == aCodecCtx->sample_fmt || AV_SAMPLE_FMT_DBLP == aCodecCtx->sample_fmt);
                        audio_sample_format_is_floating_point = (audio_sample_format_is_float || audio_sample_format_is_double);

                        audio_sample_format_is_uint8 = (AV_SAMPLE_FMT_U8 == aCodecCtx->sample_fmt || AV_SAMPLE_FMT_U8P == aCodecCtx->sample_fmt);
                        audio_sample_format_is_int16 = (AV_SAMPLE_FMT_S16 == aCodecCtx->sample_fmt || AV_SAMPLE_FMT_S16P == aCodecCtx->sample_fmt);
                        audio_sample_format_is_int32 = (AV_SAMPLE_FMT_S32 == aCodecCtx->sample_fmt || AV_SAMPLE_FMT_S32P == aCodecCtx->sample_fmt);
                        audio_sample_format_is_integer = (audio_sample_format_is_uint8 || audio_sample_format_is_int16 || audio_sample_format_is_int32);

                        audio_info.number_of_channels = aCodecCtx->channels;
                        audio_info.channel_layout = aCodecCtx->channel_layout;
                        if (!audio_info.channel_layout) {
                                qDebug() << this << "channel layout not found; running av_get_default_channel_layout() ...";
                                audio_info.channel_layout = av_get_default_channel_layout(aCodecCtx->channels);
                        }
                        qDebug() << this << "determined channel layout as" << audio_info.channel_layout;
                        if (!audio_info.channel_layout) {
                                emit error_with_string(tr("could not determine channel layout of input"));
                        }
                        audio_info.sample_format = aCodecCtx->sample_fmt;
                        audio_info.sample_rate = aCodecCtx->sample_rate;
                        audio_info.bytes_per_sample = av_get_bytes_per_sample(aCodecCtx->sample_fmt);
                        audio_info.timebase_num = pFormatCtx->streams[audioStream]->time_base.num;
                        audio_info.timebase_den = pFormatCtx->streams[audioStream]->time_base.den;

                        audio_bytes_per_sample = av_get_bytes_per_sample(aCodecCtx->sample_fmt);
                }
        }
        audio_normalization_factor = 1.0;
        determined_audio_normalization_factor = false;

        audio_level_max_double = std::numeric_limits<double>::min();
        audio_level_max_uint8 = std::numeric_limits<quint8>::min();
        audio_level_max_int16 = std::numeric_limits<qint16>::min();
        audio_level_max_int32 = std::numeric_limits<qint32>::min();



        pFrame = av_frame_alloc();
        pFrameRGB = av_frame_alloc();

        numBytes = avpicture_get_size(dest_pix_fmt, vCodecCtx->width, vCodecCtx->height);
        buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

        avpicture_fill((AVPicture *)pFrameRGB, buffer, dest_pix_fmt, vCodecCtx->width, vCodecCtx->height);

        encode_img_convert_ctx = Yua_Util::GetSwsContext(
                                vCodecCtx->width, vCodecCtx->height, src_pix_fmt, vCodecCtx->colorspace, src_is_mjpeg_color_range ? AVCOL_RANGE_JPEG : AVCOL_RANGE_MPEG,
                                vCodecCtx->width, vCodecCtx->height, dest_pix_fmt, vCodecCtx->colorspace, AVCOL_RANGE_MPEG,
                                SWS_LANCZOS);
        interlaced_convert_ctx = Yua_Util::GetSwsContext(
                                vCodecCtx->width, vCodecCtx->height/2, src_pix_fmt, vCodecCtx->colorspace, src_is_mjpeg_color_range ? AVCOL_RANGE_JPEG : AVCOL_RANGE_MPEG,
                                vCodecCtx->width, vCodecCtx->height/2, dest_pix_fmt, vCodecCtx->colorspace, AVCOL_RANGE_MPEG,
                                SWS_LANCZOS);


        //index the stream
        frame_keyframe.clear();
        keyframe_dts.clear();
        keyframe_pos.clear();
        qint64 frames_read = 0;
        qint64 keyframes_read = 0;
        qint64 last_keyframe = 0;
        qint64 last_last_keyframe = 0;
        qint64 estimated_total_frames = pFormatCtx->streams[videoStream]->nb_frames;
        file_length = QFileInfo(filename).size();
        int video_frames_decoded = 0;
        QList<qint64> video_frame_timestamp, audio_frame_timestamp;
        bool determining_audio_normalization_factor_now = true;

        seeking_method = seek_by_dts;
        if (QString(pFormatCtx->iformat->name) == "avi" && (
                                vCodecCtx->codec_id == AV_CODEC_ID_RAWVIDEO
                                || vCodecCtx->codec_id == AV_CODEC_ID_LAGARITH
                                || vCodecCtx->codec_id == AV_CODEC_ID_HUFFYUV
                                || vCodecCtx->codec_id == AV_CODEC_ID_FFVHUFF
                                ) && pFormatCtx->streams[videoStream]->nb_frames) {
                qDebug() << this << "seeking_method set to seek_by_frame because this is an all-frames-are-keyframes avi";
                seeking_method = seek_by_frame;
                video_info.number_of_frames = pFormatCtx->streams[videoStream]->nb_frames;
                determining_audio_normalization_factor_now = false;
        } else if (!pFormatCtx->streams[videoStream]->nb_frames) {
                qDebug() << this << "seeking_method set to seek_by_byte because there may be retrograde timestamps in this file";
                seeking_method = seek_by_byte;
        }

        emit index_begin();
        bool debug_indexing = false;
        forever {
                AVPacket packet;

                if (av_read_frame(pFormatCtx, &packet) < 0) {
                        if (debug_indexing) qDebug() << this << "index: av_read_frame(pFormatCtx, &packet) < 0; break";
                        break;
                }
                if (debug_indexing) qDebug() << this << "index: read video frame" << video_frames_decoded;

                if (videoStream == packet.stream_index) {
                        if (debug_indexing) qDebug() << this << "frame is video frame";
                        if (packet.flags & AV_PKT_FLAG_KEY) {
                                ++keyframes_read;
                                last_last_keyframe = last_keyframe;
                                last_keyframe = frames_read;
                                keyframe_dts.insert(last_keyframe, packet.dts);
                                keyframe_pos.insert(last_keyframe, packet.pos);
                        }
                        frame_keyframe << last_last_keyframe; //we always record frames belonging to the previous previous keyframe rather than the previous keyframe because of problems seeking to the previous keyframe in vobs (20130420)
                        ++frames_read;
                        if (frames_read % 100 == 0) {
                                if (estimated_total_frames) {
                                        emit index_progress(frames_read, estimated_total_frames);
                                } else {
                                        emit index_progress(pFormatCtx->pb->pos, file_length);
                                }
                        }
                        if (video_frames_decoded <= 100) {
                                int frameFinished;
                                int ret = avcodec_decode_video2(vCodecCtx, pFrame, &frameFinished, &packet);
                                if (ret > 0 && frameFinished) {
                                        if (debug_indexing) qDebug() << this << "video frameFinished";
                                        ++video_frames_decoded;
                                        if (video_frames_decoded >= 10) { //often the first few frames are at random times (20130915)
                                                qint64 timestamp = av_frame_get_best_effort_timestamp(pFrame);
                                                qint64 timestamp_in_converted_timebase = rescale_ts(timestamp,
                                                                                                    pFormatCtx->streams[videoStream]->time_base.num,pFormatCtx->streams[videoStream]->time_base.den,
                                                                                                    video_info.timebase_num,video_info.timebase_den);
                                                if (0) qDebug() << this << "determine monotonic_pts_increase: rescaled" << timestamp << "->" << timestamp_in_converted_timebase << "from timebase" <<
                                                            pFormatCtx->streams[videoStream]->time_base.num << pFormatCtx->streams[videoStream]->time_base.den <<
                                                            "to" << video_info.timebase_num << video_info.timebase_den;
                                                video_frame_timestamp << timestamp_in_converted_timebase;
                                        }

                                        if (!determined_interlacing) {
                                                video_info.interlaced = pFrame->interlaced_frame;
                                                if (cli_force_interlaced) video_info.interlaced = true;
                                                if (cli_force_progressive) video_info.interlaced = false;
                                                video_info.width = vCodecCtx->width;
                                                video_info.height = vCodecCtx->height;
                                                video_info.colorspace = src_pix_fmt;
                                                determined_interlacing = true;
                                                if (video_info.interlaced) {
                                                        video_info.tff = pFrame->top_field_first;
                                                }
                                                force_interlaced(video_info.interlaced);
                                                qDebug() << this << "aspect ratio is" << pFrame->sample_aspect_ratio.num << pFrame->sample_aspect_ratio.den;
                                        }
                                } //else qDebug() << "indexing: !frameFinished: ret" << ret;
                        } else if (seeking_method == seek_by_frame) {
                                av_free_packet(&packet);
                                break;
                        }

                } else if (audioStream == packet.stream_index) {
                        if (debug_indexing) qDebug() << this << "frame is audio frame";
                        int frameFinished;
                        int ret = avcodec_decode_audio4(aCodecCtx, pFrame, &frameFinished, &packet);
                        if (ret > 0 && frameFinished) {
                                if (debug_indexing) qDebug() << this << "audio frameFinished";
                                qint64 pts = av_frame_get_best_effort_timestamp(pFrame);
                                audio_frame_timestamp << pts;

                                if (determining_audio_normalization_factor_now) {
                                        determine_audio_normalization_process_avframe(pFrame);
                                }
                        }
                }

                av_free_packet(&packet);
        }
        emit index_finish();

        if (video_frame_timestamp.size() > 1) {
                //qDebug() << this << "video timestamps:" << video_frame_timestamp;
                qint64 inc_sum = 0;
                qint64 divisor = video_frame_timestamp.size()-1;
                for (int i = 1; i < video_frame_timestamp.size(); ++i) {
                        qint64 difference = video_frame_timestamp[i] - video_frame_timestamp[i-1];
                        if (0) qDebug() << this << "determine monotonic_pts_increase: difference" << difference << "(" << video_frame_timestamp[i] << "-" << video_frame_timestamp[i-1] << ")";
                        if (difference <= 0) { //don't count retrograde intervals (20130428)
                                qDebug() << this << "video: skipping nonmonotonic interval" << difference << "at frame" << i;
                                --divisor;
                                continue;
                        }
                        inc_sum += difference;
                }
                video_info.monotonic_pts_increase = video_monotonic_pts_increase = qRound((double)inc_sum / divisor);// * 2; //*2 because we are pretending the timebase denominator is double (20130327)
                qDebug() << "determined video_monotonic_pts_increase (in progressive mode) as" << video_info.monotonic_pts_increase;
                if (video_info.interlaced) {
                        video_info.monotonic_pts_increase /= 2;
                }
        }

        if (has_audio) {
                if (audio_frame_timestamp.size() > 1) {
                        //qDebug() << this << "audio timestamps:" << audio_frame_timestamp;
                        qint64 inc_sum = 0;
                        qint64 divisor = audio_frame_timestamp.size()-1;
                        for (int i = 1; i < audio_frame_timestamp.size(); ++i) {
                                qint64 difference = audio_frame_timestamp[i] - audio_frame_timestamp[i-1];
                                if (difference <= 0) { //don't count retrograde intervals (20130428)
                                        qDebug() << this << "audio: skipping nonmonotonic interval" << difference << "at frame" << i;
                                        --divisor;
                                        continue;
                                }
                                inc_sum += difference;
                                //qDebug() << audio_frame_timestamp[i] - audio_frame_timestamp[i-1] << "between" << audio_frame_timestamp[i-1] << "and" << audio_frame_timestamp[i];
                        }
                        audio_info.monotonic_pts_increase = qRound((double)inc_sum / divisor);
                        qDebug() << "determined audio_monotonic_pts_increase as" << audio_info.monotonic_pts_increase;
                }

                if (determining_audio_normalization_factor_now) {
                        determine_audio_normalization_factor_finish();
                }

                audio_resampler_initialize();
        }

        if (seeking_method != seek_by_frame) {
                video_info.number_of_frames = frames_read;
        }

        if (1) qDebug()
                        << "\nwidth" << vCodecCtx->width << "height" << vCodecCtx->height << " in video_info:" << video_info.width << video_info.height
                        << "\nnumber_of_frames" << video_info.number_of_frames
                        << "\nestimated_total_frames" << estimated_total_frames
                        << "\noff by" << (double)estimated_total_frames / (double)video_info.number_of_frames
                        << "\nduration" << pFormatCtx->streams[videoStream]->duration
                        << "\nkey frames" << keyframes_read
                        << "\navg_frame_rate" << av_q2d(pFormatCtx->streams[videoStream]->avg_frame_rate)
                        << "\nr_frame_rate" << av_q2d(pFormatCtx->streams[videoStream]->r_frame_rate)
                        << "\nnb_frames" << pFormatCtx->streams[videoStream]->nb_frames
                        << "\nticks_per_frame" << pFormatCtx->streams[videoStream]->codec->ticks_per_frame
                        << "\npCodecCtx->time_base" << vCodecCtx->time_base.num << vCodecCtx->time_base.den << "=" << av_q2d(vCodecCtx->time_base)
                        << "\npFormatCtx->time_base" << pFormatCtx->streams[videoStream]->codec->time_base.num << pFormatCtx->streams[videoStream]->codec->time_base.den << "=" << av_q2d(pFormatCtx->streams[videoStream]->codec->time_base)
                        << "\nframerate" << video_info.framerate
                        << "\n";

        int standard_field_width = -14; //negative means align left (20130417)
        int video_bitrate_bits_per_second = vCodecCtx->bit_rate;
        if (!video_bitrate_bits_per_second) {
                video_bitrate_bits_per_second = (file_length*8) / (video_info.number_of_frames / video_info.framerate);
                if (has_audio) {
                        video_bitrate_bits_per_second -= aCodecCtx->bit_rate;
                }
        }
        QString pretty_video_info = QString("Video: %1 %2 %3 %4 %5 %6")
                        .arg(QString("%1x%2").arg(vCodecCtx->width).arg(vCodecCtx->height), standard_field_width)
                        .arg(QString("%1 fps").arg(video_info.framerate), standard_field_width)
                        .arg(av_get_pix_fmt_name(src_pix_fmt), standard_field_width)
                        .arg(av_get_colorspace_name(vCodecCtx->colorspace), standard_field_width)
                        .arg(avcodec_get_name(vCodecCtx->codec_id), standard_field_width)
                        .arg(QString("%1 Kbit/s").arg(video_bitrate_bits_per_second/1000.0), standard_field_width)
                        ;
        QString pretty_audio_info = "";
        if (has_audio) {
                pretty_audio_info = QString("Audio: %1 %2 %3 %4 %5 %6")
                                .arg(QString("%1ch").arg(aCodecCtx->channels), standard_field_width)
                                .arg(QString("%1 Hz").arg(aCodecCtx->sample_rate), standard_field_width)
                                .arg(av_get_sample_fmt_name(aCodecCtx->sample_fmt), standard_field_width)
                                .arg("", standard_field_width) //just for spacing - to match the video line (20140706)
                                .arg(avcodec_get_name(aCodecCtx->codec_id), standard_field_width)
                                .arg(QString("%1 Kbit/s").arg(aCodecCtx->bit_rate/1000.0), standard_field_width)
                                ;
        }
        emit pretty_info(pretty_video_info, pretty_audio_info);

        currently_open_filename = filename;
        emit successfully_opened(filename);
}

void Nasty_Decoder::decode() {
        if (at_end) {
                if (currently_encoding) {
                        emit_decoding_finished();
                }
                return;
        }

        int frameFinished;
        AVPacket packet;

        forever {
                if (currently_encoding) {
                        if (video_frames_decoded >= video_frames_to_decode || abort) { //end of the trim section (20130403)
                                emit_decoding_finished();
                                break;
                        } else if (decode_to == 0) { //we will decode more later when requested by the encoder (20130403)
                                break;
                        }
                }

                if (av_read_frame(pFormatCtx, &packet) < 0) { //no more frames to decode
                        emit_decoding_finished();
                        at_end = true;
                        break;
                }

                if (videoStream == packet.stream_index
                                && avcodec_decode_video2(vCodecCtx, pFrame, &frameFinished, &packet) > 0 && frameFinished) {
                        qint64 pts = av_frame_get_best_effort_timestamp(pFrame);
                        //qDebug() << this << "decode(): this frame timestamp is" << pts;

                        ++video_frames_decoded;
                        --decode_to;

                        if (video_info.interlaced && src_pix_fmt != PIX_FMT_RGB24) { //deinterlacing rgb24 is broken for some reason - is it because the data is really 24-bit aligned? it shifts one of the fields by 1/3 the width of the image (20130326)
                                //libswscale actually accesses all of these array items to check them for 128-bit alignment - therefore we must provide them or we risk a segfault (20130123)
                                int in_stride[4] = {0,0,0,0};
                                int out_stride[4] = {0,0,0,0};
                                uint8_t *in[4] = {0,0,0,0};
                                uint8_t *out[4] = {0,0,0,0};
                                int in_linesize[4] = {0,0,0,0};

                                uint8_t *pDst = 0;
                                if (interlaced_yv12_conversion_needed) {
                                        pDst = (uint8_t *)av_malloc(vCodecCtx->width * vCodecCtx->height * 2);
                                        in_linesize[0] = vCodecCtx->width * 2;
                                        yv12toyuy2_field_1(pDst, in_linesize[0], //i believe this is the same type of interlaced yv12 that avisynth assumes (20130124)
                                                        (const uint8_t *)pFrame->data[0], pFrame->linesize[0],
                                                        (const uint8_t *)pFrame->data[1], pFrame->linesize[1],
                                                        (const uint8_t *)pFrame->data[2], pFrame->linesize[2],
                                                        vCodecCtx->width, vCodecCtx->height);
                                        in_stride[0] = in_linesize[0] * 2;
                                        in[0] = pDst;
                                } else {
                                        for (int i = 0; i < 4; ++i) {
                                                in_stride[i] = pFrame->linesize[i] * 2;
                                                in[i] = pFrame->data[i];
                                                in_linesize[i] = pFrame->linesize[i];
                                        }
                                }

                                out_stride[0] = pFrameRGB->linesize[0] * 2;
                                for (int field = 0; field < 2; ++field) {
                                        for (int i = 0; i < 4; ++i) {
                                                in[i] = in[i] + field*in_linesize[i]; //skip one line if this is the second field (20130126)
                                        }
                                        out[0] = pFrameRGB->data[0] + field*pFrameRGB->linesize[0];
                                        sws_scale(interlaced_convert_ctx, in, in_stride, 0, vCodecCtx->height/2, out, out_stride);
                                }

                                if (pDst) av_free(pDst);
                        } else {
                                sws_scale(encode_img_convert_ctx, pFrame->data, pFrame->linesize, 0, vCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
                        }

                        Frame frame(pFrameRGB->data[0], vCodecCtx->width, vCodecCtx->height, pFrameRGB->linesize[0], QImage::Format_ARGB32); //it makes a copy of the data for itself (20130302)

                        frame.pts = frame.second_field_pts = AV_NOPTS_VALUE;
                        if (pts != AV_NOPTS_VALUE) {
                                frame.pts = rescale_ts(pts, pFormatCtx->streams[videoStream]->time_base.num,pFormatCtx->streams[videoStream]->time_base.den, video_info.timebase_num,video_info.timebase_den);
                                frame.second_field_pts = frame.pts + video_info.monotonic_pts_increase; //this will never be used if yua is in progressive mode (20130609)
                        }
                        frame.timebase_num = video_info.timebase_num;
                        frame.timebase_den = video_info.timebase_den;
                        emit image_ready(frame);

                        if (!currently_encoding) { //only decode one video frame at a time when we aren't encoding (20130403)
                                av_free_packet(&packet);
                                break;
                        }

                } else if (audioStream == packet.stream_index
                           && currently_encoding
                           && avcodec_decode_audio4(aCodecCtx, pFrame, &frameFinished, &packet) > 0 && frameFinished) {
                        qint64 input_pts = av_frame_get_best_effort_timestamp(pFrame);

                        int number_of_samples = pFrame->nb_samples;
                        if (!audio_is_planar) number_of_samples *= pFrame->channels;
                        int data_length_for_one_channel = number_of_samples * audio_bytes_per_sample;

                        if (audio_sample_format_is_floating_point) {
                                if (audio_sample_format_is_float) {
                                        for (int plane_number = 0; plane_number < number_of_audio_planes; ++plane_number) {
                                                float *sample = (float *)pFrame->data[plane_number];
                                                for (int i = 0; i < number_of_samples; ++i) sample[i] *= audio_normalization_factor;
                                        }
                                } else if (audio_sample_format_is_double) {
                                        for (int plane_number = 0; plane_number < number_of_audio_planes; ++plane_number) {
                                                double *sample = (double *)pFrame->data[plane_number];
                                                for (int i = 0; i < number_of_samples; ++i) sample[i] *= audio_normalization_factor;
                                        }
                                }
                        } else if (audio_sample_format_is_integer) {
                                if (audio_sample_format_is_uint8) {
                                        for (int plane_number = 0; plane_number < number_of_audio_planes; ++plane_number) {
                                                quint8 *sample = (quint8 *)pFrame->data[plane_number];
                                                for (int i = 0; i < number_of_samples; ++i) sample[i] *= audio_normalization_factor;
                                        }
                                } else if (audio_sample_format_is_int16) {
                                        for (int plane_number = 0; plane_number < number_of_audio_planes; ++plane_number) {
                                                qint16 *sample = (qint16 *)pFrame->data[plane_number];
                                                for (int i = 0; i < number_of_samples; ++i) sample[i] *= audio_normalization_factor;
                                        }
                                } else if (audio_sample_format_is_int32) {
                                        for (int plane_number = 0; plane_number < number_of_audio_planes; ++plane_number) {
                                                qint32 *sample = (qint32 *)pFrame->data[plane_number];
                                                for (int i = 0; i < number_of_samples; ++i) sample[i] *= audio_normalization_factor;
                                        }
                                }
                        }

                        QList<QByteArray> list;
                        for (int plane_number = 0; plane_number < number_of_audio_planes; ++plane_number) {
                                QByteArray channel_data;
                                channel_data.resize(data_length_for_one_channel);
                                memcpy(channel_data.data(), pFrame->data[plane_number], data_length_for_one_channel);
                                list << channel_data;
                        }

                        uint8_t *input[list.size()];
                        if (audio_is_planar) { //change this when avresample_convert() stops flipping stereo with non-planar input (20130421)
                                for (int i = 0; i < list.size(); ++i) {
                                        input[i] = (uint8_t *)list[i].constData();
                                }
                        } else {
                                for (int i = 0, j = list.size()-1; i < list.size(); ++i, --j) {
                                        input[i] = (uint8_t *)list[j].constData();
                                }
                        }
                        int in_linesize = list[0].size();
                        if (!audio_is_planar) {
                                in_linesize /= audio_info.number_of_channels;
                        }
                        int in_number_of_samples_per_channel = in_linesize / audio_info.bytes_per_sample;// * source_info.number_of_channels;

                        uint8_t *output[target_info.number_of_channels]; //only the first "plane" (i.e., output[0]) is used currently because AV_SAMPLE_FMT_S16 is not a planar format - the aac encoder doesn't support planar input (20130224)
                        int out_linesize;
                        int out_number_of_samples_per_channel = avresample_available(avr) + av_rescale_rnd(avresample_get_delay(avr) + in_number_of_samples_per_channel,
                                                                                                           audio_info.sample_rate, 48000, AV_ROUND_UP);
                        av_samples_alloc(output, &out_linesize, target_info.number_of_channels, out_number_of_samples_per_channel, AV_SAMPLE_FMT_S16, 0);
                        out_number_of_samples_per_channel = avresample_convert(avr, output, out_linesize, out_number_of_samples_per_channel,
                                                         input, in_linesize, in_number_of_samples_per_channel);
                        int bytes_output = out_number_of_samples_per_channel*target_info.number_of_channels*2; //assumes 16-bit samples (20130224)

                        if (bytes_output) {
                                QByteArray data_to_encode;
                                data_to_encode.append((char *)output[0], bytes_output);
                                emit audio_samples_ready(data_to_encode, input_pts, audio_info.timebase_num, audio_info.timebase_den);
                        } else qDebug() << this << "!bytes_output";
                        av_freep(&output);
                } else {
                        //qDebug() << this << "decode(): !frameFinished? Stream index" << packet.stream_index << ", video is" << videoStream << "audio is" << audioStream;
                }

                av_free_packet(&packet);
        }
}

void Nasty_Decoder::seek_to(int seek_to_frame, bool skip_decoding) {
        qDebug() << this << "seek_to(" << seek_to_frame << ")" << "seeking_method" << seeking_method;
        if (seeking_method != seek_by_frame && frame_keyframe.isEmpty()) {
                qDebug() << this << "ordered to seek without an indexed file open!";
                return;
        }

        if (seeking_method != seek_by_frame && frame_keyframe.size() <= (int)seek_to_frame) { //we tried to seek off the end probably (20130603)
                qDebug() << this << "seek_to(): frame" << seek_to_frame << "off the end, changing to" << video_info.number_of_frames-1;
                seek_to_frame = video_info.number_of_frames-1;
        }
        qint64 current_frame = seek_to_frame;
        if (seeking_method != seek_by_frame) current_frame = frame_keyframe[seek_to_frame];

        at_end = false;
        if (seek_by_frame == seeking_method) {
                if (avformat_seek_file(pFormatCtx, videoStream, 0, seek_to_frame, seek_to_frame, AVSEEK_FLAG_FRAME) < 0) {
                        emit error_with_string(tr("decoder: avformat_seek_file() error"));
                        return;
                }
        } else if (seek_by_byte == seeking_method) {
                qint64 seek_to_pos = keyframe_pos[current_frame];
                if (avformat_seek_file(pFormatCtx, videoStream, 0, seek_to_pos, seek_to_pos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_BYTE) < 0) {
                        emit error_with_string(tr("decoder: avformat_seek_file() error"));
                        return;
                }
        } else if (seek_by_dts == seeking_method) {
                if (QString(pFormatCtx->iformat->name) == "avi") {
                        typedef struct AVIStream {
                                int64_t frame_offset;   /* current frame (video) or byte (audio) counter
                                             * (used to compute the pts) */
                                int remaining;
                                int packet_size;

                                uint32_t scale;
                                uint32_t rate;
                                int sample_size;        /* size of one sample (or packet)
                                             * (in the rate/scale sense) in bytes */

                                int64_t cum_len;        /* temporary storage (used during seek) */
                                int prefix;             /* normally 'd'<<8 + 'c' or 'w'<<8 + 'b' */
                                int prefix_count;
                                uint32_t pal[256];
                                int has_pal;
                                int dshow_block_align;  /* block align variable used to emulate bugs in
                                             * the MS dshow demuxer */

                                AVFormatContext *sub_ctx;
                                AVPacket sub_pkt;
                                uint8_t *sub_buffer;

                                int64_t seek_pos;
                        } AVIStream;
                        AVStream *st2 = pFormatCtx->streams[videoStream];
                        AVIStream *ast2 = (AVIStream *)st2->priv_data;
                        if ((int64_t)st2->time_base.num * ast2->rate != (int64_t)st2->time_base.den * ast2->scale) {
                                qDebug() << this << "av_seek_frame() will fail in an assert because"
                                         << "st2->time_base.num" << st2->time_base.num << "* ast2->rate" << ast2->rate << "(" << (qint64)st2->time_base.num * ast2->rate << ")"
                                         << "!= st2->time_base.den" << st2->time_base.den << "ast2->scale" << ast2->scale << "(" << (qint64)st2->time_base.den * ast2->scale << ")"
                                         << " did ffmpeg whack the timebase when it opened the file? e.g. \"st:0 has too large timebase, reducing\" in the console? if so as of 20130914 you're screwed, sorry - make sure your avisynth script ends with changefps(last.framerate)";
                        }
                }
                qint64 seek_to_dts = keyframe_dts[current_frame];
                if (av_seek_frame(pFormatCtx, videoStream, seek_to_dts, 0) < 0) {
                        emit error_with_string(tr("decoder: av_seek_frame() error"));
                        return;
                }
        }
        avcodec_flush_buffers(vCodecCtx);
        if (has_audio) avcodec_flush_buffers(aCodecCtx);

        while (current_frame != seek_to_frame) { //this looks wrong but it's not - here we read up to the requested frame but we do not read it - that is done in decode() (20130418)
                AVPacket packet;
                int frameFinished;
                if (av_read_frame(pFormatCtx, &packet) < 0) { //we tried to seek off the end of the video?
                        qDebug() << this << "we tried to seek off the end of the video? at_end = true";
                        at_end = true;
                        break;
                }
                if (videoStream == packet.stream_index) {
                        if (packet.flags & AV_PKT_FLAG_KEY) {
//                                qDebug() << "current_frame:" << current_frame << ": keyframe" << packet.dts;
                        } else {
//                                qDebug() << "current_frame:" << current_frame;
                        }
                        int ret = avcodec_decode_video2(vCodecCtx, pFrame, &frameFinished, &packet);
                        if (ret <= 0 || !frameFinished) {
//                                qDebug() << this << "seek(): !frameFinished: ret" << ret;
                        }
                        if (frameFinished) ++current_frame; //it's not clear to me that we shouldn't increment current_frame if avcodec_decode_video2() fails to set frameFinished - in the super dodge ball vob (a retrograde timestamps vob), current_frame is incorrect (as evidenced by the AV_PKT_FLAG_KEY flag on the decoded packet) if we do not increment it when frameFinished is not set - however not incrementing it is apparently correct for the anri test vobs as well as vobs made using ffmpeg ... (20130420)
                } else if (audioStream == packet.stream_index) {
                        avcodec_decode_audio4(aCodecCtx, pFrame, &frameFinished, &packet);
                }
                av_free_packet(&packet);
        }

        if (!skip_decoding) return decode();
}

void Nasty_Decoder::set_decoder_abort() {
        if (!currently_encoding) return;
        abort = true;
}
