#include "audio_encoder.h"

Audio_Encoder::Audio_Encoder(QObject *parent) :
        QObject(parent)
      ,mutex(NULL)
      ,currently_encoding(false)
      ,bsfc(NULL)
      ,print_timestamp_debug_info(false)
{
}

Audio_Encoder::~Audio_Encoder() {
}

void Audio_Encoder::set_source_information(Audio_Information new_info) {
        qDebug() << this << "source_info.number_of_channels set to" << source_info.number_of_channels;
        source_info = new_info;
}

void Audio_Encoder::start(Audio_Information new_target_info) {
        if (currently_encoding) return;
        currently_encoding = true;
        target_info = new_target_info;

        output_frame_pos = 0;
        last_pts = 0;
        timestamps.clear();

        if (!temp_out_file.open()) {
                emit error_with_string(tr("Audio_Encoder: failed to open temporary file"));
                return;
        }
        filename_string = temp_out_file.fileName()+"a.aac"; //using mp4 as the output format requires convert_headers() before av_interleaved_write_frame() and tends to screw up final muxing - strange mp4s are written with problems in the first few milliseconds, e.g. missing material, apparently depending on the input timebase (20130612)
        QByteArray filename_ba = filename_string.toLocal8Bit();
        char *filename_pointer = filename_ba.data();
        avformat_alloc_output_context2(&oc, NULL, NULL, filename_pointer);
        Q_ASSERT(oc);

        //keep this code in sync with the code in Nasty_Decoder::audio_resampler_initialize() (20130831)
        if (target_info.number_of_channels == 1) target_info.channel_layout = AV_CH_LAYOUT_MONO;
        else if (target_info.number_of_channels == 2) target_info.channel_layout = AV_CH_LAYOUT_STEREO;
        else { //we aren't changing them (20130223)
                target_info.number_of_channels = source_info.number_of_channels;
                target_info.channel_layout = source_info.channel_layout;
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
                qDebug() << this << "unsupported channel layout" << source_info.channel_layout << "in input; target_info.channel_layout set to" << target_info.channel_layout;
        }


        bsfc = av_bitstream_filter_init("aac_adtstoasc");
        if (!bsfc) {
                fprintf(stderr, "audio encoder: fatal: cannot open the aac_adtstoasc bitstream filter\n");
                exit(1);
        }


        int ret;

        codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
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
                fprintf(stderr, "Could not allocate audio codec context\n");
                exit(1);
        }

        /* Some formats want stream headers to be separate. */
        if (oc->oformat->flags & AVFMT_GLOBALHEADER) {
                c->flags |= CODEC_FLAG_GLOBAL_HEADER;
        }

        c->bit_rate = target_info.bitrate;
        c->sample_fmt = AV_SAMPLE_FMT_S16;
        c->sample_rate    = 48000;
        c->channel_layout = target_info.channel_layout;
        c->channels       = target_info.number_of_channels;
        c->time_base.num  = source_info.timebase_num;
        c->time_base.den  = source_info.timebase_den;

        /* open it */
        if (mutex) mutex->lock();
        ret = avcodec_open2(c, codec, NULL);
        if (mutex) mutex->unlock();
        if (ret < 0) {
                fprintf(stderr, "Could not open codec\n");
                exit(1);
        }


        /* frame containing input raw audio */
        output_frame = avcodec_alloc_frame();
        if (!output_frame) {
                fprintf(stderr, "Could not allocate audio frame\n");
                exit(1);
        }

        output_frame->nb_samples     = c->frame_size;
        output_frame->format         = c->sample_fmt;
        output_frame->channel_layout = c->channel_layout;

        /* the codec gives us the frame size, in samples,
         * we calculate the size of the samples buffer in bytes */
        output_frame_size = av_samples_get_buffer_size(NULL, c->channels, c->frame_size,
                                                 c->sample_fmt, 0);
        output_frame_data = (uchar *)av_malloc(output_frame_size);
        if (!output_frame_data) {
                fprintf(stderr, "Could not allocate %d bytes for samples buffer\n",
                        output_frame_size);
                exit(1);
        }
        /* setup the data pointers in the AVFrame */
        ret = avcodec_fill_audio_frame(output_frame, c->channels, c->sample_fmt,
                                       (const uint8_t*)output_frame_data, output_frame_size, 0);
        if (ret < 0) {
                fprintf(stderr, "Could not setup audio frame\n");
                exit(1);
        }

        output_frame->pts = 0;


        /* open the output file, if needed */
        ret = avio_open(&oc->pb, filename_pointer, AVIO_FLAG_WRITE);
        if (ret < 0) {
                fprintf(stderr, "Error occurred when opening audio output file\n");
                return;
        }

        /* Write the stream header, if any. */
        ret = avformat_write_header(oc, NULL);
        if (ret < 0) {
                fprintf(stderr, "Error occurred when writing audio header\n");
                return;
        }
}

