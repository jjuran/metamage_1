# Makefile for <https://github.com/jjuran/metamage_1>

REPOS =
REPOS += ams-68k-bin
REPOS += freemount
REPOS += git
REPOS += macward-compat

METAMAGE_1 = `git remote -v | grep '^origin.*\(fetch\)' | awk '{print $$2}'`
PLEASE_RUN = 'Please run `(cd .. && git clone' $(METAMAGE_1)')`.'

BUILD_FLAG = `uname -m | grep x86_64 | sed s/x86_64/-W/`

BUILD = ./build.pl -i $(BUILD_FLAG)

default:
	@echo 'For help, run `make help`.'

help:
	@echo 'To clone a documented companion repository, run `make $$repo.git`.'
	@echo 'To build a command-line tool, run `./build.pl $$toolname`.'
	@echo 'To build a Mac app, run `bin/build-app $$appname`.'
	@echo "If build.pl can't find a new config file, run \`make catalog\` and retry."

rm-catalog:
	rm -f var/cache/compile-driver/catalog

catalog: rm-catalog
	./build.pl

../%:
	@echo $(REPOS) | grep $* > /dev/null || (echo Unknown repo $*; exit 1)
	@test -d ../$*/.git || bin/clone-repo $*

var/links:
	mkdir -p var/links

var/links/%: var/links
	@echo $(REPOS) | grep $* > /dev/null || (echo Unknown repo $*; exit 1)
	@test -d ../$*/.git || (echo; echo $(PLEASE_RUN) | sed s/metamage_1/$*/; echo; exit 128)
	@test -e var/links/$* || (ln -s ../../../$* var/links/ && rm -f var/cache/compile-driver/catalog)

%.git: ../% var/links/%
	@true

app-build-tools:
	$(BUILD) minivx

AMS_REPOS := freemount.git ams-68k-bin.git
AMS_TOOLS := exhibit graft raster minivx xv68k freemountd

AMS_UTILS_ROOT := var/install/lib/metamage

INSTALL_BIN := "`readlink var/install`/bin"
INSTALLED_VX := $(INSTALL_BIN)/minivx

INSTALL_SCRIPT := INTERPRETER=$(INSTALLED_VX) scripts/install-script.pl

DEMO_SCRIPT := INTERPRETER=$(PWD)/var/out/minivx scripts/install-script.pl

var/install:
	@echo
	@echo "Please run \`./configure\`.  To set the install location, run e.g."
	@echo
	@echo "    INSTALL_PREFIX=/usr/local ./configure"
	@echo
	@echo "instead.  (The default is ~/.local)"
	@echo
	@exit 1

ams-linux-tools: $(AMS_REPOS) var/install
	./build.pl -i $(AMS_TOOLS) display-linux interact-linux kdmode reader

ams-linux-install: ams-linux-tools ams-68k-install ams-common-install
	@echo '#!/bin/sh'                 >  var/install/bin/display
	@echo 'exec display-linux "$$@"'  >> var/install/bin/display
	@echo '#!/bin/sh'                 >  var/install/bin/interact
	@echo 'exec interact-linux "$$@"' >> var/install/bin/interact
	@chmod +x var/install/bin/display var/install/bin/interact
	install var/out/display-linux   var/install/bin
	install var/out/interact-linux  var/install/bin
	install -d $(AMS_UTILS_ROOT)
	install -t $(AMS_UTILS_ROOT) var/out/kdmode var/out/reader
	$(INSTALL_SCRIPT) v/bin/spiel-mouse.vx     var/install/bin/spiel-mouse
	$(INSTALL_SCRIPT) v/bin/spiel-keyboard.vx  var/install/bin/spiel-keyboard

ams-linux: ams-linux-tools
	@mkdir -p var/demo
	@echo '#!/bin/sh'                 >  var/demo/display
	@echo 'exec display-linux "$$@"'  >> var/demo/display
	@echo '#!/bin/sh'                 >  var/demo/interact
	@echo 'exec interact-linux "$$@"' >> var/demo/interact
	@chmod +x var/demo/display var/demo/interact
	$(DEMO_SCRIPT) v/bin/spiel-mouse.vx    var/demo/spiel-mouse
	$(DEMO_SCRIPT) v/bin/spiel-keyboard.vx var/demo/spiel-keyboard
	@echo
	@echo "Build phase complete.  Run \`make ams-linux-inst\` to continue."
	@echo

