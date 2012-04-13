/*
	seteof.cc
	---------
*/

#include "Genie/FS/seteof.hh"

// POSIX
#include <sys/stat.h>

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	void seteof( const FSTree* it, off_t length )
	{
		const node_method_set* methods = it->methods();
		
		const data_method_set* data_methods;
		
		if ( methods  &&  (data_methods = methods->data_methods) )
		{
			if ( data_methods->seteof )
			{
				data_methods->seteof( it, length );
				
				return;
			}
		}
		
		// This confuses MWCPPC when optimizing:
		//p7::throw_errno( S_ISDIR( it->filemode() ) ? EISDIR : EINVAL );
		// internal compiler error: File: 'PCodeUtilities.c' Line: 80
		
		const volatile int error = S_ISDIR( it->filemode() ) ? EISDIR : EINVAL;
		
		p7::throw_errno( error );
	}
	
}

