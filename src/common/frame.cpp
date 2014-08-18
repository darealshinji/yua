#include "frame.h"

Frame::Frame(uchar *data, int width, int height, int bytesPerLine, QImage::Format format) :
        statid(false)
      , job_id(0)
{
        image = QImage(data, width, height, bytesPerLine, format).copy();
}

Frame::Frame(QImage image) :
        image(image)
      , statid(false)
      , job_id(0)
{}

Frame::Frame() :
        statid(false)
      , job_id(0)
{}

bool Frame::isNull() {
        return image.isNull();
}

int Frame::width() {
        return image.width();
}
int Frame::height() {
        return image.height();
}
QImage::Format Frame::format() {
        return image.format();
}
uchar *Frame::bits() {
        return image.bits();
}
int Frame::bytesPerLine() {
        return image.bytesPerLine();
}
int Frame::byteCount() {
        return image.byteCount();
}
