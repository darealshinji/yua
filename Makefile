APP    =  yua

CP     =  cp -fv
RM     =  rm -f
QMAKE  =  qmake-qt4
MAKE   =  make
STRIP  =  strip

PREFIX       :=  /usr/local
INSTALL_DIR   =  install -m755 -d
INSTALL_PROG  =  install -m755 -D


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

uninstall:
	$(RM) $(PREFIX)/bin/$(APP)

