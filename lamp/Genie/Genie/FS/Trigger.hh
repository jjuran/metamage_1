/*	==========
 *	Trigger.hh
 *	==========
 */

#ifndef GENIE_FILESYSTEM_TRIGGER_HH
#define GENIE_FILESYSTEM_TRIGGER_HH

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	class Trigger_Base : public FSTree
	{
		public:
			Trigger_Base( const FSTreePtr&     parent,
			              const plus::string&  name,
			              mode_t               mode = S_IFCHR | 0200 );
			
			virtual void Invoke() const = 0;
	};
	
	
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

