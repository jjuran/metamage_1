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

ams-linux-tools: $(AMS_REPOS)
	./build.pl -i $(AMS_TOOLS) display-linux interact-linux kdmode reader

ams-vx-Z:
	@echo 'vx -Z "$$@"' > bin/"vx -Z"
	@chmod +x bin/"vx -Z"

ams-linux: ams-linux-tools ams-vx-Z
	@test -x bin/display     || ln -s ../var/out/display-linux  bin/display
	@test -x bin/interact    || ln -s ../var/out/interact-linux bin/interact
	@test -x bin/spiel-mouse || ln -s ../v/bin/spiel-mouse.vx bin/spiel-mouse
	@echo
	@echo "Build phase complete.  Run \`make ams-linux-opt\` to continue."
	@echo

ams-linux-opt:
	@echo
	@echo "Note: root privileges required to install under /opt"
	sudo mkdir -p /opt/metamage/bin
	sudo cp var/out/kdmode var/out/reader /opt/metamage/bin/
	sudo chown root:root /opt/metamage/bin/kdmode /opt/metamage/bin/reader
	@echo
	@echo "Utilities installed.  Run \`make ams-linux-demo\` to continue."
	@echo

ams-linux-suid:
	@echo
	@echo "Note: root privileges required for setuid-root"
	sudo chmod +s /opt/metamage/bin/kdmode

ams-linux-demo:
	@test -d ~/68k || ln -s "$$PWD/../ams-68k-bin/68k" ~/
	PATH="$$PWD/bin:$$PWD/var/out:$$PATH" ./scripts/ams Welcome

ams-osx: $(AMS_REPOS) macward-compat.git
	bin/build-app Genie
	mkdir -p ~/var/run/fs
	open var/build/dbg/bin/Genie/MacRelix.app
	./build.pl -i $(AMS_TOOLS) uunix interact
	PATH="$$PWD/var/out:$$PATH" ./scripts/ams Welcome

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

