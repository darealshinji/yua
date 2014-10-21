QT        += core gui opengl
linux: QT += dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = yua
TEMPLATE = app

DEFINES += 'YUA_VERSION=\'\"7\"\''
!linux-g++-32: DEFINES += 'WITH_NNEDI3'

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

!linux-g++-32 {
SOURCES += \
    nnedi3.cpp \
    nnedi3_worker.cpp
}

!mac {
SOURCES += nnedi3_worker_step2.cpp
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

!linux-g++-32 {
HEADERS += \
    nnedi3.h \
    nnedi3_worker.h \
    nnedi3/nnedi3_constants.h
}


INCLUDEPATH += common


#unfortunately -fasm-blocks is only supported by the apple fork of gcc which means that you need os x to "compile" the nnedi3 inline assembly function files (20130216)
#mac
#for i in extract_m8_i16_SSE2 castScale_SSE computeNetwork0new_SSE2 dotProd_m32_m16_i16_SSE2 e0_m16_SSE2 uc2s64_SSE2 weightedAvgElliottMul5_m16_SSE2 processLine0_SSE2 ; do gcc -Wall -fasm-blocks -S -c nnedi3_${i}.c ; done

#linux
#for i in extract_m8_i16_SSE2 castScale_SSE computeNetwork0new_SSE2 dotProd_m32_m16_i16_SSE2 e0_m16_SSE2 uc2s64_SSE2 weightedAvgElliottMul5_m16_SSE2 processLine0_SSE2 ; do gcc -Wall -fasm-blocks -S -c nnedi3_${i}.c ; out_filename="nnedi3_${i}_linux.s" ; mv -vi "nnedi3_${i}.s" $out_filename ; perl -pi -e 's/.subsections_via_symbols//g' $out_filename ; perl -pi -e 's/"L00000000001\$pb"://g' $out_filename ; perl -pi -e 's/\.align \d/.align 16/g' $out_filename ; perl -pi -e 's/\.const//g' $out_filename ; perl -pi -e 's/,__eh_frame,coalesced,no_toc\+strip_static_syms\+live_support//g' $out_filename ; perl -pi -e "s/_${i}/${i}/g" $out_filename ; cp $out_filename /e/linux/yua/ ; done

#windows
#for i in castScale_SSE computeNetwork0new_SSE2 dotProd_m32_m16_i16_SSE2 e0_m16_SSE2 uc2s64_SSE2 weightedAvgElliottMul5_m16_SSE2 processLine0_SSE2 ; do gcc -Wall -arch i386 -fasm-blocks -S -c nnedi3_${i}_32.c ; perl -pi -e 's/.subsections_via_symbols//g' nnedi3_${i}_32.s ; perl -pi -e 's/"L00000000001\$pb"://g' nnedi3_${i}_32.s ; perl -pi -e 's/\.align 4/.align 16/g' nnedi3_${i}_32.s ; cp nnedi3_${i}_32.s /e/windows/yua/ ; done
#for i in extract_m8_i16_SSE2 ; do gcc -Wall -arch i386 -fasm-blocks -S -c -fomit-frame-pointer nnedi3_${i}_32.c ; perl -pi -e 's/.subsections_via_symbols//g' nnedi3_${i}_32.s ; perl -pi -e 's/"L00000000001\$pb"://g' nnedi3_${i}_32.s ; perl -pi -e 's/\.align 4/.align 16/g' nnedi3_${i}_32.s ; cp nnedi3_${i}_32.s /e/windows/yua/ ; done

linux-g++-64:QMAKE_CXXFLAGS_RELEASE += -fno-caller-saves #this optimization breaks nnedi3's dotProd_m32_m16_i16_SSE2() - the vals argument is 0! (20130217)
#QMAKE_CXXFLAGS_RELEASE += --save-temps


mac {
SOURCES += \
nnedi3/nnedi3_extract_m8_i16_SSE2.s \
nnedi3/nnedi3_castScale_SSE.s \
nnedi3/nnedi3_computeNetwork0new_SSE2.s \
nnedi3/nnedi3_dotProd_m32_m16_i16_SSE2.s \
nnedi3/nnedi3_e0_m16_SSE2.s \
nnedi3/nnedi3_uc2s64_SSE2.s \
nnedi3/nnedi3_weightedAvgElliottMul5_m16_SSE2.s \
nnedi3/nnedi3_processLine0_SSE2.s

SOURCES += nnedi3_worker_step2.s #stopped being able to compile this one without yua crashing regardless of optimizations when i moved to 10.8 (20130402)
}

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

win32 {
SOURCES += \
nnedi3/nnedi3_extract_m8_i16_SSE2_32.s \
nnedi3/nnedi3_castScale_SSE_32.s \
nnedi3/nnedi3_computeNetwork0new_SSE2_32.s \
nnedi3/nnedi3_dotProd_m32_m16_i16_SSE2_32.s \
nnedi3/nnedi3_e0_m16_SSE2_32.s \
nnedi3/nnedi3_uc2s64_SSE2_32.s \
nnedi3/nnedi3_weightedAvgElliottMul5_m16_SSE2_32.s \
nnedi3/nnedi3_processLine0_SSE2_32.s
}

mac:ICON = yua.icns
win32:RC_FILE = yua.rc


mac:YUAPLATFORMNAME = mac
win32:YUAPLATFORMNAME = windows
linux-g++-32|linux-g++-64:YUAPLATFORMNAME = linux

mac|windows: {
LIBS += \
../Yua/lib/$${YUAPLATFORMNAME}/libavresample.a \
../Yua/lib/$${YUAPLATFORMNAME}/libavcodec.a \
../Yua/lib/$${YUAPLATFORMNAME}/libavdevice.a \
../Yua/lib/$${YUAPLATFORMNAME}/libavfilter.a \
../Yua/lib/$${YUAPLATFORMNAME}/libavformat.a \
../Yua/lib/$${YUAPLATFORMNAME}/libavutil.a \
../Yua/lib/$${YUAPLATFORMNAME}/libpostproc.a \
../Yua/lib/$${YUAPLATFORMNAME}/libswresample.a \
../Yua/lib/$${YUAPLATFORMNAME}/libswscale.a
LIBS += ../Yua/lib/$${YUAPLATFORMNAME}/libfdk-aac.a
LIBS += ../Yua/lib/$${YUAPLATFORMNAME}/libx264.a
}

linux: {
LIBS += \
-lavcodec \
-lavformat \
-lavresample \
-lavutil \
-lswscale
}

QMAKE_CXXFLAGS += -D__STDC_CONSTANT_MACROS #for libavcodec/avcodec.h
INCLUDEPATH += ../Yua/include/$${YUAPLATFORMNAME}


LIBS += -lz
mac:LIBS += -lbz2 -liconv
win32:LIBS += libws2_32 #required for ffmpeg's networking crap
win32:LIBS += "-Wl,--large-address-aware" #so yua can use more than 1.5 gigs of ram (20130607)

mac:LIBS += -framework CoreFoundation
mac:LIBS += -framework IOKit #for Power_Management
mac:LIBS += -framework AVFoundation -framework CoreVideo -framework CoreMedia -framework VideoDecodeAcceleration

static {
    CONFIG += static
    DEFINES += STATIC
}


RESOURCES += statid.qrc
RESOURCES += yua_icon.qrc

mac: RESOURCES += helpers/mac/helpers.qrc helpers/mac/nnedi3.qrc

linux: RESOURCES += ffmpeg.qrc mp4box.qrc

linux: {
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

win32: {
RESOURCES += \
helpers/windows/ffmpeg.1.qrc \
helpers/windows/ffmpeg.2.qrc \
helpers/windows/ffmpeg.3.qrc \
helpers/windows/ffmpeg.4.qrc \
helpers/windows/ffmpeg.5.qrc \
helpers/windows/ffmpeg.6.qrc \
helpers/windows/ffmpeg.7.qrc \
helpers/windows/ffmpeg.8.qrc \
helpers/windows/ffmpeg.9.qrc \
helpers/windows/ffmpeg.10.qrc \
helpers/windows/ffmpeg.11.qrc \
helpers/windows/ffmpeg.12.qrc \
helpers/windows/ffmpeg.13.qrc \
helpers/windows/mp4box.1.qrc \
helpers/windows/mp4box.2.qrc \
helpers/windows/mp4box.3.qrc \
helpers/windows/nnedi3_weights.bin.1.qrc \
helpers/windows/nnedi3_weights.bin.2.qrc \
helpers/windows/nnedi3_weights.bin.3.qrc \
helpers/windows/nnedi3_weights.bin.4.qrc \
helpers/windows/nnedi3_weights.bin.5.qrc \
helpers/windows/nnedi3_weights.bin.6.qrc \
helpers/windows/nnedi3_weights.bin.7.qrc \
helpers/windows/nnedi3_weights.bin.8.qrc \
helpers/windows/nnedi3_weights.bin.9.qrc \
helpers/windows/nnedi3_weights.bin.10.qrc \
helpers/windows/nnedi3_weights.bin.11.qrc \
helpers/windows/nnedi3_weights.bin.12.qrc \
helpers/windows/nnedi3_weights.bin.13.qrc \
helpers/windows/nnedi3_weights.bin.14.qrc
}

#ASM_SOURCES += \
#nnedi3_castScale_SSE.asm \
#nnedi3_computeNetwork0_SSE2.asm \
#nnedi3_computeNetwork0_i16_SSE2.asm \
#nnedi3_computeNetwork0new_SSE2.asm \
#nnedi3_dotProd_m32_m16_SSE2.asm \
#nnedi3_dotProd_m32_m16_i16_SSE2.asm \
#nnedi3_dotProd_m48_m16_SSE2.asm \
#nnedi3_dotProd_m48_m16_i16_SSE2.asm \
#nnedi3_e0_m16_SSE2.asm \
#nnedi3_e1_m16_SSE2.asm \
#nnedi3_e2_m16_SSE2.asm \
#nnedi3_extract_m8_SSE2.asm \
#nnedi3_extract_m8_i16_SSE2.asm \
#nnedi3_processLine0_SSE2_supplicant.asm \
#nnedi3_uc2f48_SSE2.asm \
#nnedi3_uc2s48_SSE2.asm \
#nnedi3_uc2s64_SSE2.asm \
#nnedi3_weightedAvgElliottMul5_m16_SSE2.asm


OTHER_FILES += $$ASM_SOURCES
YASM_INCLUDES = $$join(INCLUDEPATH," -i ","-i ")
QMAKE_EXTRA_COMPILERS += yasmasm
yasmasm.output = ${QMAKE_FILE_BASE}.o

YASM_FORMAT = macho64
win32:YASM_FORMAT = win32

YASM_MACHINE = amd64
win32:YASM_MACHINE = x86

YASM_AMD64 = 1
win32:YASM_AMD64 = 0

yasmasm.commands = yasm -Dprogram_name=yua -Xgnu -DPREFIX -DPIC -m $$YASM_MACHINE -DARCH_X86_64=$$YASM_AMD64 -f $$YASM_FORMAT $$YASM_INCLUDES -o ${QMAKE_FILE_BASE}.o ${QMAKE_FILE_NAME}
yasmasm.input = ASM_SOURCES

