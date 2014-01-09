/*
	Genie/FS/sys/mac/thread/default-stack-size.cc
	---------------------------------------------
*/

#include "Genie/FS/sys/mac/thread/default-stack-size.hh"

// mac-sys-utils
#include "mac_sys/default_thread_stack_size.hh"

// plus
#include "plus/serialize.hh"
#include "plus/var_string.hh"


namespace Genie
{
	
	void sys_mac_thread_defaultstacksize::get( plus::var_string& result, const vfs::node* that, bool binary )
	{
		typedef plus::serialize_unsigned< std::size_t > serialize;
		
		const std::size_t data = mac::sys::default_thread_stack_size();
		
		serialize::deconstruct::apply( result, data, binary );
	}
	
}

