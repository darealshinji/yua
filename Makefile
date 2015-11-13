BUILD := $(shell sh ./config.guess)
BUILD_OS := $(shell sh ./check-system.sh)

ifeq ($(filter-out win32 win64,$(BUILD_OS)),)
HOST  := $(BUILD)
endif
ifeq ($(BUILD_OS),win64)
win64 := _win64
endif

ifneq ($(HOST),)
# build on mingw32
CROSS := $(HOST)-
endif

ifneq ($(CROSS),)
ifeq ($(filter-out x86_64-%,$(CROSS)),)
win64 := _win64
endif
endif


CXXFLAGS += -Wno-unused-result
LDFLAGS  += -s -Wl,-O1
ifeq ($(BUILD_OS),linux)
ifeq ($(HOST),)
CFLAGS   += -fstack-protector
CXXFLAGS += -fstack-protector
LDFLAGS  += -Wl,-z,now -Wl,-z,relro
endif
endif

QMAKE_FLAGS := \
	QMAKE_CFLAGS='$(CFLAGS) -Wno-unused-parameter $(CPPFLAGS)' \
	QMAKE_CXXFLAGS='$(CXXFLAGS) $(CPPFLAGS)' \
	QMAKE_LFLAGS='$(LDFLAGS)'


ffmpeg_prefix = $(CURDIR)/ffmpeg/libs

FDK_CONFFLAGS = \
		--prefix=$(ffmpeg_prefix) \
		--enable-static=yes \
		--enable-shared=no

X264_CONFFLAGS = \
		--prefix=$(ffmpeg_prefix) \
		--extra-cflags='$(CFLAGS) $(CPPFLAGS)' \
		--extra-ldflags='$(LDFLAGS)' \
		--bit-depth=8 \
		--chroma-format=all \
		--disable-cli \
		--enable-static \
		--enable-strip \
		--disable-opencl \
		--disable-avs \
		--disable-swscale \
		--disable-lavf \
		--disable-ffms \
		--disable-gpac \
		--disable-lsmash

FFMPEG_CONFFLAGS = \
		--extra-cflags='$(CFLAGS) -Wno-deprecated-declarations -I"$(ffmpeg_prefix)/include" $(CPPFLAGS)' \
		--extra-cxxflags='$(CXXFLAGS) -Wno-deprecated-declarations -I"$(ffmpeg_prefix)/include" $(CPPFLAGS)' \
		--extra-ldflags='$(LDFLAGS) -L"$(ffmpeg_prefix)/lib"' \
		--disable-debug \
		--disable-runtime-cpudetect \
		--disable-bzlib \
		--disable-lzma \
		--disable-zlib \
		--disable-ffplay \
		--disable-ffprobe \
		--disable-ffserver \
		--disable-doc \
		--disable-dxva2 \
		--disable-vaapi \
		--disable-vda \
		--disable-vdpau \
		--enable-avresample \
		--enable-gpl \
		--enable-libx264 \
		--enable-nonfree \
		--enable-libfdk-aac
		--disable-encoders \
		--enable-encoder=libx264 \
		--enable-encoder=libfdk_aac

MP4BOX_CONFFLAGS = \
		--extra-cflags='-Wall -Wno-unused-but-set-variable -Wno-maybe-uninitialized $(CFLAGS) $(CPPFLAGS)' \
		--extra-ldflags='$(LDFLAGS)' \
		--static-modules \
		--static-mp4box \
		--strip \
		--disable-pic \
		--disable-ipv6 \
		--disable-wx \
		--disable-platinum \
		--disable-alsa \
		--disable-oss-audio \
		--disable-jack \
		--disable-pulseaudio \
		--disable-x11 \
		--disable-ssl \
		--use-js=no     --use-ft=no \
		--use-jpeg=no   --use-png=no \
		--use-faad=no   --use-mad=no \
		--use-xvid=no   --use-ffmpeg=no \
		--use-ogg=no    --use-vorbis=no \
		--use-theora=no --use-openjpeg=no \
		--use-a52=no


ifeq ($(CROSS),)

QMAKE_QT4 := qtchooser -run-tool=qmake -qt=4
QMAKE_QT5 := qtchooser -run-tool=qmake -qt=5

# no shared libs, so we don't need PIC
patch_mp4box_config := sed -i 's|-fPIC -DPIC||g' config.mak
MP4BOX_CONFFLAGS    += --extra-libs="-lz -lm"

else

QMAKE_QT4  := $(CROSS)qmake-qt4
QMAKE_QT5  := $(CROSS)qmake-qt5
CC         := $(CROSS)gcc
CXX        := $(CROSS)g++
LD         := $(CROSS)ld
AR         := $(CROSS)ar
PKG_CONFIG := $(CROSS)pkg-config
EXEEXT     := .exe

FDK_CONFFLAGS    += --host=$(HOST)

X264_CONFFLAGS   += --host=$(HOST) \
                    --cross-prefix=$(CROSS) \
					--enable-win32thread

FFMPEG_CONFFLAGS += --cross-prefix=$(CROSS) \
					--enable-cross-compile \
					--arch=$(firstword $(subst -, ,$(HOST))) \
					--target-os=mingw32 \
					--enable-memalign-hack \
					--disable-pthreads \
					--enable-w32threads

# no -Wl,-z on mingw32 linkers
patch_mp4box_config := sed -i 's|-Wl,-z,defs||g' config.mak
MP4BOX_CONFFLAGS    += --cross-prefix=$(CROSS) \
					   --use-zlib=internal \
					   --extra-libs="-lz -lws2_32 -lwinmm"
endif

ifeq ($(V),1)
MAKE := make -j$(shell nproc) V=1
MP4BOX_CONFFLAGS += --verbose
else
MAKE := make -j$(shell nproc)
endif

