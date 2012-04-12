/*
	node_method_set.hh
	------------------
*/

#ifndef GENIE_FS_NODEMETHODSET_HH
#define GENIE_FS_NODEMETHODSET_HH

// POSIX
#include <sys/types.h>

// Genie
#include "Genie/FS/FSTree_fwd.hh"


// #include <time.h>
struct timespec;

// #include <sys/stat.h>
struct stat;


namespace Genie
{
	
	typedef void (*stat_method)( const FSTree*, struct ::stat& );
	
	typedef void (*chmod_method)( const FSTree*, mode_t );
	
	typedef void (*touch_method)( const FSTree* );
	typedef void (*utime_method)( const FSTree*, const struct timespec times[2] );
	
	typedef void (*remove_method)( const FSTree* );
	
	typedef void (*rename_method)( const FSTree*, const FSTree* );
	
	struct data_method_set;
	struct link_method_set;
	struct dir_method_set;
	struct file_method_set;
	struct misc_method_set;
	
}

namespace vfs
{
	
	using Genie::stat_method;
	using Genie::chmod_method;
	using Genie::touch_method;
	using Genie::utime_method;
	using Genie::remove_method;
	using Genie::rename_method;
	
	using Genie::data_method_set;
	using Genie::link_method_set;
	using Genie::dir_method_set;
	using Genie::file_method_set;
	using Genie::misc_method_set;
	
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
	
	using vfs::node_method_set;
	
}

#endif

