#ifndef WEBCAM_DISPLAY_H
#define WEBCAM_DISPLAY_H

#include <QVBoxLayout>
#include "rita.h"

class Webcam_Display : public QWidget {
        Q_OBJECT
public:
        explicit Webcam_Display(QWidget *parent = 0);
        QSize sizeHint() const;

protected:
        void resizeEvent(QResizeEvent *);
        QSize minimumSizeHint() const;
        
signals:
        void image_ready(QImage);
        void clicked(QMouseEvent *);
        void clicked();
        void mouse_moved(QMouseEvent *);

public slots:
        void display_image(QImage);
        void set_aspect_ratio(int, int);

private slots:
        void rita_clicked(QMouseEvent *);
        void rita_mouse_moved(QMouseEvent *);

private:
        Rita *rita;
        int current_image_width, current_image_height;
        int current_width, current_height;
        QSize size_hint;
        void correct_aspect_ratio();
        double aspect_ratio;
        bool using_arbitrary_aspect_ratio;

};

#endif // WEBCAM_DISPLAY_H
