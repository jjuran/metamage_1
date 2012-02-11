/*
	link_method_set.hh
	------------------
*/

#ifndef GENIE_FS_LINKMETHODSET_HH
#define GENIE_FS_LINKMETHODSET_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	typedef plus::string (*readlink_method)( const FSTree* );
	
	typedef FSTreePtr (*resolve_method)( const FSTree* );
	
	typedef void (*symlink_method)( const FSTree*, const plus::string& );
	
	
	struct link_method_set
	{
		readlink_method readlink;
		resolve_method  resolve;
		symlink_method  symlink;
	};
	
}

#endif

