include config.mak

QMAKE_FLAGS := \
	QMAKE_CFLAGS='$(CFLAGS) $(CPPFLAGS)' \
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
		--enable-libfdk-aac \
		--disable-encoders \
		--enable-encoder=libx264 \
		--enable-encoder=libfdk_aac

MP4BOX_CONFFLAGS = \
		--extra-cflags='-Wall -Wno-unused-but-set-variable -Wno-maybe-uninitialized $(CFLAGS) $(CPPFLAGS)' \
		--extra-ldflags='$(LDFLAGS) -Wl,--allow-multiple-definition' \
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


ifeq ($(CROSS_BUILD),yes)
FDK_CONFFLAGS    += --host=$(TARGET) \
					--build=$(BUILD)
X264_CONFFLAGS   += --host=$(TARGET) \
					--cross-prefix=$(CROSS)
FFMPEG_CONFFLAGS += --cross-prefix=$(CROSS) \
					--enable-cross-compile \
					--arch=$(ARCH)
MP4BOX_CONFFLAGS += --host=$(TARGET) \
					--build=$(BUILD) \
					--cross-prefix=$(CROSS)
endif


ifeq ($(TARGET_OS),windows)
FFMPEG_CONFFLAGS += --target-os=mingw32 \
					--enable-memalign-hack \
					--disable-pthreads \
					--enable-w32threads
MP4BOX_CONFFLAGS += --use-zlib=internal \
					--extra-libs="-lz -lws2_32 -lwinmm"
else
MP4BOX_CONFFLAGS += --extra-libs="-lz -lm"
endif


ifeq ($(V),1)
MAKE := make V=1
MP4BOX_CONFFLAGS += --verbose
else
MAKE := make
endif


STRIP := $(CROSS)strip
UPX   := upx-ucl

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

yua$(EXEEXT): src/yua.pro
	cd src && $(QMAKE) $(QMAKE_FLAGS) yua.pro
	$(make_yua)

src/yua.pro: deps
	cat $@.in > $@
	echo 'RESOURCES += \' >> $@
	cat src/helpers/qrc_list >> $@


DEPS_TARGETS = \
	x264/libx264.a \
	fdk-aac/.libs/libfdk-aac.a \
	ffmpeg/ffmpeg$(EXEEXT) \
	gpac/bin/gcc/mp4box$(EXEEXT)

deps: $(DEPS_TARGETS)
	mkdir -p src/helpers
	cp -fv gpac/bin/gcc/mp4box$(EXEEXT) src/helpers/mp4box
	cp -fv ffmpeg/ffmpeg$(EXEEXT) src/helpers/ffmpeg
	$(STRIP) src/helpers/mp4box
ifeq ($(TARGET_OS),linux)
	$(UPX) src/helpers/mp4box
	$(UPX) src/helpers/ffmpeg
endif
	cp -fv src/nnedi3_weights.bin src/helpers
	cd src && ./make_qrc.sh

fdk-aac/.libs/libfdk-aac.a: download
	[ -f $@ ] || (cd fdk-aac && ./autogen.sh && \
		CXXFLAGS='-O3 $(CXXFLAGS)' CPPFLAGS='$(CPPFLAGS)' LDFLAGS='$(LDFLAGS)' \
		./configure $(FDK_CONFFLAGS) && $(MAKE) && $(MAKE) install )

x264/libx264.a: download
	[ -f $@ ] || (cd x264 && ./configure $(X264_CONFFLAGS) && $(MAKE) && $(MAKE) install)

gpac/bin/gcc/mp4box$(EXEEXT): download
	[ -f $@ ] || (cd gpac && ./configure $(MP4BOX_CONFFLAGS) && \
		sed -i 's|-fPIC -DPIC||g; s|-Wl,-z,defs||g' config.mak && \
		$(MAKE) lib && \
		$(MAKE) -C applications/mp4box MP4BOX_STATIC=yes PROG=mp4box$(EXEEXT) )

ffmpeg/ffmpeg$(EXEEXT): x264/libx264.a fdk-aac/.libs/libfdk-aac.a
	[ -f $@ ] || (cd ffmpeg && ./configure $(FFMPEG_CONFFLAGS) && $(MAKE))

download:
	[ -d x264 ] || git clone --depth 1 "git://git.videolan.org/x264.git"
	[ -d ffmpeg ] || git clone -b release/2.8 --depth 1 "https://github.com/FFmpeg/FFmpeg" ffmpeg
	[ -d fdk-aac ] || git clone --depth 1 "git://git.code.sf.net/p/opencore-amr/fdk-aac"
ifeq ($(TARGET_OS),windows)
	[ -d gpac ] || (git clone "https://github.com/gpac/gpac" && \
		cd gpac && git checkout 2fcb2101dd47321879e50e5dedb3f6078e98ed19 && \
		patch -p1 < ../gpac-mingw32.patch)
else
	[ -d gpac ] || git clone --depth 1 "https://github.com/gpac/gpac"
endif


CLEANFILES = yua *.exe src/yua src/*.exe src/*.o src/moc_*.cpp src/qrc_*.cpp \
	src/yua_plugin_import.cpp src/object_script.yua.* src/debug src/release

DISTCLEANFILES = config.mak ffmpeg/libs gpac/bin/gcc/mp4box* \
	src/helpers src/yua.pro src/yua.pro.in src/Makefile* src/qrc_list

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
	rm -rf fdk-aac x264 gpac ffmpeg

install:
	install -m755 -d $(DESTDIR)$(PREFIX)/bin
	install -m755 -D yua $(DESTDIR)$(PREFIX)/bin
	cp -rfv share $(DESTDIR)$(PREFIX)
	gzip -f9 $(DESTDIR)$(PREFIX)/share/man/man1/yua.1

config.mak:
	./configure

