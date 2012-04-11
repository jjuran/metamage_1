/*
	geteof.hh
	---------
*/

// POSIX
#include <sys/types.h>


namespace Genie
{
	
	class FSTree;
	
	off_t geteof( const FSTree* node );
	
}

