QT += core gui opengl dbus

TARGET = yua
TEMPLATE = app

DEFINES += 'YUA_VERSION=\'\"7\"\''
linux-g++-64: DEFINES += 'WITH_NNEDI3'

SOURCES += main.cpp\
    yua.cpp \
    common/webcam_display.cpp \
    common/rita.cpp \
    nasty_decoder.cpp \
    deinterlacer.cpp \
    audio_information.cpp \
    video_encoder.cpp \
    video_information.cpp \
    job.cpp \
    audio_encoder.cpp \
    yv12toyuy2.c \
    common/frame.cpp \
    scaler.cpp \
    common/throttle.cpp \
    common/gtimer.cpp \
    common/power_management.cpp \
    decoder_mothership.cpp \
    statid_generator.cpp \
    fps_conversion.cpp \
    yua_util.cpp

linux-g++-64 {
SOURCES += \
    nnedi3.cpp \
    nnedi3_worker.cpp \
    nnedi3_worker_step2.cpp
}

HEADERS  += yua.h \
    common/webcam_display.h \
    common/rita.h \
    nasty_decoder.h \
    deinterlacer.h \
    audio_information.h \
    video_encoder.h \
    progress_type.h \
    video_information.h \
    job.h \
    audio_encoder.h \
    conversion.h \
    yv12toyuy2.h \
    common/frame.h \
    scaler.h \
    common/throttle.h \
    common/gtimer.h \
    common/power_management.h \
    decoder_mothership.h \
    statid_generator.h \
    decoder.h \
    common/threadsafequeue.h \
    fps_conversion.h \
    yua_util.h

linux-g++-64 {
HEADERS += \
    nnedi3.h \
    nnedi3_worker.h \
    nnedi3/nnedi3_constants.h
}


INCLUDEPATH += common


linux-g++-64:QMAKE_CXXFLAGS_RELEASE += -fno-caller-saves #this optimization breaks nnedi3's dotProd_m32_m16_i16_SSE2() - the vals argument is 0! (20130217)

linux-g++-64 {
SOURCES += \
nnedi3/nnedi3_extract_m8_i16_SSE2_linux.s \
nnedi3/nnedi3_castScale_SSE_linux.s \
nnedi3/nnedi3_computeNetwork0new_SSE2_linux.s \
nnedi3/nnedi3_dotProd_m32_m16_i16_SSE2_linux.s \
nnedi3/nnedi3_e0_m16_SSE2_linux.s \
nnedi3/nnedi3_uc2s64_SSE2_linux.s \
nnedi3/nnedi3_weightedAvgElliottMul5_m16_SSE2_linux.s \
nnedi3/nnedi3_processLine0_SSE2_linux.s
}

LIBS += \
-lavcodec \
-lavformat \
-lavresample \
-lavutil \
-lswscale \
-lz

QMAKE_CXXFLAGS += -D__STDC_CONSTANT_MACROS #for libavcodec/avcodec.h
INCLUDEPATH += ../Yua/include/$${YUAPLATFORMNAME}

static {
    CONFIG += static
    DEFINES += STATIC
}


RESOURCES += statid.qrc yua_icon.qrc ffmpeg.qrc mp4box.qrc

linux-g++-64: {
RESOURCES += \
helpers/linux/nnedi3_weights.bin.1.qrc \
helpers/linux/nnedi3_weights.bin.2.qrc \
helpers/linux/nnedi3_weights.bin.3.qrc \
helpers/linux/nnedi3_weights.bin.4.qrc \
helpers/linux/nnedi3_weights.bin.5.qrc \
helpers/linux/nnedi3_weights.bin.6.qrc \
helpers/linux/nnedi3_weights.bin.7.qrc \
helpers/linux/nnedi3_weights.bin.8.qrc \
helpers/linux/nnedi3_weights.bin.9.qrc \
helpers/linux/nnedi3_weights.bin.10.qrc \
helpers/linux/nnedi3_weights.bin.11.qrc \
helpers/linux/nnedi3_weights.bin.12.qrc \
helpers/linux/nnedi3_weights.bin.13.qrc \
helpers/linux/nnedi3_weights.bin.14.qrc
}

