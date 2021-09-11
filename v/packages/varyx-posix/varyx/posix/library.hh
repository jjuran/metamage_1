/*
	library.hh
	----------
*/

#ifndef VARYX_POSIX_LIBRARY_HH
#define VARYX_POSIX_LIBRARY_HH

// vlib
#include "vlib/proc_info.hh"


namespace varyx
{
namespace posix
{
	
	using vlib::proc_info;
	
	extern const proc_info proc_EXECV;
	extern const proc_info proc_EXECVP;
	extern const proc_info proc_exit;
	extern const proc_info proc_getenv;
	extern const proc_info proc_getpid;
	extern const proc_info proc_print;
	extern const proc_info proc_RUN;
	extern const proc_info proc_RUNOUT;
	extern const proc_info proc_setenv;
	extern const proc_info proc_sleep;
	extern const proc_info proc_system;
	extern const proc_info proc_SYSTEM;  // unrestricted
	extern const proc_info proc_time;
	extern const proc_info proc_utime;
	extern const proc_info proc_warn;
	
}
}

#endif
