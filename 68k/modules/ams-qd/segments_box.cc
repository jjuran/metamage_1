/*
	segments_box.cc
	---------------
*/

#include "segments_box.hh"

// ams-common
#include "scoped_zone.hh"


#pragma exceptions off


Handle segments_storage_handle = (scoped_zone(), NewHandle( 0 ));
