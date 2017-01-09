/*
	item_method_set.hh
	------------------
*/

#ifndef VFS_METHODS_ITEMMETHODSET_HH
#define VFS_METHODS_ITEMMETHODSET_HH

// POSIX
#include <sys/types.h>


// #include <time.h>
struct timespec;

// #include <sys/stat.h>
struct stat;


namespace vfs
{
	
	class node;
	
	
	typedef void (*stat_method)( const node*, struct stat& );
	
	typedef void (*chmod_method)( const node*, mode_t );
	
	typedef void (*touch_method)( const node* );
	typedef void (*utime_method)( const node*, const struct timespec times[2] );
	
	typedef void (*remove_method)( const node* );
	
	typedef void (*rename_method)( const node*, const node* );
	
	
	struct item_method_set
	{
		stat_method    stat;
		chmod_method   chmod;
		touch_method   touch;
		utime_method   utime;
		remove_method  remove;
		rename_method  rename;
	};
	
}

#endif
