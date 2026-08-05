/*
	res_file.hh
	-----------
*/

#ifndef CPRES_RESFILE_HH
#define CPRES_RESFILE_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif


enum ForkType
{
	dataFork,
	rsrcFork,
};


ResFileRefNum open_res_file( const char* path, ForkType fork );

ResFileRefNum open_res_file( const char* path, ForkType fork, bool exists );

void set_BNDL_bit( const char* path, bool value );

inline
bool has_FSOpenResourceFile()
{
	return TARGET_API_MAC_CARBON  &&  &::FSOpenResourceFile != NULL;
}

#endif
