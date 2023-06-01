/*	==========
 *	MacFile.hh
 *	==========
 */

#ifndef GENIE_IO_MACFILE_HH
#define GENIE_IO_MACFILE_HH

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_ptr.hh"

// <Files.h>
typedef signed char SInt8;
struct FSSpec;


namespace Genie
{
	
	typedef short (*ForkOpener)( const FSSpec& file, SInt8 perm );
	
	typedef vfs::node_ptr (*FileGetter)( const FSSpec& );
	
	
	vfs::filehandle_ptr
	//
	OpenMacFileHandle( const FSSpec&  fileSpec,
	                   int            flags,
	                   ForkOpener     openFork,
	                   FileGetter     getFile );
	
}

#endif
