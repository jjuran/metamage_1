How To Use Metamage Code
========================

The Metamage codebase has several build mechanisms:

* Compile::Driver
	- Compile::Driver is a build tool in the form of a collection of Perl modules.  Run `./build.pl <project-name>` from the metamage_1 directory to compile and link a project (along with its prerequisites).  It works on Unix systems for projects of all types, including OS X applications.  Build output goes to the var/build directory.

* A-line
	- A-line is an older build tool in C++ that supports classic Mac OS development.  It works in Unix environments generally, but is only needed in MacRelix.  Build products end up in $HOME/var/build.

* GNUmakefile
	- Don't be fooled -- this is not a typical makefile, and trying to use it like one won't have the results you expect.  It's mainly a convenient place for little scripts.

* pack.pl
	- This is a simple Perl script that takes hexadecimal codes as input and produces the corresponding raw bytestream as output, discarding comments.  It's used to transcribe 68K machine code from relatively human-readable text to the raw form expected by v68k and d68k.
