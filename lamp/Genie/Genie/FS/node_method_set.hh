/*
	node_method_set.hh
	------------------
*/

#ifndef GENIE_FS_NODEMETHODSET_HH
#define GENIE_FS_NODEMETHODSET_HH

// vfs
#include "vfs/methods/node_method_set.hh"


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

