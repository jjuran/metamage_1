/*
	node_method_set.hh
	------------------
*/

#ifndef GENIE_FS_NODEMETHODSET_HH
#define GENIE_FS_NODEMETHODSET_HH

// POSIX
#include <sys/types.h>

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


// #include <time.h>
struct timespec;

// #include <sys/stat.h>
struct stat;


namespace Genie
{
	
	class FSTree;
	
	typedef FSTreePtr (*parent_method)( const FSTree* );
	
	typedef ino_t (*inode_method)( const FSTree* );
	
	typedef void (*stat_method)( const FSTree*, struct ::stat& );
	
	typedef void (*chmod_method)( const FSTree*, mode_t );
	
	typedef void (*touch_method)( const FSTree* );
	typedef void (*utime_method)( const FSTree*, const struct timespec times[2] );
	
	typedef void (*symlink_method)( const FSTree*, const plus::string& );
	
	
	struct node_method_set
	{
		parent_method   parent;
		inode_method    parent_inode;
		stat_method     stat;
		chmod_method    chmod;
		touch_method    touch;
		utime_method    utime;
		symlink_method  symlink;
	};
	
}

#endif

