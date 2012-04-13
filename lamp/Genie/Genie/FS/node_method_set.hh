/*
	node_method_set.hh
	------------------
*/

#ifndef GENIE_FS_NODEMETHODSET_HH
#define GENIE_FS_NODEMETHODSET_HH

// POSIX
#include <sys/types.h>


// #include <time.h>
struct timespec;

// #include <sys/stat.h>
struct stat;


namespace vfs
{
	
	class node;
	
	
	typedef void (*stat_method)( const node*, struct ::stat& );
	
	typedef void (*chmod_method)( const node*, mode_t );
	
	typedef void (*touch_method)( const node* );
	typedef void (*utime_method)( const node*, const struct timespec times[2] );
	
	typedef void (*remove_method)( const node* );
	
	typedef void (*rename_method)( const node*, const node* );
	
	struct data_method_set;
	struct link_method_set;
	struct dir_method_set;
	struct file_method_set;
	struct misc_method_set;
	
	
	struct node_method_set
	{
		stat_method     stat;
		chmod_method    chmod;
		touch_method    touch;
		utime_method    utime;
		remove_method   remove;
		rename_method   rename;
		
		const data_method_set*  data_methods;
		const link_method_set*  link_methods;
		const dir_method_set*   dir_methods;
		const file_method_set*  file_methods;
		const misc_method_set*  misc_methods;
	};
	
}

namespace Genie
{
	
	using vfs::stat_method;
	using vfs::chmod_method;
	using vfs::touch_method;
	using vfs::utime_method;
	using vfs::remove_method;
	using vfs::rename_method;
	
	using vfs::data_method_set;
	using vfs::link_method_set;
	using vfs::dir_method_set;
	using vfs::file_method_set;
	using vfs::misc_method_set;
	
	using vfs::node_method_set;
	
}

#endif

