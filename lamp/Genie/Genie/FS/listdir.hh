/*
	listdir.hh
	----------
*/


namespace vfs
{
	
	class dir_contents;
	
}

namespace Genie
{
	
	class FSTree;
	
	void listdir( const FSTree* node, vfs::dir_contents& contents );
	
}

