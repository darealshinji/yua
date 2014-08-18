#include "deinterlacer.h"

Deinterlacer::Deinterlacer(QObject *parent) : QObject(parent)
      ,preview_width(320)
      ,preview_height(240)
      ,width_for_deinterlacer(0)
      ,height_for_deinterlacer(0)
#ifdef WITH_NNEDI3
      ,nnedi3(NULL)
#endif
{
}

Deinterlacer::~Deinterlacer() {
#ifdef WITH_NNEDI3
        if (nnedi3_thread.isRunning()) {
                nnedi3_thread.quit();
                nnedi3_thread.wait();
        }
        if (nnedi3) delete nnedi3;
#endif
}

void Deinterlacer::set_preview_size(int new_preview_width, int new_preview_height) {
        preview_width = new_preview_width;
        preview_height = new_preview_height;
        //qDebug() << "preview size set to" << preview_width << preview_height;
}

void Deinterlacer::make_preview(Frame frame) {
        if (frame.isNull()) {
                qDebug() << "Deinterlacer: make_preview(): null image!";
                return;
        }

        Frame out_frame = frame;
        if (!out_frame.statid) out_frame.image = scaler.scale(out_frame.image, preview_width, preview_height);

        emit preview_ready(out_frame);
}

void Deinterlacer::deinterlace(Frame interlaced_image, bool d4) {
        if (interlaced_image.isNull()) {
                qDebug() << "Deinterlacer: deinterlace(): null image!";
                return;
        }

        QImage top_field_image(interlaced_image.width(), interlaced_image.height()/2, interlaced_image.format());
        QImage bottom_field_image(interlaced_image.width(), interlaced_image.height()/2, interlaced_image.format());

        uchar *in = interlaced_image.bits();
        uchar *out_top = top_field_image.bits();
        uchar *out_bottom = bottom_field_image.bits();

        bool top_field = true;
        int linesize = interlaced_image.bytesPerLine();
        int lines_remaining = interlaced_image.height();

        while (lines_remaining--) {
                uchar **out = top_field ? &out_top : &out_bottom;
                memcpy(*out, in, linesize);
                *out += linesize;
                in += linesize;
                top_field = !top_field;
        }

        if (d4) {
                top_field_image = scaler.scale(top_field_image, preview_width, preview_height);
                bottom_field_image = scaler.scale(bottom_field_image, preview_width, preview_height);
        }

        Frame top_field_frame(interlaced_image);
        top_field_frame.image = top_field_image;

        Frame bottom_field_frame(interlaced_image);
        bottom_field_frame.image = bottom_field_image;
        bottom_field_frame.pts = bottom_field_frame.second_field_pts;

        emit top_bottom_field_ready(top_field_frame, bottom_field_frame);
}

void Deinterlacer::push_down_one_line(Frame top_field, Frame bottom_field) {
        Frame top_field_pushed_down = top_field;
        top_field_pushed_down.image = push_down_one_line(top_field.image);

        Frame bottom_field_pushed_down = bottom_field;
        bottom_field_pushed_down.image = push_down_one_line(bottom_field.image);

        emit pushed_down(top_field, bottom_field, top_field_pushed_down, bottom_field_pushed_down);
}

QImage Deinterlacer::push_down_one_line(QImage input_image) {
        QImage out_image = input_image.copy();
        uchar *in = input_image.bits();
        uchar *out = out_image.bits() + out_image.bytesPerLine();
        memcpy(out, in, out_image.bytesPerLine() * (out_image.height()-1));
        return out_image;
}

void Deinterlacer::retard_bob(Frame top_field_in, Frame bottom_field_in, bool alternate) {
        if (alternate) {
                Frame tmp = top_field_in;
                top_field_in = bottom_field_in;
                bottom_field_in = tmp;
        }

        QImage top_field_out_image = top_field_in.image.copy();
        QImage bottom_field_out_image = bottom_field_in.image.copy();

        uchar *in = top_field_in.bits();
        uchar *out = top_field_out_image.bits();

        int image_width = top_field_in.bytesPerLine();
        int image_height = top_field_in.height()-1;

        for (int i = 0; i < image_height; ++i) {
                for (int j = 0; j < image_width; ++j) {
                        *out = ((3 * *in)/4 + *(in+image_width)/4);
                        ++in;
                        ++out;
                }
        }

        in = bottom_field_in.bits() + image_width;
        out = bottom_field_out_image.bits() + image_width;
        for (int i = 0; i < image_height; ++i) {
                for (int j = 0; j < image_width; ++j) {
                        *out = ((3 * *in)/4 + *(in-image_width)/4);
                        ++in;
                        ++out;
                }
        }

        Frame top_field_out = top_field_in;
        top_field_out.image = top_field_out_image;
        Frame bottom_field_out = bottom_field_in;
        bottom_field_out.image = bottom_field_out_image;

        if (!alternate) {
                emit retard_bobbed(top_field_out, bottom_field_out, alternate);
        } else {
                emit retard_bobbed(bottom_field_out, top_field_out, alternate);
        }
}

void Deinterlacer::double_height(Frame one, bool top_field) {
        //one.image = one.image.scaled(640, 240);
        if (height_for_deinterlacer != one.height() || width_for_deinterlacer != one.width()) {
                width_for_deinterlacer = one.width();
                height_for_deinterlacer = one.height();
#ifdef WITH_NNEDI3
                if (nnedi3_thread.isRunning()) {
                        nnedi3_thread.quit();
                        nnedi3_thread.wait();
                }
                if (nnedi3) delete nnedi3;
                //qDebug() << this << "making new nnedi3 with params" << width_for_deinterlacer << height_for_deinterlacer;
                nnedi3 = new NNEDI3(width_for_deinterlacer, height_for_deinterlacer);
                nnedi3->moveToThread(&nnedi3_thread);
                connect(this, SIGNAL(double_height_deinterlace(Frame,bool)), nnedi3, SLOT(double_height(Frame,bool)));
                connect(nnedi3, SIGNAL(height_doubled_image(Frame)), this, SLOT(height_doubled_image_ready(Frame)));
                nnedi3_thread.start();
#endif
        }

        emit double_height_deinterlace(one, top_field);
}

void Deinterlacer::height_doubled_image_ready(Frame frame) {
        frame.image = scaler.scale(frame.image, preview_width, preview_height);
        emit height_doubled(frame);
}
