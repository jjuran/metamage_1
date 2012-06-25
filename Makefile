# Makefile for <https://github.com/jjuran/metamage_1>

ALINE = var/build/dbg/bin/A-line/A-line

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

A-line:
	./build.pl A-line
	cp $(ALINE) .

