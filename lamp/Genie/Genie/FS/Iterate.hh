/*
	Iterate.hh
	----------
*/

#ifndef GENIE_FS_ITERATE_HH
#define GENIE_FS_ITERATE_HH

// POSIX
#include <sys/types.h>

// plus
#include "plus/ref_count.hh"

// vfs
#include "vfs/dir_entry.hh"

// Genie
#include "Genie/FS/FSIteratorPtr.hh"
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	class FSIterator : public plus::ref_count< FSIterator >
	{
		public:
			virtual ~FSIterator();
			
			virtual vfs::dir_entry Get() const = 0;
			
			virtual void Advance() = 0;
			
			virtual void Seek( off_t index ) = 0;
			
			virtual off_t Tell() const = 0;
	};
	
	typedef boost::intrusive_ptr< FSIterator > FSIteratorPtr;
	
	
	FSIteratorPtr Iterate( const FSTreePtr& dir );
	
}

#endif

