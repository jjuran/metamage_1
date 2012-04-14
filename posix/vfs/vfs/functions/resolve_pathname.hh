/*
	resolve_pathname.hh
	-------------------
*/

#ifndef VFS_FUNCTIONS_RESOLVEPATHNAME_HH
#define VFS_FUNCTIONS_RESOLVEPATHNAME_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	node_ptr resolve_relative_path( const char*  begin,
	                                std::size_t  length,
	                                const node*  current );
	
	
	node_ptr resolve_absolute_path( const char*  begin,
	                                std::size_t  length );
	
	node_ptr resolve_absolute_path( const plus::string& path );
	
	
	node_ptr resolve_pathname( const char*  begin,
	                           std::size_t  length,
	                           const node*  current = NULL );
	
	node_ptr resolve_pathname( const char*  pathname,
	                           const node*  current = NULL );
	
	node_ptr resolve_pathname( const plus::string&  pathname,
	                           const node*          current = NULL );
	
}

#endif

