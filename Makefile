APP    =  yua

CP     =  cp -rfv
RM     =  rm -rf
QMAKE  =  qmake-qt4
MAKE   =  make
STRIP  =  strip
GZIP   =  gzip -f9

PREFIX       :=  /usr/local
INSTALL_DIR   =  install -m755 -d
INSTALL_PROG  =  install -m755 -D

SIZES = 16 24 32 48 64 96 128 256 512


all:
	cd src && $(QMAKE)
	cd src && $(MAKE)
	$(CP) src/$(APP) $(APP)
	$(STRIP) $(APP)

clean:
	cd src && $(MAKE) clean
	$(RM) $(APP)

distclean:
	cd src && $(MAKE) distclean
	$(RM) $(APP)

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
	$(RM) $(PREFIX)/doc/$(APP)
	$(foreach SIZE,$(SIZES),$(RM) $(PREFIX)/share/icons/hicolor/$(SIZE)x$(SIZE)/apps/$(APP).png ;)

