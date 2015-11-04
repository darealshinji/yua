#include "scaler.h"

Scaler::Scaler(QObject *parent) :
        QObject(parent)
{
}

Scaler::~Scaler() {
        clean_up();
}

void Scaler::clean_up() {
        QHashIterator<QString, SwsContext *> swscales_it(swscales);
        while (swscales_it.hasNext()) {
                swscales_it.next();
                sws_freeContext(swscales_it.value());
        }
        swscales.clear();

        QHashIterator<QString, AVPicture *> input_avpictures_it(input_avpictures);
        while (input_avpictures_it.hasNext()) {
                input_avpictures_it.next();
                av_freep(input_avpictures_it.value());
        }
        input_avpictures.clear();

        QHashIterator<QString, AVPicture *> output_avpictures_it(output_avpictures);
        while (output_avpictures_it.hasNext()) {
                output_avpictures_it.next();
                av_freep(output_avpictures_it.value());
        }
        output_avpictures.clear();
}

QImage Scaler::scale(QImage in, int out_width, int out_height) {
        if (in.isNull()) {
                qDebug() << this << "input image isNull()!";
                return in;
        }

        if (in.format() != QImage::Format_ARGB32) {
                qDebug() << this << "in.format() != QImage::Format_ARGB32";
                return in;
        }

        SwsContext *resize_img_ctx = NULL;
        AVPicture *input_avpicture = NULL;
        AVPicture *output_avpicture = NULL;

        QString key = QString("%1 %2 %3 %4")
                        .arg(in.width()).arg(in.height())
                        .arg(out_width).arg(out_height);

        if (swscales.contains(key)) {
                resize_img_ctx = swscales[key];
                input_avpicture = input_avpictures[key];
                output_avpicture = output_avpictures[key];
        } else {
                if (swscales.size() > 10) { //only cache up to 10 SwsContexts and input and output AVPictures (20130316)
                        qDebug() << this << "reached > 10 cached SwsContexts! cleaning up ...";
                        clean_up();
                }

                AVPixelFormat pix_fmt = AV_PIX_FMT_RGB32;

                swscales[key] = resize_img_ctx = Yua_Util::GetSwsContext(
                                        in.width(), in.height(), pix_fmt, AVCOL_SPC_UNSPECIFIED, AVCOL_RANGE_MPEG,
                                        out_width, out_height, pix_fmt, AVCOL_SPC_UNSPECIFIED, AVCOL_RANGE_MPEG,
                                        SWS_LANCZOS);
                Q_ASSERT(resize_img_ctx);

                input_avpictures[key] = input_avpicture = (AVPicture *)av_malloc(sizeof(AVPicture));
                avpicture_alloc(input_avpicture, pix_fmt, in.width(), in.height());

                output_avpictures[key] = output_avpicture = (AVPicture *)av_malloc(sizeof(AVPicture));
                avpicture_alloc(output_avpicture, pix_fmt, out_width, out_height);
        }

        memcpy(input_avpicture->data[0], in.bits(), in.byteCount());

        sws_scale(resize_img_ctx,
                  input_avpicture->data, input_avpicture->linesize,
                  0, in.height(),
                  output_avpicture->data, output_avpicture->linesize);

        QImage out = QImage(output_avpicture->data[0],
                        out_width, out_height,
                        output_avpicture->linesize[0],
                        QImage::Format_ARGB32).copy();

        emit scaled(out);
        return out;
}
