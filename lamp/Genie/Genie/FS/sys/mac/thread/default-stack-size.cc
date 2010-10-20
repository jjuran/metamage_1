/*
	Genie/FS/sys/mac/thread/default-stack-size.cc
	---------------------------------------------
*/

#include "Genie/FS/sys/mac/thread/default-stack-size.hh"

// Mac OS
#ifndef __THREADS__
#include <Threads.h>
#endif

// plus
#include "plus/serialize.hh"
#include "plus/var_string.hh"


namespace Genie
{
	
	static inline std::size_t DefaultThreadStackSize()
	{
		::Size size = 0;
		
		// Jaguar returns paramErr
		OSStatus err = ::GetDefaultThreadStackSize( kCooperativeThread, &size );
		
		return size;
	}
	
	void sys_mac_thread_defaultstacksize::get( plus::var_string& result, const FSTree* that, bool binary )
	{
		typedef plus::serialize_unsigned< std::size_t > serialize;
		
		const std::size_t data = DefaultThreadStackSize();
		
		serialize::deconstruct::apply( result, data, binary );
	}
	
}

