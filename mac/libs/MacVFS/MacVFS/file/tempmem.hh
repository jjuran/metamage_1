/*
	tempmem.hh
	----------
*/

#ifndef MACVFS_FILE_TEMPMEM_HH
#define MACVFS_FILE_TEMPMEM_HH

// plus
#include "plus/string_fwd.hh"

// Genie
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	node_ptr new_sys_mac_tempmem( const node*          parent,
	                              const plus::string&  name,
	                              const void*          args );
	
}

#endif
