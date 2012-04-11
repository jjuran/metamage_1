/*
	seteof.hh
	---------
*/

// POSIX
#include <sys/types.h>


namespace Genie
{
	
	class FSTree;
	
	void seteof( const FSTree* node, off_t length );
	
}

