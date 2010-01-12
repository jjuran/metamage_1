/*	========
 *	Entry.hh
 *	========
 */

#ifndef GENIE_PROCESS_ENTRY_HH
#define GENIE_PROCESS_ENTRY_HH

// Mac OS
#ifndef __LOWMEM__
#include <LowMem.h>
#endif

// Iota
#include "iota/environ.hh"


namespace Genie
{
	
	typedef void (*Dispatcher)( unsigned );
	
	struct ToolScratchGlobals
	{
		Dispatcher    dispatcher;
		iota::envp_t  envp;
	};
	
	struct ApplScratchGlobals
	{
		void*            reserved1;
		void*            reserved2;
		void*            reserved3;
	};
	
	inline ToolScratchGlobals& GetToolScratchGlobals()
	{
		return *reinterpret_cast< ToolScratchGlobals* >( LMGetToolScratch() );
	}
	
	inline void SetToolScratchGlobals( const ToolScratchGlobals& globals )
	{
		GetToolScratchGlobals() = globals;
	}
	
	void SetUpToolScratch( Dispatcher dispatcher, iota::envp_t envp );
	
}

#endif

