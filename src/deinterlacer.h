#ifndef DEINTERLACER_H
#define DEINTERLACER_H

#include <QtGui>

//extern "C" {
//#include "libswscale/swscale.h"
//#include "libavcodec/avcodec.h"
//}

#include "nnedi3.h"
#include "frame.h"
#include "scaler.h"

class Deinterlacer : public QObject {
        Q_OBJECT
public:
        explicit Deinterlacer(QObject *parent = 0);
        ~Deinterlacer();
        
signals:
        void preview_ready(Frame);
        void top_bottom_field_ready(Frame, Frame);
        void pushed_down(Frame, Frame, Frame, Frame);
        void retard_bobbed(Frame, Frame, bool);
        void double_height_deinterlace(Frame, bool);
        void height_doubled(Frame);
        
public slots:
        void deinterlace(Frame, bool);
        void make_preview(Frame);
        void push_down_one_line(Frame, Frame);
        void retard_bob(Frame, Frame, bool);
        void set_preview_size(int, int);
        void double_height(Frame, bool);

private slots:
        void height_doubled_image_ready(Frame);

private:
        QImage push_down_one_line(QImage);
        int preview_width, preview_height;
        int width_for_deinterlacer, height_for_deinterlacer;
        NNEDI3 *nnedi3;
        QThread nnedi3_thread;
//        SwsContext *resize_img_ctx;
//        AVPicture *input_avpicture, *output_avpicture;
//        int last_input_width, last_input_height, last_output_width, last_output_height;
//        void clean_up_ffmpeg_crap();
        Scaler scaler;

};

#endif // DEINTERLACER_H
