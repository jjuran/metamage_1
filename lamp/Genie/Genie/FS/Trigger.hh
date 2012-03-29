/*	==========
 *	Trigger.hh
 *	==========
 */

#ifndef GENIE_FILESYSTEM_TRIGGER_HH
#define GENIE_FILESYSTEM_TRIGGER_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	typedef void (*trigger_function)( const FSTree* );
	
	struct trigger_extra
	{
		trigger_function  function;
		intptr_t          data;
		
	};
	
	FSTreePtr trigger_factory( const FSTreePtr&     parent,
	                           const plus::string&  name,
	                           const void*          args );
	
}

#endif

