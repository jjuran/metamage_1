// AEObjects.cp

#ifndef NITROGEN_AEOBJECTS_H
#include "Nitrogen/AEObjects.h"
#endif

namespace Nitrogen {
	
	OSLAccessor::OSLAccessor()
	:
		desiredClass(),
		containerType(),
		accessor(),
		accessorRefCon(),
		isSysHandler()
	{}
	
	bool operator==( const OSLAccessor& a, const OSLAccessor& b )
	{
		return a.accessor        == b.accessor
		    && a.accessorRefCon  == b.accessorRefCon
		    && a.desiredClass    == b.desiredClass
		    && a.containerType   == b.containerType
		    && a.isSysHandler    == b.isSysHandler;
	}
	
	void AEObjectInit()
	{
		ThrowOSStatus( ::AEObjectInit() );
	}
	
	Owned< AEToken, TokenDisposer > AEResolve( const AEObjectSpecifier&  objectSpecifier, 
	                                           AEResolveCallbackFlags    callbackFlags )
	{
		AEToken token;
		ThrowOSStatus( ::AEResolve( &objectSpecifier, callbackFlags, &token ) );
		return Owned< AEToken, TokenDisposer >::Seize( token );
	}
	
	Owned< OSLAccessor > AEInstallObjectAccessor( const OSLAccessor& toInstall )
	{
		ThrowOSStatus( ::AEInstallObjectAccessor( toInstall.desiredClass,
		                                          toInstall.containerType,
		                                          toInstall.accessor,
		                                          toInstall.accessorRefCon,
		                                          toInstall.isSysHandler ) );
		
		return Owned< OSLAccessor >::Seize( toInstall );
	}
	
	OSLAccessor AEGetObjectAccessor( DescType desiredClass,
	                                 DescType containerType,
	                                 bool isSysHandler )
	{
		::OSLAccessorUPP accessor;
		long accessorRefCon;
		
		ThrowOSStatus( ::AEGetObjectAccessor( desiredClass,
		                                      containerType,
		                                      &accessor,
		                                      &accessorRefCon,
		                                      isSysHandler ) );
		
		return OSLAccessor( desiredClass, containerType, accessor, accessorRefCon, isSysHandler );
	}
	
}

