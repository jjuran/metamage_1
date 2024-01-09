/*
	remotefs.cc
	-----------
*/

#include "remotefs.hh"


#ifndef NULL
#define NULL  0L
#endif


const Byte* remotefs_lookup( VCB* vcb, const Byte* name )
{
	if ( name == NULL  ||  name[ 0 ] == '\0' )
	{
		return NULL;
	}
	
	return name;
}
