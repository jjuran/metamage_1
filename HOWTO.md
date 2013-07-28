How To Use Metamage Code
========================

The Metamage codebase has several build mechanisms:

* Compile::Driver
	- Compile::Driver is a build tool in the form of a collection of Perl modules.  Run `./build.pl <project-name>` from the metamage_1 directory to compile and link a project (along with its prerequisites).  It works for command-line tools on Unix systems.  Build output goes to the var/build directory.

* A-line
	- A-line is an older and more complete build tool in C++ that supports classic Mac OS development and can build OS X applications.  (It can be built using Compile::Driver; `make A-line` does this.)  Build products end up in $HOME/var/build.

* a Makefile
	- Don't be fooled -- this is not a typical Makefile, and trying to use it like one won't have the results you expect.  It's mainly a convenient place for little scripts.

* pack.pl
	- This is a simple Perl script that takes hexidecimal codes as input and produces the corresponding raw bytestream as output, discarding comments.  It's used to transcribe 68K machine code from relatively human-readable text to the raw form expected by v68k and d68k.

