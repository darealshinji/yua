#ifndef RITA_H
#define RITA_H

#include <QtGui>
#include <QGLWidget>
#ifdef G_HAVE_QT_MULTIMEDIA
#include <QVideoFrame>
#endif

class Rita : public QGLWidget {
        Q_OBJECT
public:
        explicit Rita(QWidget *parent = 0);

protected:
        void initializeGL();
        void resizeGL(int w, int h);
        void paintGL();

        void mousePressEvent(QMouseEvent *);
        void mouseReleaseEvent(QMouseEvent *);
        void mouseMoveEvent(QMouseEvent *);

        QSize minimumSizeHint() const;
        QSize sizeHint() const;
        
signals:
        void clicked(QMouseEvent *);
        void mouse_moved(QMouseEvent *);
        
public slots:
#ifdef G_HAVE_QT_MULTIMEDIA
        void display_frame(QVideoFrame);
#endif
        void display_image(QImage);

private:
        QImage current_frame;
        QSize last_size;
        GLuint texture_name;
        void load_texture(uchar *, int, int, int);
        
};

#endif // RITA_H
