/*
	StrUtils.cc
	-----------
*/

#include "StrUtils.hh"

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif


pascal unsigned char** GetString_patch( short resID )
{
	return (StringHandle) GetResource( 'STR ', resID );
}
