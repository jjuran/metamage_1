/*
	chmod.hh
	--------
*/

// POSIX
#include <sys/types.h>


namespace Genie
{
	
	class FSTree;
	
	void chmod( const FSTree* node, mode_t mode );
	
}

