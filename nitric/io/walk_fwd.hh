// io/walk_fwd.hh
// --------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007-2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef IO_WALKFWD_HH
#define IO_WALKFWD_HH

// Io
#ifndef IO_FILES_HH
#include "io/files.hh"
#endif



namespace io
{
	
	template < class FileSpec > struct directory_contents_traits;
	
	namespace you_missed_some_includes  // dummy
	{
		
		class directory_stream {};
		
	}
	
}

#endif

