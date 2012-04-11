/*
	mkdir.hh
	--------
*/

// POSIX
#include <sys/types.h>


namespace Genie
{
	
	class FSTree;
	
	void mkdir( const FSTree* node, mode_t mode );
	
}

