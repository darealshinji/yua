#ifndef FRAME_H
#define FRAME_H

#include <QImage>

class Frame {
public:
        Frame(uchar *data, int width, int height, int bytesPerLine, QImage::Format format);
        Frame(QImage);
        Frame();
        bool isNull();
        int width();
        int height();
        QImage::Format format();
        uchar *bits();
        int bytesPerLine();
        int byteCount();

        QImage image;
        qint64 timebase_num, timebase_den;
        qint64 pts, second_field_pts;
        bool statid;
        int job_id;
        
signals:
        
public slots:

private:
        
};

#endif // FRAME_H
