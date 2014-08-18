#include "rita.h"

Rita::Rita(QWidget *parent) : QGLWidget(parent) {
}

void Rita::initializeGL() {
        makeCurrent();
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glGenTextures(1, &texture_name);
        glBindTexture(GL_TEXTURE_2D, texture_name);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        //glTranslatef(-1, 0, 0); //flip the texture upside down
 }

void Rita::paintGL() {
        makeCurrent();

        //glClear(GL_COLOR_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, texture_name);


        glBegin (GL_QUADS);

        glTexCoord2f (0, 0);
        glVertex3f (-1, 1, 0);

        glTexCoord2f (1, 0);
        glVertex3f (1, 1, 0);

        glTexCoord2f (1, 1);
        glVertex3f (1, -1, 0);

        glTexCoord2f (0, 1);
        glVertex3f (-1, -1, 0);

        glEnd();
}

QSize Rita::minimumSizeHint() const {
        return QSize(32,32);
}

QSize Rita::sizeHint() const {
        if (last_size.isEmpty()) return minimumSizeHint();
        return last_size;
}

void Rita::resizeGL(int width, int height) {
        makeCurrent();
        glViewport(0, 0, width, height);
        //qDebug() << "Rita: glViewport:" << width << height;
}

void Rita::load_texture(uchar *buffer, int width, int height, int color_format) {
        makeCurrent();
        glBindTexture(GL_TEXTURE_2D, texture_name);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, color_format, GL_UNSIGNED_BYTE, buffer);
        last_size = QSize(width, height);
        updateGL();
}

#ifdef G_HAVE_QT_MULTIMEDIA
void Rita::display_frame(QVideoFrame new_frame) {
        int opengl_color_format = GL_RGB;
        switch (new_frame.pixelFormat()) {
        case QVideoFrame::Format_RGB24:
        case QVideoFrame::Format_BGR24: opengl_color_format = GL_RGB; break;
        case QVideoFrame::Format_BGR32:
        case QVideoFrame::Format_RGB32:
        case QVideoFrame::Format_BGRA32:
        case QVideoFrame::Format_ARGB32: opengl_color_format = GL_RGBA; break;
        default: qDebug() << "Rita: unknown color format of frame to display!";
        }

        new_frame.map(QAbstractVideoBuffer::ReadOnly);
        load_texture(new_frame.bits(), new_frame.width(), new_frame.height(), opengl_color_format);
        new_frame.unmap();
}
#endif

void Rita::display_image(QImage new_frame) {
        if (new_frame.isNull()) {
                qDebug() << "Rita: null image!";
                return;
        }

        int opengl_color_format = GL_RGB;
        switch (new_frame.format()) {
        case QImage::Format_RGB888: opengl_color_format = GL_RGB; break;
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
        case QImage::Format_RGB32: opengl_color_format = GL_RGBA; break;
        default: qDebug() << "Rita: unknown color format of image to display!";
        }

        //if (opengl_color_format != GL_RGB) { //uncommented 20130803 due to e-con's scene camera - recommented 20130816 due to e-con's scene camera? lol
                new_frame = new_frame.rgbSwapped();
        //}
        load_texture(new_frame.bits(), new_frame.width(), new_frame.height(), opengl_color_format);
}

void Rita::mousePressEvent(QMouseEvent *event) {
        emit clicked(event);
}
void Rita::mouseReleaseEvent(QMouseEvent *event) {
        emit clicked(event);
}
void Rita::mouseMoveEvent(QMouseEvent *event) {
        emit mouse_moved(event);
}
