# Makefile for <https://github.com/jjuran/metamage_1>

REPOS = git macward-compat

ALINE = var/build/dbg/bin/A-line/A-line
D68K  = var/build/dbg/bin/d68k/d68k
XV68K = var/build/dbg/bin/xv68k/xv68k

PACK68K = engines/v68k/utils/pack68k.pl

default:
	@echo 'For help, run `make help`.'

help:
	@echo 'To build a command-line tool, run `./build.pl $$toolname`.'
	@echo 'To build a Mac app, first `make A-line` and then `./A-line $$appname`.'
	@echo "If build.pl can't find a new config file, run \`make catalog\` and retry."

rm-catalog:
	rm -f var/cache/compile-driver/catalog

catalog: rm-catalog
	./build.pl

LINKS/%:
	@echo $(REPOS) | grep $* > /dev/null || (echo Unknown repo $*; exit 1)
	@test -d ../$*/.git || (echo 'Please run `(cd .. && git clone git://github.com/jjuran/$*.git)`.'; exit 128)
	ln -s ../../$* LINKS/

%.git: LINKS/%
	@true

A-line:
	./build.pl A-line
	cp $(ALINE) .

d68k:
	./build.pl d68k

d68k-hello: d68k
	$(PACK68K) engines/v68k/demos/hello.p68k | $(D68K)

d68k-fizzbuzz: d68k
	$(PACK68K) engines/v68k/demos/fizzbuzz.p68k | $(D68K)

xv68k:
	./build.pl xv68k

xv68k-rts: xv68k
	echo 4E75 | $(PACK68K) | $(XV68K)

xv68k-ExitToShell: xv68k
	echo A9F4 | $(PACK68K) | $(XV68K)

xv68k-beep: xv68k
	echo 3F00 A9C8 4E75 | $(PACK68K) | $(XV68K)

xv68k-hello: xv68k
	$(PACK68K) engines/v68k/demos/hello.p68k | $(XV68K)

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

