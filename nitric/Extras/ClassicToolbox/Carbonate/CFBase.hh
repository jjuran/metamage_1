// Carbonate/CFBase.hh

#ifndef CARBONATE_CFBASE_HH
#define CARBONATE_CFBASE_HH

#ifndef __CFBASE__
#include <CFBase.h>
#endif

#if TARGET_API_MAC_CARBON
#error Configuration error:  This file is for classic only
#endif


struct CFObject
{
	ItemCount retainCount;
	
	CFObject() : retainCount( 1 )  {}
};

template < class CFType >
CFType CFType_Cast( CFTypeRef cf )
{
	return static_cast< CFType >( const_cast< void* >( cf ) );
}


#endif

