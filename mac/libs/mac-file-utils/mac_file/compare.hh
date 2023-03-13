/*
	compare.hh
	----------
*/

#ifndef MACFILE_COMPARE_HH
#define MACFILE_COMPARE_HH

struct FSSpec;


namespace mac  {
namespace file {
	
	bool FSSpecs_are_same( const FSSpec& a, const FSSpec& b );
	
}
}

#endif
