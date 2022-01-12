/*
	freestore_free.cc
	-----------------
	
	Programs that don't use the free store, but do construct objects on the
	stack, by default get saddled with an unneeded exception-handling runtime
	when using the Metrowerks toolchain, but this can be avoided.
	
	The original approach is to include iota/freestore_free.hh in the main
	source file of the program, but that only works if the source is in C++.
	As an alternative, a program can link with this library instead. 
*/

// iota
#include "iota/freestore_free.hh"

#ifndef __MWERKS__

int dummy;

#endif