void Audio_Encoder::finish() {
        if (!currently_encoding) return;

        encode_data();

        //now encode the last frame if we have "left over" data - this is shorter than buffer_size (the "natural audio frame length" for the encoder) and we can only do it once (20130224)
        int last_frame_size = 0;
        if (output_frame_pos) {
                last_frame_size = output_frame_size - output_frame_pos;
        }
        if (last_frame_size) {
                output_frame->nb_samples = last_frame_size/2/c->channels; //assumes 16-bit samples (20130223)
                encode_frame(output_frame);
        }

        /* get the delayed frames */
        for (int got_output = 1; got_output;) {
                got_output = encode_frame(NULL);
        }

        av_write_trailer(oc);

        av_freep(&output_frame_data);
        avcodec_free_frame(&output_frame);

        if (mutex) mutex->lock();
        avcodec_close(c);
        if (mutex) mutex->unlock();

        for (uint i = 0; i < oc->nb_streams; ++i) {
                av_freep(&oc->streams[i]->codec);
                av_freep(&oc->streams[i]);
        }

        avio_close(oc->pb);
        av_free(oc);

        av_bitstream_filter_close(bsfc);
        bsfc = NULL;

        temp_out_file.close();
        currently_encoding = false;
        emit finished(filename_string);

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

void Audio_Encoder::encode_samples(QByteArray data_to_encode, qint64 input_pts) {
        if (!currently_encoding) return;
        input_frames << data_to_encode;
        input_timestamps << input_pts;
        encode_data();
}

void Audio_Encoder::encode_data() {
        AVRational src_timebase = {source_info.timebase_num, source_info.timebase_den};
        while (input_frames.size()) {
                QByteArray input_frame = input_frames.takeFirst();
                uchar *input_frame_data = (uchar *)input_frame.data();
                int input_frame_size = input_frame.size();
                qint64 input_pts = input_timestamps.takeFirst();
                int input_frame_pos = 0;
                while (input_frame_pos < input_frame_size) {
                        if (output_frame_pos == 0) {
                                qint64 new_pts = av_rescale_q(input_pts + (double)source_info.monotonic_pts_increase * ((double)input_frame_pos / input_frame_size), src_timebase, st->time_base);
                                output_frame->pts = new_pts;
                        }

                        int bytes_to_copy = output_frame_size - output_frame_pos;
                        if (bytes_to_copy > input_frame_size - input_frame_pos) {
                                bytes_to_copy = input_frame_size - input_frame_pos;
                        }
                        memcpy(output_frame_data + output_frame_pos, input_frame_data + input_frame_pos, bytes_to_copy);
                        input_frame_pos += bytes_to_copy;
                        output_frame_pos += bytes_to_copy;

                        if (output_frame_pos == output_frame_size) {
                                output_frame->nb_samples = output_frame_size/2/target_info.number_of_channels; //assumes 16-bit samples (20130223)
                                encode_frame(output_frame);
                                output_frame_pos = 0;
                        }
                }
        }
}

void Audio_Encoder::convert_headers(AVPacket *pkt) {
        //have to use the aac_adtstoasc bitstream filter to convert adts headers for storing the output of the aac encoder in an mp4 container, which is required for mp4box to handle initial offset timestamp correctly (20130306)
        AVPacket new_pkt = *pkt;
        int a = av_bitstream_filter_filter(bsfc, c, NULL,
                                           &new_pkt.data, &new_pkt.size,
                                           pkt->data, pkt->size,
                                           pkt->flags & AV_PKT_FLAG_KEY);
        if(a == 0 && new_pkt.data != pkt->data && new_pkt.destruct) {
                uint8_t *t = (uint8_t *)av_malloc(new_pkt.size + FF_INPUT_BUFFER_PADDING_SIZE); //the new should be a subset of the old so cannot overflow
                if(t) {
                        memcpy(t, new_pkt.data, new_pkt.size);
                        memset(t + new_pkt.size, 0, FF_INPUT_BUFFER_PADDING_SIZE);
                        new_pkt.data = t;
                        a = 1;
                } else
                        a = AVERROR(ENOMEM);
        }
        if (a > 0) {
                av_free_packet(pkt);
                new_pkt.destruct = av_destruct_packet;
        } else if (a < 0) {
                av_log(NULL, AV_LOG_ERROR, "Failed to open bitstream filter %s for stream %d with codec %s",
                       bsfc->filter->name, pkt->stream_index,
                       c->codec ? c->codec->name : "copy");
                exit(1);
        }
        *pkt = new_pkt;
}

int Audio_Encoder::encode_frame(AVFrame *frame) {
        if (frame) {
                if (last_pts && frame->pts <= last_pts) {
                        qDebug() << this << "encode_frame(): frame->pts adjusted forward from" << frame->pts << "to" << last_pts+last_good_pts_interval;
                        frame->pts = last_pts+last_good_pts_interval;
                } else {
                        last_good_pts_interval = frame->pts - last_pts;
                }
                last_pts = frame->pts;
        }

        if (print_timestamp_debug_info) if (frame) timestamps << frame->pts;

        AVPacket pkt;
        av_init_packet(&pkt);
        pkt.data = NULL; // packet data will be allocated by the encoder
        pkt.size = 0;

        int got_output;
        if (avcodec_encode_audio2(c, &pkt, frame, &got_output) < 0) {
                fprintf(stderr, "Error encoding audio frame\n");
                exit(1);
        }
        if (got_output) {
                //convert_headers(&pkt); //only used for mp4 output (20130612)
                pkt.stream_index = st->index;
                if (av_interleaved_write_frame(oc, &pkt) != 0) {
                        fprintf(stderr, "Error while writing audio frame\n");
                        exit(1);
                }
        }// else qDebug() << this << "!got_output";

        return got_output;
}

void Audio_Encoder::set_target_number_of_audio_channels(int number) {
        qDebug() << this << "target_info.number_of_channels set to" << number;
        target_info.number_of_channels = number;
}