ams-linux-inst: var/install
	install -d $(AMS_UTILS_ROOT)
	install -t $(AMS_UTILS_ROOT) var/out/kdmode var/out/reader
	@echo
	@echo "Utilities installed.  Run \`make ams-linux-demo\` to continue."
	@echo

ams-linux-suid:
	chown root:root $(AMS_UTILS_ROOT)/kdmode
	chmod +s $(AMS_UTILS_ROOT)/kdmode

unspeakably-root:
	@test `id -u` -ne 0 || echo
	@test `id -u` -ne 0 || echo "    Are you ABSOLUTELY SURE you want to run this as root?"
	@test `id -u` -ne 0 || echo
	@test `id -u` -ne 0 || echo "    Do you /really/ think that's a good idea?"
	@test `id -u` -ne 0 || echo
	@test `id -u` -ne 0 || echo "    (Spoiler:  It's not.)"
	@test `id -u` -ne 0 || echo
	@test `id -u` -ne 0 || exit 13

ams-linux-check: unspeakably-root
	@test -e /dev/fb0 || echo
	@test -e /dev/fb0 || echo "No /dev/fb0?  Sorry, this isn't going to work."
	@test -e /dev/fb0 || echo
	@test -e /dev/fb0 || exit 1
	@test -w /dev/fb0 || echo
	@test -w /dev/fb0 || echo "/dev/fb0 exists, but isn't writable."
	@test -w /dev/fb0 || echo "Join the 'video' group and try again."
	@test -w /dev/fb0 || echo
	@test -w /dev/fb0 || echo "(You need a new login shell for this to work -- "
	@test -w /dev/fb0 || echo 'DO NOT run AMS as root!)'
	@test -w /dev/fb0 || echo
	@test -w /dev/fb0 || exit 1

display-check:
	@test -z "$(DISPLAY)" || echo
	@test -z "$(DISPLAY)" || echo "You have the DISPLAY environment variable set,"
	@test -z "$(DISPLAY)" || echo "but framebuffer mode doesn't work in X11."
	@test -z "$(DISPLAY)" || echo
	@test -z "$(DISPLAY)" || echo "Switch to a framebuffer console and try again,"
	@test -z "$(DISPLAY)" || echo "or run \`make ams-x11\` instead."
	@test -z "$(DISPLAY)" || echo
	@test -z "$(DISPLAY)" || exit 1

NEW_PATH = PATH="$$PWD/var/demo:$$PWD/var/out:$$PATH"
AMS_ROOT = var/links/ams-68k-bin
AMS_RSRC = "$(AMS_ROOT)/mnt/AMS Resources"
AMS_VARS = AMS_BIN=$(AMS_ROOT)/bin AMS_LIB=$(AMS_ROOT)/lib AMS_MNT=$(AMS_ROOT)/mnt
RUN_AMS  = $(NEW_PATH) $(AMS_VARS) var/out/minivx -Z v/bin/ams.vx

ams-linux-demo: ams-linux-check display-check
	$(RUN_AMS)

ams-vnc-build: $(AMS_REPOS)
	$(BUILD) $(AMS_TOOLS) listen vnc-interact

ams-vnc: ams-vnc-build
	@mkdir -p var/demo
	@echo '#!/bin/sh'                                          >  var/demo/interact
	@echo 'exec var/out/minivx -Z v/bin/interact-vnc.vx "$$@"' >> var/demo/interact
	@chmod +x var/demo/interact
	$(RUN_AMS)

ams-x11-build: $(AMS_REPOS)
	./build.pl -i $(AMS_TOOLS) interact-x11

ams-x11: ams-x11-build
	EXHIBIT_INTERACT=interact-x11 $(RUN_AMS)

ams-system-rsrcs:
	$(BUILD) minivx
	var/out/minivx -Z v/bin/mkrsrc.vx -o $(AMS_RSRC)/rsrc $(AMS_RSRC)

