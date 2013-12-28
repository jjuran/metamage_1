/*	==========
 *	Trigger.hh
 *	==========
 */

#ifndef GENIE_FILESYSTEM_TRIGGER_HH
#define GENIE_FILESYSTEM_TRIGGER_HH

// plus
#include "plus/string_fwd.hh"

//vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	typedef void (*trigger_function)( const vfs::node* );
	
	struct trigger_extra
	{
		trigger_function  function;
		intptr_t          data;
		
	};
	
	vfs::node_ptr trigger_factory( const vfs::node*     parent,
	                               const plus::string&  name,
	                               const void*          args );
	
}

#endif

