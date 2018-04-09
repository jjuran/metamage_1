/*
	signals.hh
	----------
*/

#ifndef VARYX_POSIX_SIGNALS_HH
#define VARYX_POSIX_SIGNALS_HH

// vlib
#include "vlib/proc_info.hh"


namespace varyx
{
namespace posix
{
	
	using vlib::proc_info;
	
	extern const proc_info proc_sig_dfl;
	extern const proc_info proc_sig_ign;
	
}
}

#endif
