#!/bin/sh

g++ -Wl,-Bsymbolic-functions -Wl,-z,relro -Wl,-z,noexecstack -Wl,--as-needed -Wl,-O1 -o yua \
main.o yua.o webcam_display.o rita.o nasty_decoder.o deinterlacer.o audio_information.o video_encoder.o \
video_information.o job.o audio_encoder.o yv12toyuy2.o frame.o scaler.o throttle.o gtimer.o \
power_management.o decoder_mothership.o statid_generator.o fps_conversion.o yua_util.o nnedi3.o \
nnedi3_worker.o nnedi3_worker_step2.o nnedi3_extract_m8_i16_SSE2_linux.o nnedi3_castScale_SSE_linux.o \
nnedi3_computeNetwork0new_SSE2_linux.o nnedi3_dotProd_m32_m16_i16_SSE2_linux.o nnedi3_e0_m16_SSE2_linux.o \
nnedi3_uc2s64_SSE2_linux.o nnedi3_weightedAvgElliottMul5_m16_SSE2_linux.o nnedi3_processLine0_SSE2_linux.o \
moc_yua.o moc_webcam_display.o moc_rita.o moc_nasty_decoder.o moc_deinterlacer.o moc_video_encoder.o \
moc_audio_encoder.o moc_scaler.o moc_throttle.o moc_gtimer.o moc_decoder_mothership.o moc_statid_generator.o \
moc_decoder.o moc_nnedi3.o moc_nnedi3_worker.o qrc_statid.o qrc_yua_icon.o qrc_ffmpeg.o qrc_mp4box.o  \
-L../ffmpeg -lavcodec -lx264 -lfdk-aac -lswresample -lavformat -lavresample -lavutil -lswscale -lz \
-lQtDBus -lQtOpenGL -lQtGui -lQtCore -lGL -lpthread -pthread

