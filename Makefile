include confflags

APP    =  yua

CP     =  cp -rfv
RM     =  rm -rf
MV     =  mv -v
MKDIR  =  mkdir -p
RMDIR  =  rmdir --ignore-fail-on-non-empty

QMAKE  =  qmake-qt4
MAKE   =  make -j$(shell nproc)
STRIP  =  strip

GZIP   =  gzip -f9
GIT    =  git
WGET   =  wget
TAR    =  tar

PREFIX       :=  /usr/local
INSTALL_DIR   =  install -m755 -d
INSTALL_PROG  =  install -m755 -D

SIZES = 16 24 32 48 64 96 128 256 512


all: $(APP)

$(APP):
	cd src && $(QMAKE) $(APP).pro
	cd src && $(MAKE)
	$(CP) src/$(APP) $(APP)
	$(STRIP) $(APP)

static: static-deps
	cd src && ./make_qrc.sh
	cat src/$(APP)_static.pro.in src/helpers/qrc_list > src/$(APP)_static.pro && \
		echo icon.qrc >> src/$(APP)_static.pro
	cd src && $(QMAKE) $(APP)_static.pro
	cd src && $(MAKE)
	$(CP) src/$(APP) $(APP)
	$(STRIP) $(APP)

static-deps: fdk-aac x264 mp4box ffmpeg
	$(MKDIR) src/helpers
	$(CP) gpac/bin/gcc/MP4Box src/helpers/mp4box
	$(CP) ffmpeg/ffmpeg src/helpers
	$(STRIP) src/helpers/mp4box

fdk-aac: download
	cd fdk-aac && ./autogen.sh
	cd fdk-aac && ./configure $(FDK_CONFFLAGS)
	cd fdk-aac && $(MAKE)

x264: download
	cd x264 && ./configure $(X264_CONFFLAGS)
	cd x264 && $(MAKE)

mp4box: download
	cd gpac && ./configure $(MP4BOX_CONFFLAGS)
	cd gpac && $(MAKE) lib
	cd gpac && $(MAKE) apps

ffmpeg: download
	cd ffmpeg && ./configure $(FFMPEG_CONFFLAGS)
	cd ffmpeg && $(MAKE)

download: clean-download
	$(GIT) clone git://git.videolan.org/x264.git

	$(GIT) clone git://git.code.sf.net/p/opencore-amr/fdk-aac opencore-amr-fdk-aac
	$(MV) opencore-amr-fdk-aac fdk-aac
	cd fdk-aac && git checkout v$(FDKVERSION)

	$(WGET) http://archive.ubuntu.com/ubuntu/pool/universe/g/gpac/gpac_$(GPACVERSION).orig.tar.bz2
	$(TAR) xvjf gpac_$(GPACVERSION).orig.tar.bz2
	$(MV) gpac-$(GPACVERSION).orig gpac

	$(WGET) http://ffmpeg.org/releases/ffmpeg-$(FFVERSION).tar.bz2
	$(TAR) xvjf ffmpeg-$(FFVERSION).tar.bz2
	$(RM) ffmpeg-$(FFVERSION).tar.bz2
	$(MV) ffmpeg-$(FFVERSION) ffmpeg

clean:
	cd src && $(RM) $(APP) ../$(APP) *.o moc_*.cpp qrc_*.cpp

distclean: clean-download clean
	$(RM) src/helpers src/$(APP)_static.pro src/Makefile

clean-download:
	$(RM) fdk-aac opencore-amr-fdk-aac x264 \
		gpac gpac-$(GPACVERSION).orig gpac_$(GPACVERSION).orig.tar.bz2 \
		ffmpeg ffmpeg-$(FFVERSION) ffmpeg-$(FFVERSION).tar.bz2

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

	# remove directories if they're empty
	$(RMDIR) $(PREFIX)/bin
	$(RMDIR) $(PREFIX)/share/applications/apps
	$(RMDIR) $(PREFIX)/share/applications
	$(RMDIR) $(PREFIX)/share/doc
	$(RMDIR) $(PREFIX)/share/man/man1
	$(RMDIR) $(PREFIX)/share/man
	$(RMDIR) $(PREFIX)/share/pixmaps
	$(foreach SIZE,$(SIZES),
		$(RMDIR) $(PREFIX)/share/icons/hicolor/$(SIZE)x$(SIZE)/apps ;
		$(RMDIR) $(PREFIX)/share/icons/hicolor/$(SIZE)x$(SIZE) ;
	)
	$(RMDIR) $(PREFIX)/share/icons/hicolor
	$(RMDIR) $(PREFIX)/share/icons
	$(RMDIR) $(PREFIX)/share

