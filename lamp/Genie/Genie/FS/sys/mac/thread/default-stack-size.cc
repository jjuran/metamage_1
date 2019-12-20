/*
	Genie/FS/sys/mac/thread/default-stack-size.cc
	---------------------------------------------
*/

#include "Genie/FS/sys/mac/thread/default-stack-size.hh"

// mac-sys-utils
#include "mac_sys/default_thread_stack_size.hh"
#include "mac_sys/has/ThreadManager.hh"

// plus
#include "plus/serialize.hh"
#include "plus/var_string.hh"


namespace Genie
{
	
	static
	void sys_mac_thread_defaultstacksize_get( plus::var_string& result, const vfs::node* that, bool binary )
	{
		using mac::sys::default_thread_stack_size;
		using mac::sys::has_ThreadManager;
		
		typedef plus::serialize_unsigned< std::size_t > serialize;
		
		const bool has = has_ThreadManager();
		
		const std::size_t data = has ? default_thread_stack_size() : 0;
		
		serialize::deconstruct::apply( result, data, binary );
	}
	
	const vfs::property_params sys_mac_thread_defaultstacksize_params =
	{
		4,  // fixed size
		&sys_mac_thread_defaultstacksize_get,
	};
	
}
