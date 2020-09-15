/*
	Nitrogen/CGColorSpace.cc
	------------------------
	
	Copyright 2017 Josh Juran
	
	Distributed under the Boost Software License, Version 1.0.
	(See LICENSE/Boost-1.0.txt in the metamage_1 repository, or visit
	<http://www.boost.org/LICENSE_1_0.txt>.)
*/

#include "Nitrogen/CGColorSpace.hh"


namespace Nitrogen
{
	
	namespace n = nucleus;
	
	
#ifdef MAC_OS_X_VERSION_10_2
	
	n::owned< CGColorSpaceRef > CGColorSpaceCreateWithName( CFStringRef name )
	{
		CGColorSpaceRef result = ::CGColorSpaceCreateWithName( name );
		
		if ( result == NULL )
		{
			throw CGColorSpaceCreateWithName_Failed();
		}
		
		return nucleus::owned< CGColorSpaceRef >::seize( result );
	}
	
#endif
	
}
