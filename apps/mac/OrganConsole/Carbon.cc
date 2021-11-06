/*
	Carbon.cc
	---------
*/

#include "Carbon.hh"


#if TARGET_API_MAC_CARBON

/*
	This is unimplemented, but allow linking to succeed.
*/

void fourtone_amend_rate( int channel, FTRate rate )
{
}

void fourtone_start( FTRate rate )
{
}

void fourtone_amend_submit()
{
}

void full_stop()
{
}

#endif  // #if TARGET_API_MAC_CARBON
