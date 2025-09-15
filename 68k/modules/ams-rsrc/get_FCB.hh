/*
	get_FCB.hh
	----------
*/

#ifndef GETFCB_HH
#define GETFCB_HH

// ams-common
#include "FCB.hh"


inline
FCB* get_FCB( short refNum )
{
	if ( refNum > 0 )
	{
		return (FCB*) ((char*) FCBSPtr + refNum);
	}
	
	return 0;  // NULL
}

#endif
