# Makefile for <https://github.com/jjuran/metamage_1>

REPOS =
REPOS += ams-68k-bin
REPOS += freemount
REPOS += git
REPOS += macward-compat
REPOS += scripts
REPOS += skif-samples

METAMAGE_1 = `git remote -v | grep '^origin.*\(fetch\)' | awk '{print $$2}'`
PLEASE_RUN = 'Please run `(cd .. && git clone' $(METAMAGE_1)')`.'

BUILD = ./build.pl -i

VAR_BIN := var/bin

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

$(VAR_BIN)/lib-static:
	$(BUILD) lib-static

build-tools: $(VAR_BIN)/lib-static

app-build-tools: build-tools
	$(BUILD) minivx

AMS_REPOS := freemount.git ams-68k-bin.git
AMS_TOOLS := exhibit graft skif minivx xv68k freemountd

AMS_UTILS_ROOT := var/install/lib/metamage

INSTALL_BIN := "`readlink var/install`/bin"
INSTALLED_VX := $(INSTALL_BIN)/minivx

INSTALL_SCRIPT := INTERPRETER=$(INSTALLED_VX) scripts/install-script.pl

DEMO_SCRIPT := INTERPRETER=$(PWD)/$(VAR_BIN)/minivx scripts/install-script.pl

var/install:
	@echo
	@echo "Please run \`./configure\`.  To set the install location, run e.g."
	@echo
	@echo "    INSTALL_PREFIX=/usr/local ./configure"
	@echo
	@echo "instead.  (The default is ~/.local)"
	@echo
	@exit 1

ams-linux-tools: $(AMS_REPOS) var/install build-tools
	./build.pl -i $(AMS_TOOLS) display-fbdev interact-fbdev kdmode reader

ams-linux-install: ams-linux-tools ams-68k-install ams-common-install
	@echo '#!/bin/sh'                 >  var/install/bin/display
	@echo 'exec display-fbdev "$$@"'  >> var/install/bin/display
	@echo '#!/bin/sh'                 >  var/install/bin/interact
	@echo 'exec interact-fbdev "$$@"' >> var/install/bin/interact
	@chmod +x var/install/bin/display var/install/bin/interact
	install $(VAR_BIN)/display-fbdev   var/install/bin
	install $(VAR_BIN)/interact-fbdev  var/install/bin
	install -d $(AMS_UTILS_ROOT)
	install -t $(AMS_UTILS_ROOT) $(VAR_BIN)/kdmode $(VAR_BIN)/reader
	$(INSTALL_SCRIPT) v/bin/spiel-mouse.vx     var/install/bin/spiel-mouse
	$(INSTALL_SCRIPT) v/bin/spiel-keyboard.vx  var/install/bin/spiel-keyboard

ams-linux: ams-linux-tools
	@mkdir -p var/demo
	@echo '#!/bin/sh'                 >  var/demo/display
	@echo 'exec display-fbdev "$$@"'  >> var/demo/display
	@echo '#!/bin/sh'                 >  var/demo/interact
	@echo 'exec interact-fbdev "$$@"' >> var/demo/interact
	@chmod +x var/demo/display var/demo/interact
	$(DEMO_SCRIPT) v/bin/spiel-mouse.vx    var/demo/spiel-mouse
	$(DEMO_SCRIPT) v/bin/spiel-keyboard.vx var/demo/spiel-keyboard
	@echo
	@echo "Build phase complete.  Run \`make ams-linux-inst\` to continue."
	@echo

ams-linux-inst: var/install
	install -d $(AMS_UTILS_ROOT)
	install -t $(AMS_UTILS_ROOT) $(VAR_BIN)/kdmode $(VAR_BIN)/reader
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

NEW_PATH = PATH="$$PWD/var/demo:$$PWD/$(VAR_BIN):$$PATH"
AMS_ROOT = var/links/ams-68k-bin
AMS_RSRC = "$(AMS_ROOT)/mnt/AMS Resources"
AMS_VARS = AMS_BIN=$(AMS_ROOT)/bin AMS_LIB=$(AMS_ROOT)/lib AMS_MNT=$(AMS_ROOT)/mnt
RUN_AMS  = $(NEW_PATH) $(AMS_VARS) v/bin/ams.vx

ams-linux-demo: ams-linux-check display-check
	$(RUN_AMS)

ams-vnc-build: $(AMS_REPOS) build-tools
	$(BUILD) $(AMS_TOOLS) listen vnc-interact

ams-vnc: ams-vnc-build
	EXHIBIT_INTERACT="$$PWD/v/bin/interact-vnc.vx" $(RUN_AMS)

ams-x11-build: $(AMS_REPOS) build-tools
	./build.pl -i $(AMS_TOOLS) interact-x11

ams-x11: ams-x11-build
	EXHIBIT_INTERACT=interact-x11 $(RUN_AMS)

