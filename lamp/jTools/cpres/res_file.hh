/*
	res_file.hh
	-----------
*/

#ifndef CPRES_RESFILE_HH
#define CPRES_RESFILE_HH

// Nitrogen
#ifndef NITROGEN_RESOURCES_HH
#include "Nitrogen/Resources.hh"
#endif


enum ForkType
{
	dataFork,
	rsrcFork,
};


nucleus::owned< Nitrogen::ResFileRefNum >
open_res_file( const char* path, ForkType fork );

nucleus::owned< Nitrogen::ResFileRefNum >
open_res_file( const char* path, ForkType fork, bool exists );

inline
bool has_FSOpenResourceFile()
{
	return TARGET_API_MAC_CARBON  &&  &::FSOpenResourceFile != NULL;
}

#endif