STRIP := $(CROSS)strip
UPX   := upx-ucl

PREFIX ?= /usr/local
SIZES = 16 22 24 32 48 64 96 128 256 512

define make_yua
	cd src && $(MAKE)
	if [ -f src/release/yua$(EXEEXT) ]; then \
  cp -fv src/release/yua$(EXEEXT) yua$(EXEEXT); \
else \
  cp -fv src/yua$(EXEEXT) yua$(EXEEXT); \
fi
	$(STRIP) yua$(EXEEXT)
endef



all: yua$(EXEEXT)

yua$(EXEEXT): qt5

external_weights: src/yua_external_weights.pro
	cd src && $(QMAKE_QT4) $(QMAKE_FLAGS) yua_$@.pro
	$(make_yua)

qt5: src/yua.pro
	cd src && $(QMAKE_QT5) $(QMAKE_FLAGS) yua.pro
	$(make_yua)

qt4: src/yua.pro
	cd src && $(QMAKE_QT4) $(QMAKE_FLAGS) yua.pro
	$(make_yua)

src/yua.pro: deps
	sed "s|@EXTERNAL_NNEDI3_WEIGHTS@||" src/yua$(win64).pro.in > $@
	cat src/nnedi3_weights.bin.qrc-list >> $@
	echo 'RESOURCES += \' >> $@
	cat src/helpers/qrc_list >> $@
	echo 'yua_icon.qrc' >> $@

src/yua_external_weights.pro: deps
	sed "s|@EXTERNAL_NNEDI3_WEIGHTS@|'EXTERNAL_NNEDI3_WEIGHTS'|" src/yua$(win64).pro.in > $@
	echo 'RESOURCES += \' >> $@
	cat src/helpers/qrc_list >> $@
	echo 'yua_icon.qrc' >> $@

deps: fdk-aac x264 ffmpeg mp4box
	mkdir -p src/helpers
	cp -fv gpac/bin/gcc/mp4box$(EXEEXT) src/helpers
	cp -fv ffmpeg/ffmpeg$(EXEEXT) src/helpers
	$(STRIP) src/helpers/mp4box$(EXEEXT)
ifeq ($(BUILD_OS),linux)
ifeq ($(HOST),)
	$(UPX) src/helpers/mp4box
	$(UPX) src/helpers/ffmpeg
endif
endif
	cp -fv src/nnedi3_weights.bin src/helpers
	cd src && ./make_qrc.sh

fdk-aac: download
	[ -f fdk-aac/.libs/libfdk-aac.a ] || \
		( cd fdk-aac && ./autogen.sh && \
		CXXFLAGS='-O3 $(CXXFLAGS)' CPPFLAGS='$(CPPFLAGS)' LDFLAGS='$(LDFLAGS)' \
		./configure $(FDK_CONFFLAGS) && $(MAKE) && $(MAKE) install )

x264: download
	[ -f x264/libx264.a ] || \
		( cd x264 && ./configure $(X264_CONFFLAGS) && $(MAKE) && $(MAKE) install )

mp4box: download
	[ -f gpac/bin/gcc/mp4box$(EXEEXT) ] || \
		( cd gpac && ./configure $(MP4BOX_CONFFLAGS) && \
		$(patch_mp4box_config) && \
		$(MAKE) lib && \
		$(MAKE) -C applications/mp4box MP4BOX_STATIC=yes PROG=mp4box$(EXEEXT) )

ffmpeg: x264 fdk-aac
	[ -f ffmpeg/ffmpeg$(EXEEXT) ] || \
		( cd ffmpeg && \
		./configure $(FFMPEG_CONFFLAGS) && $(MAKE) )

download:
	[ -d x264 ] || git clone --depth 1 "git://git.videolan.org/x264.git"
	[ -d ffmpeg ] || git clone -b release/2.8 --depth 1 "https://github.com/FFmpeg/FFmpeg" ffmpeg
	[ -d fdk-aac ] || git clone --depth 1 "git://git.code.sf.net/p/opencore-amr/fdk-aac"
	[ -d gpac ] || git clone --depth 1 "https://github.com/gpac/gpac"


CLEANFILES = yua *.exe src/yua src/*.exe src/*.o src/moc_*.cpp src/qrc_*.cpp \
	src/yua_plugin_import.cpp src/object_script.yua.* src/debug src/release

DISTCLEANFILES = src/helpers src/yua.pro src/yua_external_weights.pro src/Makefile* src/qrc_list

clean:
	rm -rf $(CLEANFILES)
	[ ! -f fdk-aac/Makefile ] || $(MAKE) -C fdk-aac $@
	[ ! -f ffmpeg/config.mak ] || $(MAKE) -C ffmpeg $@
	[ ! -f gpac/config.mak ] || $(MAKE) -C gpac $@
	[ ! -f x264/config.mak ] || $(MAKE) -C x264 $@

distclean:
	rm -rf $(CLEANFILES) $(DISTCLEANFILES)
	[ ! -f fdk-aac/Makefile ] || $(MAKE) -C fdk-aac $@
	[ ! -f ffmpeg/config.mak ] || $(MAKE) -C ffmpeg $@
	[ ! -f gpac/config.mak ] || $(MAKE) -C gpac $@
	[ ! -f x264/config.mak ] || $(MAKE) -C x264 $@

clean-download:
	rm -rf fdk-aac x264 gpac* ffmpeg*

install:
	install -m755 -d $(DESTDIR)$(PREFIX)/bin
	install -m755 -D yua $(DESTDIR)$(PREFIX)/bin
	cp -rfv share $(DESTDIR)$(PREFIX)
	gzip -f9 $(DESTDIR)$(PREFIX)/share/man/man1/yua.1

