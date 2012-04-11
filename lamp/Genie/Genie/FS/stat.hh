/*
	stat.hh
	-------
*/


// <sys/stat.h>
struct stat;


namespace Genie
{
	
	class FSTree;
	
	void stat( const FSTree* node, struct ::stat& sb );
	
}

