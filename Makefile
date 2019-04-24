# Makefile for <https://github.com/jjuran/metamage_1>

REPOS =
REPOS += ams-68k-bin
REPOS += freemount
REPOS += git
REPOS += macward-compat

D68K  = var/build/dbg/bin/d68k/d68k
XV68K = var/build/dbg/bin/xv68k/xv68k

PACK68K = utils/pack.pl

METAMAGE_1 = `git remote -v | grep '^origin.*\(fetch\)' | awk '{print $$2}'`
PLEASE_RUN = 'Please run `(cd .. && git clone' $(METAMAGE_1)')`.'

BUILD_FLAG = `uname -m | grep x86_64 | sed s/x86_64/-W/`

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
	./build.pl -i vx

AMS_REPOS := freemount.git ams-68k-bin.git
AMS_TOOLS := exhibit graft raster vx xv68k freemountd

AMS_UTILS_ROOT := var/install/lib/metamage

var/install:
	@echo
	@echo "Please run \`./configure\`.  Or, if you lack root access, run"
	@echo
	@echo "    INSTALL_PREFIX=\$$HOME ./configure"
	@echo
	@echo "instead."
	@echo
	@exit 1

ams-linux-tools: $(AMS_REPOS) var/install
	./build.pl -i $(AMS_TOOLS) display-linux interact-linux kdmode reader

ams-vx-Z:
	@echo 'vx -Z "$$@"' > bin/"vx -Z"
	@chmod +x bin/"vx -Z"

ams-linux: ams-linux-tools ams-vx-Z
	@test -x bin/display     || ln -s ../var/out/display-linux  bin/display
	@test -x bin/interact    || ln -s ../var/out/interact-linux bin/interact
	@test -x bin/spiel-mouse || ln -s ../v/bin/spiel-mouse.vx bin/spiel-mouse
	@test -x bin/spiel-keyboard || ln -s ../v/bin/spiel-keyboard.vx bin/spiel-keyboard
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

ams-linux-demo: ams-linux-check display-check
	PATH="$$PWD/bin:$$PWD/var/out:$$PATH" ./scripts/ams

ams-x11-build: $(AMS_REPOS) ams-vx-Z
	./build.pl -i $(AMS_TOOLS) interact-x11

ams-x11: ams-x11-build
	PATH="$$PWD/bin:$$PWD/var/out:$$PATH" EXHIBIT_INTERACT=interact-x11 ./scripts/ams

ams-osx: $(AMS_REPOS) macward-compat.git
	bin/build-app Genie
	mkdir -p ~/var/run/fs
	open var/build/dbg/bin/Genie/MacRelix.app
	./build.pl -i $(AMS_TOOLS) uunix interact
	PATH="$$PWD/var/out:$$PATH" ./scripts/ams

sndtrack:
	./build.pl -i vx
	var/out/vx -Z v/bin/portaudio-pkg.vx make
	./build.pl -i $(BUILD_FLAG) sndtrack

d68k:
	./build.pl d68k

d68k-hello: d68k
	$(PACK68K) v68k/demos/hello.p68k | $(D68K)

d68k-fizzbuzz: d68k
	$(PACK68K) v68k/demos/fizzbuzz.p68k | $(D68K)

freemountd-tcp: freemount.git
	./build.pl -i freemountd listen

var/freemount/hello.txt:
	mkdir -p var/freemount/
	echo Hello world > var/freemount/hello.txt

freemountd-tcp-test: freemountd-tcp var/freemount/hello.txt
	var/out/listen 127.0.0.1:4564 var/out/freemountd --root var/freemount

freemount-tcp: freemount.git
	./build.pl -i fls fcat fget utcp

fls-test: freemount-tcp
	PATH="$$PWD/var/out:$$PATH" var/out/fls mnt://127.0.0.1

fcat-test: freemount-tcp
	PATH="$$PWD/var/out:$$PATH" var/out/fcat mnt://127.0.0.1/hello.txt

xv68k:
	./build.pl xv68k

xv68k-rts: xv68k
	echo 4E75 | $(PACK68K) | $(XV68K)

xv68k-hello: xv68k
	$(PACK68K) v68k/demos/hello.p68k | $(XV68K)

xv68k-ill: xv68k
	echo 4AFC 4E75 | $(PACK68K) | $(XV68K)

xv68k-ill-priv: xv68k
	echo 4E72 4E75 | $(PACK68K) | $(XV68K)

xv68k-ill-F: xv68k
	echo FFFF 4E75 | $(PACK68K) | $(XV68K)

xv68k-segv: xv68k
	echo 2010 4E75 | $(PACK68K) | $(XV68K)

xv68k-segv-pc: xv68k
	perl -e 'print pack "n*", 0x4EF8, 0x0000, 0x4E75' | $(XV68K)
	echo 4EF8 0000 4E75 | $(PACK68K) | $(XV68K)

xv68k-bus: xv68k
	echo 202F 0001 4E75 | $(PACK68K) | $(XV68K)

xv68k-bus-pc: xv68k
	echo 6001 4E71 4E75 | $(PACK68K) | $(XV68K)

xv68k-div0: xv68k
	echo 80C1 4E75 | $(PACK68K) | $(XV68K)

.SECONDARY:

