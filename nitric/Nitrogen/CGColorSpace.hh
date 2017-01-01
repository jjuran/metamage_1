/*
	Nitrogen/CGColorSpace.hh
	------------------------
	
	Copyright 2017 Josh Juran
	
	Distributed under the Boost Software License, Version 1.0.
	(See LICENSE/Boost-1.0.txt in the metamage_1 repository, or visit
	<http://www.boost.org/LICENSE_1_0.txt>.)
*/

#ifndef NITROGEN_CGCOLORSPACE_HH
#define NITROGEN_CGCOLORSPACE_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef CGCOLORSPACE_H_
#ifndef __CGCOLORSPACE__
#include <CGColorSpace.h>
#endif
#endif

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif


namespace nucleus
{
	
	template <> struct disposer< CGColorSpaceRef >
	{
		typedef CGColorSpaceRef  argument_type;
		typedef void             result_type;
		
		void operator()( CGColorSpaceRef colorSpace ) const
		{
			::CGColorSpaceRelease( colorSpace );
		}
	};
	
}

namespace Nitrogen
{
	
	inline
	nucleus::owned< CGColorSpaceRef > CGColorSpaceCreateDeviceGray()
	{
		CGColorSpaceRef result = ::CGColorSpaceCreateDeviceGray();
		
		return nucleus::owned< CGColorSpaceRef >::seize( result );
	}
	
	inline
	nucleus::owned< CGColorSpaceRef > CGColorSpaceCreateDeviceRGB()
	{
		CGColorSpaceRef result = ::CGColorSpaceCreateDeviceRGB();
		
		return nucleus::owned< CGColorSpaceRef >::seize( result );
	}
	
	inline
	nucleus::owned< CGColorSpaceRef > CGColorSpaceCreateDeviceCMYK()
	{
		CGColorSpaceRef result = ::CGColorSpaceCreateDeviceCMYK();
		
		return nucleus::owned< CGColorSpaceRef >::seize( result );
	}
	
	class CGColorSpaceCreateWithName_Failed {};
	
	nucleus::owned< CGColorSpaceRef >
	//
	CGColorSpaceCreateWithName( CFStringRef name );
	
	inline
	nucleus::owned< CGColorSpaceRef > CGColorSpaceRetain( CGColorSpaceRef colorSpace )
	{
		::CGColorSpaceRetain( colorSpace );
		
		return nucleus::owned< CGColorSpaceRef >::seize( colorSpace );
	}
	
	inline
	void CGColorSpaceRelease( nucleus::owned< CGColorSpaceRef > )
	{
	}
	
}

#endif
