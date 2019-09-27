/*
	create_alias.hh
	---------------
*/

#ifndef MACAPP_CREATEALIAS_HH
#define MACAPP_CREATEALIAS_HH

// Mac OS
//#include <Files.h>
struct FSSpec;


namespace mac {
namespace app {
	
#if ! __LP64__
	
	short create_alias( const FSSpec& alias_file, const FSSpec& target );
	
#endif
	
}
}

#endif
