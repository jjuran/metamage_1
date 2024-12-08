/*
	FSRef_from_path.hh
	------------------
*/

#ifndef MACRELIX_FSREFFROMPATH_HH
#define MACRELIX_FSREFFROMPATH_HH


struct FSRef;


namespace mac   {
namespace relix {
	
	int FSRef_from_path( const char* path, FSRef& result );
	
}
}

#endif
