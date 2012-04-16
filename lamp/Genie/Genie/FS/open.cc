/*
	open.cc
	-------
*/

#include "Genie/FS/open.hh"

// POSIX
#include <fcntl.h>

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	IOPtr open( const FSTree* it, int flags, mode_t mode )
	{
		const node_method_set* methods = it->methods();
		
		const data_method_set* data_methods;
		
		if ( methods  &&  (data_methods = methods->data_methods) )
		{
			if ( data_methods->open )
			{
				return data_methods->open( it, flags, mode );
			}
		}
		
		if ( (flags & (O_CREAT|O_EXCL)) == (O_CREAT|O_EXCL) )
		{
			p7::throw_errno( EPERM );
		}
		
		throw p7::errno_t( flags & O_CREAT ? EPERM : ENOENT );
	}
	
}

