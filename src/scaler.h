#ifndef SCALER_H
#define SCALER_H

#include <QtGui>

#include "yua_util.h"

class Scaler : public QObject {
        Q_OBJECT
public:
        explicit Scaler(QObject *parent = 0);
        ~Scaler();
signals:
        void scaled(QImage);
public slots:
        QImage scale(QImage, int, int);
private:
        QHash<QString, SwsContext *> swscales;
        QHash<QString, AVPicture *> input_avpictures;
        QHash<QString, AVPicture *> output_avpictures;
        void clean_up();
};

#endif // SCALER_H
