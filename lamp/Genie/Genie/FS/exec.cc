/*
	exec.cc
	-------
*/

#include "Genie/FS/exec.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/file_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	shared_exec_handle exec( const FSTree* it )
	{
		const node_method_set* methods = it->methods();
		
		const file_method_set* file_methods;
		
		if ( methods  &&  (file_methods = methods->file_methods) )
		{
			if ( file_methods->loadexec )
			{
				return file_methods->loadexec( it );
			}
		}
		
		throw p7::errno_t( ENOEXEC );
	}
	
}