ams-68k-install: var/install ams-system-rsrcs
	install -d var/install/share/ams/bin
	install -d var/install/share/ams/lib
	install -d var/install/share/ams/mnt
	install -m444 $(AMS_ROOT)/bin/app    var/install/share/ams/bin
	install -m444 $(AMS_ROOT)/lib/ams-*  var/install/share/ams/lib
	install -m444 $(AMS_ROOT)/lib/vdb    var/install/share/ams/lib
	install -m444 $(AMS_ROOT)/lib/umsp   var/install/share/ams/lib
	cp -R $(AMS_ROOT)/mnt/*              var/install/share/ams/mnt

ams-common-install: var/install
	install -d var/install/bin
	test \! -x var/out/sndtrack || install var/out/sndtrack var/install/bin
	install var/out/minivx      var/install/bin
	install var/out/raster      var/install/bin
	install var/out/exhibit     var/install/bin
	install var/out/graft       var/install/bin
	install var/out/freemountd  var/install/bin
	install var/out/xv68k       var/install/bin
	$(INSTALL_SCRIPT) v/bin/ams.vx var/install/bin/ams

ams-vnc-install: ams-vnc-build ams-68k-install ams-common-install
	install var/out/listen        var/install/bin
	install var/out/vnc-interact  var/install/bin
	$(INSTALL_SCRIPT) v/bin/interact-vnc.vx var/install/bin/interact

ams-x11-install: ams-x11-build ams-68k-install ams-common-install
	install var/out/interact-x11  var/install/bin
	@echo '#!/bin/sh'               >  var/install/bin/interact
	@echo 'exec interact-x11 "$$@"' >> var/install/bin/interact
	@chmod +x var/install/bin/interact

ams-quartz-build: $(AMS_REPOS)
	$(BUILD) graft xv68k minivx freemountd Amethyst

RETROMATIC := PATH="$$PWD/var/out:$$PATH" v/bin/retromatic.vx
RETRO_APPS := var/apps

ams-quartz-demo: ams-quartz-build
	@mkdir -p $(RETRO_APPS)
	$(RETROMATIC) $(RETRO_APPS) ../ams-68k-bin/mnt/IAGO
	$(RETROMATIC) $(RETRO_APPS) ../ams-68k-bin/mnt/"Nyanochrome Cat"
	$(RETROMATIC) $(RETRO_APPS) ../ams-68k-bin/mnt/tic-tac-toe.txt
	@open $(RETRO_APPS)

ams-osx-build: $(AMS_REPOS) macward-compat.git
	bin/build-app Genie
	$(BUILD) $(AMS_TOOLS) uunix interact

MACRELIX := var/build/dbg/bin/Genie/MacRelix.app
APPS := /Applications
UAPPS := ~/Applications

ams-osx: ams-osx-build
	mkdir -p ~/var/run/fs
	test -p ~/var/run/fs/gui.fifo || mkfifo ~/var/run/fs/gui.fifo
	open $(MACRELIX)
	true > ~/var/run/fs/gui.fifo
	$(RUN_AMS)

ams-osx-install: ams-osx-build ams-68k-install ams-common-install
	install var/out/interact  var/install/bin
	install var/out/uunix     var/install/bin
	if [ -w $(APPS) ]; then cp -R $(MACRELIX) $(APPS); else mkdir -p $(UAPPS) && cp -R $(MACRELIX) $(UAPPS); fi

sndtrack:
	$(BUILD) minivx
	var/out/minivx -Z v/bin/portaudio-pkg.vx make
	$(BUILD) sndtrack

freemountd-tcp: freemount.git
	$(BUILD) freemountd listen

var/freemount/hello.txt:
	mkdir -p var/freemount/
	echo Hello world > var/freemount/hello.txt

freemountd-tcp-test: freemountd-tcp var/freemount/hello.txt
	var/out/listen 127.0.0.1:4564 var/out/freemountd --root var/freemount

freemount-tcp: freemount.git
	$(BUILD) fls fcat fget utcp

fls-test: freemount-tcp
	PATH="$$PWD/var/out:$$PATH" var/out/fls mnt://127.0.0.1

fcat-test: freemount-tcp
	PATH="$$PWD/var/out:$$PATH" var/out/fcat mnt://127.0.0.1/hello.txt

.SECONDARY:

