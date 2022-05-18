/*
	FSSpec_from_path.hh
	-------------------
*/

#ifndef MACRELIX_FSSPECFROMPATH_HH
#define MACRELIX_FSSPECFROMPATH_HH


struct FSSpec;


namespace mac   {
namespace relix {
	
	int FSSpec_from_existing_path( const char* path, FSSpec& result );
	
}
}

#endif
