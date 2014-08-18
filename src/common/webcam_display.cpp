#include "webcam_display.h"

Webcam_Display::Webcam_Display(QWidget *parent) : QWidget(parent)
      , current_image_width(0)
      , current_image_height(0)
      , current_width(0)
      , current_height(0)
      , using_arbitrary_aspect_ratio(false)
      , minimum_size_hint(320,240) //for yua (20140215)
{
        QVBoxLayout *layout = new QVBoxLayout;
        layout->setContentsMargins(0,0,0,0);
        setLayout(layout);

        rita = new Rita(this);
        connect(this, SIGNAL(image_ready(QImage)), rita, SLOT(display_image(QImage)));
        connect(rita, SIGNAL(clicked(QMouseEvent *)), this, SLOT(rita_clicked(QMouseEvent *)));
        connect(rita, SIGNAL(mouse_moved(QMouseEvent *)), this, SLOT(rita_mouse_moved(QMouseEvent *)));
        layout->addWidget(rita);

        throttle.break_time_ms = 0;
}

QSize Webcam_Display::minimumSizeHint() const {
        return minimum_size_hint;
}

QSize Webcam_Display::sizeHint() const {
        //qDebug() << this << "returning size_hint" << size_hint;
        return size_hint;
}

void Webcam_Display::correct_aspect_ratio() {
        if (!current_width) {
                current_width = width();
                current_height = height();
        }

        double image_aspect_ratio = (double)current_image_width/(double)current_image_height;
        if (using_arbitrary_aspect_ratio) {
                image_aspect_ratio = aspect_ratio;
        }

        int new_height = current_width / image_aspect_ratio;
        if (new_height <= current_height) {
                size_hint = QSize(current_width, new_height);
        } else {
                int new_width = current_height * image_aspect_ratio;
                size_hint = QSize(new_width, current_height);
        }

        rita->resize(size_hint);
}

void Webcam_Display::display_image(QImage image) {
        if (image.isNull()) {
                //qDebug() << this << "null image!";
                return;
        }

        if (throttle.break_time_ms && throttle.busy()) return;

        bool need_update = (current_image_width != image.width() || current_image_height != image.height());
        current_image_width = image.width();
        current_image_height = image.height();

        emit image_ready(image);

        if (throttle.break_time_ms) throttle.done();

        if (!need_update) return;

        return correct_aspect_ratio();
}

void Webcam_Display::resizeEvent(QResizeEvent *event) {
        if (!rita) return;
        QSize new_size = event->size();
        current_width = new_size.width();
        current_height = new_size.height();
        if (!current_image_width) return;
        return correct_aspect_ratio();
}

void Webcam_Display::set_aspect_ratio(int num, int den) {
        aspect_ratio = (double)num / (double)den;
        using_arbitrary_aspect_ratio = true;
        return correct_aspect_ratio();
}

void Webcam_Display::rita_clicked(QMouseEvent *event) {
        emit clicked(event);
        if (event->type() == QEvent::MouseButtonPress) emit clicked();
}

void Webcam_Display::rita_mouse_moved(QMouseEvent *event) {
        emit mouse_moved(event);
}

void Webcam_Display::set_minimum_size_hint(int width, int height) {
        minimum_size_hint = QSize(width, height);
}
