/*
	compare.hh
	----------
*/

#ifndef MACFILE_COMPARE_HH
#define MACFILE_COMPARE_HH

struct FSRef;
struct FSSpec;


namespace mac  {
namespace file {
	
	bool FSRefs_are_same ( const FSRef&  a, const FSRef&  b );
	bool FSSpecs_are_same( const FSSpec& a, const FSSpec& b );
	
}
}

#endif
