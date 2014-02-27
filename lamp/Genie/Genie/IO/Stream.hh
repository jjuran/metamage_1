/*	=========
 *	Stream.hh
 *	=========
 */

#ifndef GENIE_IO_STREAM_HH
#define GENIE_IO_STREAM_HH

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	class StreamHandle : public IOHandle
	{
		public:
			StreamHandle( int                                flags,
			              const vfs::filehandle_method_set*  methods = NULL );
			
			StreamHandle( const vfs::node*                   file,
			              int                                flags,
			              const vfs::filehandle_method_set*  methods = NULL );
			
			virtual ~StreamHandle();
			
			void TryAgainLater() const;
	};
	
}

#endif

