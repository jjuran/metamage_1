/*	=====================
 *	SystemCallRegistry.cc
 *	=====================
 */

// Genie
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	static SystemCallRegistry& TheSystemCallRegistry()
	{
		static SystemCallRegistry theSystemCallRegistry;
		
		return theSystemCallRegistry;
	}
	
	void RegisterSystemCall( const char* name, void* func )
	{
		TheSystemCallRegistry()[ name ] = func;
	}
	
	SystemCallRegistry::const_iterator SystemCallsBegin()
	{
		return TheSystemCallRegistry().begin();
	}
	
	SystemCallRegistry::const_iterator SystemCallsEnd()
	{
		return TheSystemCallRegistry().end();
	}
	
}