ams-system-rsrcs: build-tools
	$(BUILD) minivx
	$(VAR_BIN)/minivx v/bin/mkrsrc.vx -o $(AMS_RSRC)/rsrc $(AMS_RSRC)

ams-68k-install: var/install ams-system-rsrcs
	install -d var/install/share/ams/bin
	install -d var/install/share/ams/cfg
	install -d var/install/share/ams/lib
	install -d var/install/share/ams/mnt
	install -m444 $(AMS_ROOT)/bin/app    var/install/share/ams/bin
	install -m444 $(AMS_ROOT)/lib/ams-*  var/install/share/ams/lib
	install -m444 $(AMS_ROOT)/lib/vdb    var/install/share/ams/lib
	install -m444 $(AMS_ROOT)/lib/umsp   var/install/share/ams/lib
	cp -R $(AMS_ROOT)/cfg/*              var/install/share/ams/cfg
	cp -R $(AMS_ROOT)/mnt/*              var/install/share/ams/mnt

ams-common-install: var/install
	install -d var/install/bin
	test \! -x $(VAR_BIN)/sndtrack || install $(VAR_BIN)/sndtrack var/install/bin
	install $(VAR_BIN)/minivx      var/install/bin
	install $(VAR_BIN)/skif        var/install/bin
	install $(VAR_BIN)/exhibit     var/install/bin
	install $(VAR_BIN)/graft       var/install/bin
	install $(VAR_BIN)/freemountd  var/install/bin
	install $(VAR_BIN)/xv68k       var/install/bin
	$(INSTALL_SCRIPT) v/bin/ams.vx var/install/bin/ams

ams-vnc-install: ams-vnc-build ams-68k-install ams-common-install
	install $(VAR_BIN)/listen        var/install/bin
	install $(VAR_BIN)/vnc-interact  var/install/bin
	$(INSTALL_SCRIPT) v/bin/interact-vnc.vx var/install/bin/interact

ams-x11-install: ams-x11-build ams-68k-install ams-common-install
	install $(VAR_BIN)/interact-x11  var/install/bin
	@echo '#!/bin/sh'               >  var/install/bin/interact
	@echo 'exec interact-x11 "$$@"' >> var/install/bin/interact
	@chmod +x var/install/bin/interact

ams-quartz-build: $(AMS_REPOS) build-tools
	$(BUILD) graft mbin xv68k fndr-sync minivx freemountd Amethyst Amphitheatre

ams-sdl-build: $(AMS_REPOS) build-tools
	$(BUILD) graft mbin xv68k fndr-sync minivx freemountd Pearl

RETROMATIC := PATH="$$PWD/$(VAR_BIN):$$PATH" v/bin/retromatic.vx
RETRO_APPS := ~/Applications/"Advanced Mac Substitute"

ams-quartz-demo-nosound: ams-quartz-build
	@mkdir -p $(RETRO_APPS)
	$(RETROMATIC) $(RETRO_APPS) ../ams-68k-bin/cfg/nyanochrome-cat.txt
	$(RETROMATIC) $(RETRO_APPS) ../ams-68k-bin/cfg/tic-tac-toe.txt
	$(RETROMATIC) $(RETRO_APPS) ../ams-68k-bin/cfg/black-knights-chess.txt
	$(RETROMATIC) $(RETRO_APPS) ../ams-68k-bin/cfg/organ-console.txt
	@open $(RETRO_APPS)

ams-quartz-demo: sndtrack ams-quartz-demo-nosound

ams-osx-build: $(AMS_REPOS) macward-compat.git build-tools
	bin/build-app -N Interax
	$(BUILD) $(AMS_TOOLS) interact-shim

INTERAX := var/build/*-mach-carb-dbg/bin/Interax/Interax.app

ams-osx: ams-osx-build
	open $(INTERAX)
	EXHIBIT_INTERACT=v/bin/interact-osx.vx $(RUN_AMS)

sndtrack: build-tools
	$(BUILD) minivx
	$(VAR_BIN)/minivx v/bin/portaudio-pkg.vx make
	$(BUILD) sndtrack

freemountd-tcp: freemount.git build-tools
	$(BUILD) freemountd listen

var/freemount/hello.txt:
	mkdir -p var/freemount/
	echo Hello world > var/freemount/hello.txt

freemountd-tcp-test: freemountd-tcp var/freemount/hello.txt
	$(VAR_BIN)/listen 127.0.0.1:4564 $(VAR_BIN)/freemountd --root var/freemount

freemount-tcp: freemount.git build-tools
	$(BUILD) fls fcat fget utcp

fls-test: freemount-tcp
	PATH="$$PWD/$(VAR_BIN):$$PATH" $(VAR_BIN)/fls mnt://127.0.0.1

fcat-test: freemount-tcp
	PATH="$$PWD/$(VAR_BIN):$$PATH" $(VAR_BIN)/fcat mnt://127.0.0.1/hello.txt

.SECONDARY:

