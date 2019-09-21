/*
	functions.hh
	------------
*/

#ifndef ED25519_FUNCTIONS_HH
#define ED25519_FUNCTIONS_HH


namespace vlib
{
	
	struct proc_info;
	
}

namespace varyx
{
namespace ed25519
{
	
	using vlib::proc_info;
	
	extern const proc_info proc_mkpub;
	extern const proc_info proc_sign;
	extern const proc_info proc_verify;
	
}
}

#endif
