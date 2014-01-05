/*	==============
 *	RegularFile.hh
 *	==============
 */

#ifndef GENIE_IO_REGULARFILE_HH
#define GENIE_IO_REGULARFILE_HH

// vfs
#include "vfs/filehandle.hh"


namespace Genie
{
	
	class RegularFileHandle : public vfs::filehandle
	{
		public:
			RegularFileHandle( int                                flags,
			                   const vfs::filehandle_method_set*  methods = NULL );
			
			RegularFileHandle( const vfs::node_ptr&               file,
			                   int                                flags,
			                   const vfs::filehandle_method_set*  methods = NULL );
			
			virtual ~RegularFileHandle();
	};
	
}

#endif

