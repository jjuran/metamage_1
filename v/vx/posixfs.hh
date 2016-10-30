/*
	posixfs.hh
	----------
*/

#ifndef POSIXFS_HH
#define POSIXFS_HH

// vlib
#include "vlib/proc_info.hh"


namespace vlib
{
	
	extern const proc_info proc_close;
	extern const proc_info proc_dirname;
	extern const proc_info proc_dup;
	extern const proc_info proc_dup2;
	extern const proc_info proc_fstat;
	extern const proc_info proc_listdir;
	extern const proc_info proc_load;
	extern const proc_info proc_lstat;
	extern const proc_info proc_pipe;
	extern const proc_info proc_read;
	extern const proc_info proc_stat;
	extern const proc_info proc_write;
	
	extern const proc_info proc_append;
	extern const proc_info proc_truncate;
	
}

#endif
