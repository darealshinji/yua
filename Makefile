GPACVERSION = 0.5.0+svn4288~dfsg1
ffmpeg_prefix = $(CURDIR)/ffmpeg/libs

CFLAGS   += -fstack-protector --param=ssp-buffer-size=4
CXXFLAGS += -fstack-protector --param=ssp-buffer-size=4
CPPFLAGS += -D_FORTIFY_SOURCE=2
LDFLAGS  += -Wl,-Bsymbolic-functions -Wl,-z,relro -Wl,-z,noexecstack -Wl,--as-needed

QMAKE_FLAGS := \
	QMAKE_CFLAGS='$(CFLAGS) $(CPPFLAGS)' \
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
		--extra-cflags='$(CFLAGS) $(CPPFLAGS) -I./libs/include' \
		--extra-cxxflags='$(CXXFLAGS) $(CPPFLAGS)' \
		--extra-ldflags='$(LDFLAGS)  -L./libs/lib' \
		--disable-debug \
		--disable-runtime-cpudetect \
		--disable-bzlib \
		--disable-ffplay \
		--disable-ffprobe \
		--disable-ffserver \
		--disable-doc \
		--disable-lzma \
		--enable-avresample \
		--enable-gpl \
		--enable-libx264 \
		--enable-nonfree \
		--enable-libfdk-aac

MP4BOX_CONFFLAGS = \
		--extra-cflags='-Wall -DXP_UNIX  $(CFLAGS) $(CPPFLAGS)' \
		--extra-ldflags='$(LDFLAGS)' \
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
WGET   ?=  wget
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
	./configure $(MP4BOX_CONFFLAGS) && $(MAKE) lib && $(MAKE) apps )

ffmpeg: download
	cd ffmpeg && ./configure $(FFMPEG_CONFFLAGS)
	cd ffmpeg && $(MAKE)

download:
	[ -d x264 ] || $(GIT) clone --depth 1 git://git.videolan.org/x264.git

	[ -d ffmpeg ] || $(GIT) clone --depth 1 git://source.ffmpeg.org/ffmpeg.git

	[ -d fdk-aac ] || $(GIT) clone --depth 1 git://git.code.sf.net/p/opencore-amr/fdk-aac

	[ -d gpac ] || \
	( $(WGET) http://archive.ubuntu.com/ubuntu/pool/universe/g/gpac/gpac_$(GPACVERSION).orig.tar.bz2 && \
	$(TAR) xvjf gpac_$(GPACVERSION).orig.tar.bz2 && \
	$(RM) gpac_$(GPACVERSION).orig.tar.bz2 && \
	$(MV) gpac-$(GPACVERSION).orig gpac )

clean:
	cd src && $(RM) $(APP) ../$(APP) *.o moc_*.cpp qrc_*.cpp

distclean: clean-download clean
	$(RM) src/helpers/linux src/$(APP)_static.pro src/Makefile

clean-download:
	$(RM) fdk-aac x264 gpac* ffmpeg*

install:
	$(INSTALL_DIR) $(DESTDIR)$(PREFIX)/bin
	$(INSTALL_PROG) $(APP) $(DESTDIR)$(PREFIX)/bin
	$(CP) share $(DESTDIR)$(PREFIX)
	$(GZIP) $(DESTDIR)$(PREFIX)/share/man/man1/$(APP).1

#uninstall:
#	$(RM) $(DESTDIR)$(PREFIX)/bin/$(APP)
#	$(RM) $(DESTDIR)$(PREFIX)/share/applications/apps/$(APP).desktop
#	$(RM) $(DESTDIR)$(PREFIX)/share/man/man1/$(APP).1.gz
#	$(RM) $(DESTDIR)$(PREFIX)/share/pixmaps/$(APP).xpm
#	$(RM) $(DESTDIR)$(PREFIX)/share/doc/$(APP)
#	$(foreach SIZE,$(SIZES),$(RM) $(DESTDIR)$(PREFIX)/share/icons/hicolor/$(SIZE)x$(SIZE)/apps/$(APP).png ;)

