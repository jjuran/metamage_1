// Nitrogen/CGDataProvider.hh
// --------------------------

// Part of the Nitrogen project.
//
// Written 2004-2016 by Marshall Clow, Lisa Lippincott, and Josh Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_CGDATAPROVIDER_HH
#define NITROGEN_CGDATAPROVIDER_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef CGDATAPROVIDER_H_
#include <CGDataProvider.h>
#endif

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif


#if UNIVERSAL_INTERFACES_VERSION < 0x0400

typedef CGReleaseDataProcPtr CGDataProviderReleaseDataCallback;

#elif ! defined( MAC_OS_X_VERSION_10_4 )

typedef void (*CGDataProviderReleaseDataCallback)(void*, const void*, size_t);

#endif

namespace nucleus
{
	
	template <> struct disposer< CGDataProviderRef >
	{
		typedef CGDataProviderRef  argument_type;
		typedef void               result_type;
		
		void operator()( CGDataProviderRef image ) const
		{
			::CGDataProviderRelease( image );
		}
	};
	
}

namespace Nitrogen
{
#if UNIVERSAL_INTERFACES_VERSION >= 0x0400
	
	using ::CGDataProviderGetTypeID;
	
#endif
	
	/*
		Normally we don't add new arguments to functions, but this is a
		special case:  The caller is passing ownership of the data to the new
		data provider.  This raises a question about exception safety:  What
		happens if ::CGDataProviderCreateWithData() fails and an exception is
		thrown?  Either the caller remains the owner until successful creation
		of the data provider, and must catch and rethrow the exception (after
		manually releasing the data if necessary), or the called function
		assumes ownership immediately, and must release the data itself on
		failure.  Clearly, the former violates the spirit of Nitrogen.
		
		The new fifth argument is a release-data callback of the same type as
		the fourth argument -- in fact, the expected usage is to pass the same
		function as both arguments.  If creating the data provider fails, and
		the `fail` argument is non-NULL, it will be called to release the data
		before the exception is thrown.  If the caller prefers to retain
		ownership on exception, it can pass NULL here.  To avoid likely errors,
		neither release-data callback argument is optional.
	*/
	
	class CGDataProviderCreateWithData_Failed {};
	
	static
	nucleus::owned< CGDataProviderRef >
	//
	CGDataProviderCreateWithData( void*                              info,
	                              const void*                        data,
	                              size_t                             size,
	                              CGDataProviderReleaseDataCallback  free,
	                              CGDataProviderReleaseDataCallback  fail )
	{
		CGDataProviderRef result = ::CGDataProviderCreateWithData( info,
		                                                           data,
		                                                           size,
		                                                           free );
		
		if ( result == NULL )
		{
			if ( fail != NULL )
			{
				fail( info, data, size );
			}
			
			throw CGDataProviderCreateWithData_Failed();
		}
		
		return nucleus::owned< CGDataProviderRef >::seize( result );
	}
	
	static inline
	nucleus::owned< CGDataProviderRef >
	//
	CGDataProviderCreateWithData( const void*                        data,
	                              size_t                             size,
	                              CGDataProviderReleaseDataCallback  free,
	                              CGDataProviderReleaseDataCallback  fail )
	{
		namespace N = Nitrogen;
		
		return N::CGDataProviderCreateWithData( NULL, data, size, free, fail );
	}
	
#ifdef MAC_OS_X_VERSION_10_4
	
	class CGDataProviderCreateWithCFData_Failed {};
	
	inline
	nucleus::owned< CGDataProviderRef >
	//
	CGDataProviderCreateWithCFData( CFDataRef data )
	{
		CGDataProviderRef result = ::CGDataProviderCreateWithCFData( data );
		
		if ( result == NULL )
		{
			throw CGDataProviderCreateWithCFData_Failed();
		}
		
		return nucleus::owned< CGDataProviderRef>::seize( result );
	}
	
#endif
	
	class CGDataProviderCreateWithURL_Failed {};
	
	inline
	nucleus::owned< CGDataProviderRef >
	//
	CGDataProviderCreateWithURL( CFURLRef url )
	{
		CGDataProviderRef result = ::CGDataProviderCreateWithURL( url );
		
		if ( result == NULL )
		{
			throw CGDataProviderCreateWithURL_Failed();
		}
		
		return nucleus::owned< CGDataProviderRef>::seize( result );
	}
	
	inline
	nucleus::owned< CGDataProviderRef >
	//
	CGDataProviderRetain( CGDataProviderRef dataProvider )
	{
		::CGDataProviderRetain( dataProvider );
		
		return nucleus::owned< CGDataProviderRef >::seize( dataProvider );
	}
	
	inline
	void CGDataProviderRelease( nucleus::owned< CGDataProviderRef > )
	{
	}
	
}

#endif
