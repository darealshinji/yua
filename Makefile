ffmpeg_prefix = $(CURDIR)/ffmpeg/libs

CXXFLAGS += -Wno-unused-result
LDFLAGS  += -Wl,-z,relro -Wl,-z,noexecstack -Wl,--as-needed

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
		--extra-cflags='$(CFLAGS) -Wno-deprecated-declarations $(CPPFLAGS) -I./libs/include' \
		--extra-cxxflags='$(CXXFLAGS) -Wno-deprecated-declarations $(CPPFLAGS)' \
		--extra-ldflags='$(LDFLAGS)  -L./libs/lib' \
		--disable-debug \
		--disable-runtime-cpudetect \
		--disable-bzlib \
		--disable-ffplay \
		--disable-ffprobe \
		--disable-ffserver \
		--disable-doc \
		--disable-lzma \
		--disable-dxva2 \
		--disable-vaapi \
		--disable-vda \
		--disable-vdpau \
		--enable-avresample \
		--enable-gpl \
		--enable-libx264 \
		--enable-nonfree \
		--enable-libfdk-aac

MP4BOX_CONFFLAGS = \
		--extra-cflags='-Wall -DXP_UNIX $(CFLAGS) $(CPPFLAGS)' \
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
		--use-js=no --use-ft=no --use-jpeg=no --use-png=no --use-faad=no --use-mad=no --use-xvid=no \
		--use-ffmpeg=no --use-ogg=no --use-vorbis=no --use-theora=no --use-openjpeg=no --use-a52=no



APP    ?=  yua

CP     ?=  cp -rfv
RM     :=  rm -rf
MV     ?=  mv -v
MKDIR  ?=  mkdir -p
RMDIR  ?=  rmdir

QMAKE_QT4 := qtchooser -run-tool=qmake -qt=4 $(QMAKE_FLAGS)
QMAKE_QT5 := qtchooser -run-tool=qmake -qt=5 $(QMAKE_FLAGS)

ifeq ($(V),1)
MAKE   :=  make -j$(shell nproc) V=1
MP4BOX_CONFFLAGS += --verbose
else
MAKE   :=  make -j$(shell nproc)
endif
STRIP  ?=  strip
UPX    ?=  upx-ucl

GZIP   ?=  gzip -f9
GIT    ?=  git
TAR    ?=  tar

PREFIX        ?=  /usr/local
INSTALL_DIR   ?=  install -m755 -d
INSTALL_PROG  ?=  install -m755 -D

SIZES = 16 22 24 32 48 64 96 128 256 512


all: $(APP)

static: static-deps static-$(APP)

$(APP): qrc
	cd src && $(QMAKE_QT5) $(APP).pro
	cd src && $(MAKE)
	$(CP) src/$(APP) $(APP)
	$(STRIP) $(APP)

qt4_linux_external_weights:
	cd src && $(QMAKE_QT4) $(APP)_$@.pro
	cd src && $(MAKE)
	$(CP) src/$(APP) $(APP)
	$(STRIP) $(APP)

qt4: qrc
	cd src && $(QMAKE_QT4) $(APP)_qt4_linux.pro
	cd src && $(MAKE)
	$(CP) src/$(APP) $(APP)
	$(STRIP) $(APP)

static-$(APP): static-deps src/$(APP)_static.pro
	cd src && $(QMAKE_QT5) $(APP)_static.pro
	cd src && $(MAKE)
	$(CP) src/$(APP) $(APP)
	$(STRIP) $(APP)

static-qt4: static-deps src/$(APP)_static.pro
	cd src && $(QMAKE_QT4) $(APP)_static.pro
	cd src && $(MAKE)
	$(CP) src/$(APP) $(APP)
	$(STRIP) $(APP)

src/$(APP)_static.pro:
	$(MKDIR) src/helpers/linux
ifeq ($(shell uname -p),x86_64)
	$(CP) src/helpers/nnedi3_weights.bin src/helpers/linux
endif
	cd src && ./make_qrc_linux.sh
	cat src/$(APP)_static.pro.in src/helpers/linux/qrc_list > src/$(APP)_static.pro
	echo $(APP)_icon.qrc >> src/$(APP)_static.pro

static-deps: fdk-aac x264 ffmpeg mp4box
	$(MKDIR) src/helpers/linux
	$(CP) src/helpers/nnedi3_weights.bin src/helpers/linux
	$(CP) gpac/bin/gcc/MP4Box src/helpers/linux/mp4box
	$(CP) ffmpeg/ffmpeg src/helpers/linux
	$(STRIP) src/helpers/linux/mp4box
	$(UPX) src/helpers/linux/mp4box
	$(UPX) src/helpers/linux/ffmpeg

qrc:
	$(MKDIR) src/helpers/linux
	$(CP) src/ffmpeg src/mp4box src/helpers/linux
ifeq ($(shell uname -p),x86_64)
	$(CP) src/helpers/nnedi3_weights.bin src/helpers/linux
endif
	cd src && ./make_qrc_linux.sh

fdk-aac: download
	[ -f fdk-aac/.libs/libfdk-aac.a ] || \
	( cd fdk-aac && ./autogen.sh && \
	CXXFLAGS='-O3 $(CXXFLAGS)' CPPFLAGS='$(CPPFLAGS)' LDFLAGS='$(LDFLAGS)' \
	./configure $(FDK_CONFFLAGS) && $(MAKE) && $(MAKE) install )

x264: download
	[ -f x264/libx264.a ] || \
	( cd x264 && ./configure $(X264_CONFFLAGS) && $(MAKE) && $(MAKE) install )

mp4box: download
	[ -f gpac/bin/gcc/MP4Box ] || ( cd gpac && \
	./configure $(MP4BOX_CONFFLAGS) && \
	sed -i 's|-fPIC -DPIC||g' config.mak && \
	$(MAKE) lib && $(MAKE) -C applications/mp4box MP4BOX_STATIC=yes )

ffmpeg: download x264 fdk-aac
	[ -f ffmpeg/ffmpeg_stamp ] || ( cd ffmpeg && \
	./configure $(FFMPEG_CONFFLAGS) && $(MAKE) && touch ffmpeg_stamp )

download:
	[ -d x264 ] || $(GIT) clone --depth 1 git://git.videolan.org/x264.git

	[ -d ffmpeg ] || $(GIT) clone --depth 1 git://source.ffmpeg.org/ffmpeg.git

	[ -d fdk-aac ] || $(GIT) clone --depth 1 git://git.code.sf.net/p/opencore-amr/fdk-aac

	[ -d gpac ] || $(GIT) clone --depth 1 https://github.com/gpac/gpac

clean:
	cd src && $(RM) $(APP) ../$(APP) *.o moc_*.cpp qrc_*.cpp

distclean: clean-download clean
	$(RM) src/helpers/linux src/$(APP)_static.pro src/Makefile src/qrc_list

clean-download:
	$(RM) fdk-aac x264 gpac* ffmpeg*

install:
	$(INSTALL_DIR) $(DESTDIR)$(PREFIX)/bin
	$(INSTALL_PROG) $(APP) $(DESTDIR)$(PREFIX)/bin
	$(CP) share $(DESTDIR)$(PREFIX)
	$(GZIP) $(DESTDIR)$(PREFIX)/share/man/man1/$(APP).1

