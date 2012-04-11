/*
	listdir.hh
	----------
*/

// vfs
#include "vfs/dir_contents_fwd.hh"


namespace Genie
{
	
	class FSTree;
	
	void listdir( const FSTree* node, vfs::dir_contents& contents );
	
}

