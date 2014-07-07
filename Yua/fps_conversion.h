#ifndef FPS_CONVERSION_H
#define FPS_CONVERSION_H

#include <QtGlobal>
#include <QDebug>

class FPS_Conversion {
public:
        static quint64 my_UInt32x32To64(quint32 a, quint32 b);
        static bool float_to_frac(float input, unsigned &num, unsigned &den);
        static bool reduce_float(float value, unsigned &num, unsigned &den);
        static void reduce_frac(unsigned &num, unsigned &den, unsigned limit);
        static void float_to_fps(float n, unsigned &num, unsigned &den);
};

#endif // FPS_CONVERSION_H
