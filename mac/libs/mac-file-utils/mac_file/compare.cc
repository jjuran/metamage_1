/*
	compare.cc
	----------
*/

#include "mac_file/compare.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// mac-glue-utils
#include "mac_glue/StringCompare.hh"


#pragma exceptions off


namespace mac  {
namespace file {

#if ! __LP64__

bool FSSpecs_are_same( const FSSpec& a, const FSSpec& b )
{
	return a.vRefNum == b.vRefNum  &&
	       a.parID   == b.parID    &&
	       mac::glue::EqualString_sans_case( a.name, b.name );
}

#else

int dummy;

#endif  // #if ! __LP64__

}
}
