/*
	Utilities.cc
	------------
*/

#include "Utilities.hh"

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif


pascal Handle GetString_patch( short id )
{
	return GetResource( 'STR ', id );
}

pascal Handle GetIcon_patch( short id )
{
	return GetResource( 'ICON', id );
}

pascal Handle GetPattern_patch( short id )
{
	return GetResource( 'PAT ', id );
}

pascal Handle GetCursor_patch( short id )
{
	return GetResource( 'CURS', id );
}

pascal Handle GetPicture_patch( short id )
{
	return GetResource( 'PICT', id );
}
