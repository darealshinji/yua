ffmpeg_prefix = $(CURDIR)/ffmpeg/libs

CXXFLAGS += -Wno-unused-result
LDFLAGS  += -Wl,-O1 -Wl,-z,now -Wl,-z,relro

QMAKE_FLAGS := \
	QMAKE_CFLAGS='$(CFLAGS) -Wno-unused-parameter $(CPPFLAGS)' \
	QMAKE_CXXFLAGS='$(CXXFLAGS) $(CPPFLAGS)' \
	QMAKE_LFLAGS='$(LDFLAGS)'

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
		--extra-cflags='-Wall -Wno-unused-but-set-variable -Wno-maybe-uninitialized $(CFLAGS) -DXP_UNIX $(CPPFLAGS)' \
		--extra-ldflags='$(LDFLAGS)' \
		--extra-libs="-lz -lm" \
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
		--use-js=no --use-ft=no \
		--use-jpeg=no --use-png=no \
		--use-faad=no --use-mad=no \
		--use-xvid=no --use-ffmpeg=no \
		--use-ogg=no --use-vorbis=no \
		--use-theora=no --use-openjpeg=no \
		--use-a52=no


QMAKE_QT4 := qtchooser -run-tool=qmake -qt=4 $(QMAKE_FLAGS)
QMAKE_QT5 := qtchooser -run-tool=qmake -qt=5 $(QMAKE_FLAGS)

ifeq ($(V),1)
MAKE := make -j$(shell nproc) V=1
MP4BOX_CONFFLAGS += --verbose
else
MAKE := make -j$(shell nproc)
endif

PREFIX ?= /usr/local
SIZES = 16 22 24 32 48 64 96 128 256 512

ARCH ?= $(shell uname -p)
tmp = src/helpers/tmp

define make_yua
	cd src && $(MAKE)
	cp -fv src/yua yua
	strip yua
endef



all: yua

static: static-yua

yua: qrc
	cd src && $(QMAKE_QT5) yua.pro
	$(make_yua)

qt4_external_weights:
	cd src && $(QMAKE_QT4) yua_$@.pro
	$(make_yua)

static_qt4_external_weights: src/yua_static_qt4_external_weights.pro
	cd src && $(QMAKE_QT4) yua_$@.pro
	$(make_yua)

qt4: qrc
	cd src && $(QMAKE_QT4) yua_qt4.pro
	$(make_yua)

static-yua: src/yua_static.pro
	cd src && $(QMAKE_QT5) yua_static.pro
	$(make_yua)

static-qt4: src/yua_static.pro
	cd src && $(QMAKE_QT4) yua_static.pro
	$(make_yua)

src/yua_static.pro: copy_nnedi_weights static-deps
	cat $@.in $(tmp)/qrc_list > $@
	echo yua_icon.qrc >> $@

src/yua_static_qt4_external_weights.pro: static-deps
	cat $@.in $(tmp)/qrc_list > $@
	echo yua_icon.qrc >> $@

static-deps: fdk-aac x264 ffmpeg mp4box
	mkdir -p $(tmp)
	cp -fv gpac/bin/gcc/MP4Box $(tmp)/mp4box
	cp -fv ffmpeg/ffmpeg $(tmp)
	strip $(tmp)/mp4box
	upx-ucl $(tmp)/mp4box
	upx-ucl $(tmp)/ffmpeg
	cd src && ./make_qrc.sh

qrc: copy_nnedi_weights
	cp -fv src/ffmpeg src/mp4box $(tmp)
	cd src && ./make_qrc.sh

copy_nnedi_weights:
	mkdir -p $(tmp)
	test "$(ARCH)" != "x86_64" || cp -fv src/helpers/nnedi3_weights.bin $(tmp)

fdk-aac: download
	[ -f fdk-aac/.libs/libfdk-aac.a ] || \
		( cd fdk-aac && ./autogen.sh && \
		CXXFLAGS='-O3 $(CXXFLAGS)' CPPFLAGS='$(CPPFLAGS)' LDFLAGS='$(LDFLAGS)' \
		./configure $(FDK_CONFFLAGS) && $(MAKE) && $(MAKE) install )

x264: download
	[ -f x264/libx264.a ] || \
		( cd x264 && ./configure $(X264_CONFFLAGS) && $(MAKE) && $(MAKE) install )

mp4box: download
	[ -f gpac/bin/gcc/MP4Box ] || \
		( cd gpac && ./configure $(MP4BOX_CONFFLAGS) && \
		sed -i 's|-fPIC -DPIC||g' config.mak && \
		$(MAKE) lib && \
		$(MAKE) -C applications/mp4box MP4BOX_STATIC=yes )

ffmpeg: download x264 fdk-aac
	[ -f ffmpeg/ffmpeg ] || \
		( cd ffmpeg && ./configure $(FFMPEG_CONFFLAGS) && $(MAKE) )

download:
	[ -d x264 ] || git clone --depth 1 "git://git.videolan.org/x264.git"
	[ -d ffmpeg ] || git clone --depth 1 "git://source.ffmpeg.org/ffmpeg.git"
	[ -d fdk-aac ] || git clone --depth 1 "git://git.code.sf.net/p/opencore-amr/fdk-aac"
	[ -d gpac ] || git clone --depth 1 "https://github.com/gpac/gpac"

clean:
	cd src && rm -f yua ../yua *.o moc_*.cpp qrc_*.cpp

distclean: clean-download clean
	rm -rf $(tmp)
	rm -f src/yua_static.pro src/yua_static_qt4_external_weights.pro
	rm -f src/Makefile src/qrc_list

clean-download:
	rm -rf fdk-aac x264 gpac* ffmpeg*

install:
	install -m755 -d $(DESTDIR)$(PREFIX)/bin
	install -m755 -D yua $(DESTDIR)$(PREFIX)/bin
	cp -rfv share $(DESTDIR)$(PREFIX)
	gzip -f9 $(DESTDIR)$(PREFIX)/share/man/man1/yua.1

