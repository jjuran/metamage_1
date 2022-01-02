/*
	posixfs.hh
	----------
*/

#ifndef VARYX_POSIX_POSIXFS_HH
#define VARYX_POSIX_POSIXFS_HH

// vlib
#include "vlib/proc_info.hh"


namespace varyx
{
namespace posix
{
	
	using vlib::proc_info;
	using vlib::Value;
	
	extern const proc_info proc_chdir;
	extern const proc_info proc_close;
	extern const proc_info proc_dirname;
	extern const proc_info proc_dup;
	extern const proc_info proc_dup2;
	extern const proc_info proc_fstat;
	extern const proc_info proc_isatty;
	extern const proc_info proc_link;
	extern const proc_info proc_listdir;
	extern const proc_info proc_load;
	extern const proc_info proc_lstat;
	extern const proc_info proc_mkdir;
	extern const proc_info proc_pipe;
	extern const proc_info proc_read;
	extern const proc_info proc_reader;
	extern const proc_info proc_readlink;
	extern const proc_info proc_realpath;
	extern const proc_info proc_remove;
	extern const proc_info proc_rename;
	extern const proc_info proc_rewrite;
	extern const proc_info proc_stat;
	extern const proc_info proc_symlink;
	extern const proc_info proc_touch;
	extern const proc_info proc_write;
	
	extern const proc_info proc_append;
	extern const proc_info proc_truncate;
	
	void send_data( int fd, const Value& v );
	
}
}

#endif
