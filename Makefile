FFVERSION    =  2.3.3
FDKVERSION   =  0.1.3
GPACVERSION  =  0.5.0+svn4288~dfsg1

FDK_CONFFLAGS = \
		--enable-static=yes \
		--enable-shared=no

X264_CONFFLAGS = \
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
		--extra-cflags='-I../x264 -I../fdk-aac/libAACenc/include -I../fdk-aac/libAACdec/include -I../fdk-aac/libSYS/include' \
		--extra-ldflags='-L../x264 -L../fdk-aac/.libs' \
		--disable-debug \
		--disable-runtime-cpudetect \
		--disable-bzlib \
		--disable-ffplay \
		--disable-ffprobe \
		--disable-ffserver \
		--disable-doc \
		--enable-avresample \
		--enable-gpl \
		--enable-libx264 \
		--enable-nonfree \
		--enable-libfdk-aac

MP4BOX_CONFFLAGS = \
		--extra-cflags='-Wall -fPIC -DPIC -DXP_UNIX' \
		--strip \
		--disable-ipv6 \
		--disable-wx \
		--disable-platinum \
		--disable-alsa \
		--disable-oss-audio \
		--disable-jack \
		--disable-pulseaudio \
		--disable-x11-shm \
		--disable-x11-xv \
		--disable-ssl \
		--static-mp4box



APP    ?=  yua

CP     ?=  cp -rfv
RM     :=  rm -rf
MV     ?=  mv -v
MKDIR  ?=  mkdir -p
RMDIR  ?=  rmdir

QMAKE  ?=  qmake
MAKE   ?=  make -j$(shell nproc)
STRIP  ?=  strip
UPX    ?=  upx-ucl

GZIP   ?=  gzip -f9
GIT    ?=  git
WGET   ?=  wget
TAR    ?=  tar

QMAKE_QT4  ?=  qmake-qt4

PREFIX        ?=  /usr/local
INSTALL_DIR   ?=  install -m755 -d
INSTALL_PROG  ?=  install -m755 -D

SIZES = 16 24 32 48 64 96 128 256 512


all: $(APP)

static: static-deps static-$(APP)

$(APP): nnedi3_weights_qrc
	cd src && $(QMAKE) $(APP).pro
	cd src && $(MAKE)
	$(CP) src/$(APP) $(APP)
	$(STRIP) $(APP)

qt4: nnedi3_weights_qrc
	cd src && $(QMAKE_QT4) $(APP)_qt4_linux.pro
	cd src && $(MAKE)
	$(CP) src/$(APP) $(APP)
	$(STRIP) $(APP)

static-$(APP): static-deps src/$(APP)_static.pro
	cd src && $(QMAKE) $(APP)_static.pro
	cd src && $(MAKE)
	$(CP) src/$(APP) $(APP)
	$(STRIP) $(APP)
	$(UPX) $(APP)

static-qt4: static-deps src/$(APP)_static.pro
	cd src && $(QMAKE_QT4) $(APP)_static.pro
	cd src && $(MAKE)
	$(CP) src/$(APP) $(APP)
	$(STRIP) $(APP)
	$(UPX) $(APP)

src/$(APP)_static.pro:
	cd src && ./make_qrc_linux.sh
	cat src/$(APP)_static.pro.in src/helpers/linux/qrc_list > src/$(APP)_static.pro
	echo $(APP)_icon.qrc >> src/$(APP)_static.pro

static-deps: fdk-aac x264 mp4box ffmpeg
	$(MKDIR) src/helpers/linux
	$(CP) src/helpers/nnedi3_weights.bin src/helpers/linux
	$(CP) gpac/bin/gcc/MP4Box src/helpers/linux/mp4box
	$(CP) ffmpeg/ffmpeg src/helpers/linux
	$(STRIP) src/helpers/linux/mp4box
	$(UPX) src/helpers/linux/mp4box
	$(UPX) src/helpers/linux/ffmpeg

nnedi3_weights_qrc:
	$(MKDIR) src/helpers/linux
	$(CP) src/helpers/nnedi3_weights.bin src/helpers/linux
	cd src && ./make_qrc_linux.sh

fdk-aac: download
	[ -f fdk-aac/.libs/libfdk-aac.a ] || \
	( cd fdk-aac && ./autogen.sh && ./configure $(FDK_CONFFLAGS) && $(MAKE) )
	# fix fdk-aac detection for ffmpeg
	$(MKDIR) fdk-aac/libAACenc/include/fdk-aac
	$(MKDIR) fdk-aac/libAACdec/include/fdk-aac
	cd fdk-aac/libAACenc/include && $(CP) aacenc_lib.h fdk-aac
	cd fdk-aac/libAACdec/include && $(CP) aacdecoder_lib.h fdk-aac

x264: download
	[ -f x264/libx264.a ] || ( cd x264 && ./configure $(X264_CONFFLAGS) && $(MAKE) )

mp4box: download
	[ -f gpac/bin/gcc/MP4Box ] || ( cd gpac && \
	./configure $(MP4BOX_CONFFLAGS) && $(MAKE) lib && $(MAKE) apps )

ffmpeg: download
	cd ffmpeg && ./configure $(FFMPEG_CONFFLAGS)
	cd ffmpeg && $(MAKE)

download:
	[ -d x264 ] || $(GIT) clone git://git.videolan.org/x264.git

	[ -d fdk-aac ] || \
	( $(GIT) clone git://git.code.sf.net/p/opencore-amr/fdk-aac && \
	cd fdk-aac && git checkout v$(FDKVERSION) )

	[ -d gpac ] || \
	( $(WGET) http://archive.ubuntu.com/ubuntu/pool/universe/g/gpac/gpac_$(GPACVERSION).orig.tar.bz2 && \
	$(TAR) xvjf gpac_$(GPACVERSION).orig.tar.bz2 && \
	$(MV) gpac-$(GPACVERSION).orig gpac )

	[ -d ffmpeg ] || \
	( $(WGET) http://ffmpeg.org/releases/ffmpeg-$(FFVERSION).tar.bz2 && \
	$(TAR) xvjf ffmpeg-$(FFVERSION).tar.bz2 && \
	$(MV) ffmpeg-$(FFVERSION) ffmpeg )

clean:
	cd src && $(RM) $(APP) ../$(APP) *.o moc_*.cpp qrc_*.cpp

distclean: clean-download clean
	$(RM) src/helpers/linux src/$(APP)_static.pro src/Makefile

clean-download:
	$(RM) fdk-aac x264 gpac* ffmpeg*

install:
	$(INSTALL_DIR) $(PREFIX)/bin
	$(INSTALL_PROG) $(APP) $(PREFIX)/bin
	$(CP) share $(PREFIX)
	$(GZIP) $(PREFIX)/share/man/man1/$(APP).1

uninstall:
	$(RM) $(PREFIX)/bin/$(APP)
	$(RM) $(PREFIX)/share/applications/apps/$(APP).desktop
	$(RM) $(PREFIX)/share/man/man1/$(APP).1.gz
	$(RM) $(PREFIX)/share/pixmaps/$(APP).xpm
	$(RM) $(PREFIX)/share/doc/$(APP)
	$(foreach SIZE,$(SIZES),$(RM) $(PREFIX)/share/icons/hicolor/$(SIZE)x$(SIZE)/apps/$(APP).png ;)
