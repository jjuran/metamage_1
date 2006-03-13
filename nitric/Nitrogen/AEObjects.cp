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
		Nucleus::OnlyOnce< RegisterObjectSupportLibraryErrors >();
		
		ThrowOSStatus( ::AEObjectInit() );
	}
	
	void AESetObjectCallbacks( OSLCompareUPP        myCompareProc,
	                           OSLCountUPP          myCountProc,
	                           OSLDisposeTokenUPP   myDisposeTokenProc,
	                           OSLGetMarkTokenUPP   myGetMarkTokenProc,
	                           OSLMarkUPP           myMarkProc,
	                           OSLAdjustMarksUPP    myAdjustMarksProc,
	                           OSLGetErrDescUPP     myGetErrDescProcPtr )
	{
		Nucleus::OnlyOnce< RegisterObjectSupportLibraryErrors >();
		
		ThrowOSStatus( ::AESetObjectCallbacks( myCompareProc,
		                                       myCountProc,
		                                       myDisposeTokenProc,
		                                       myGetMarkTokenProc,
		                                       myMarkProc,
		                                       myAdjustMarksProc,
		                                       myGetErrDescProcPtr ) );
	}
	
#if TARGET_RT_MAC_CFM
	
	void AESetObjectCallbacks( ::OSLCompareProcPtr       compareProc,
	                           ::OSLCountProcPtr         countProc,
	                           ::OSLDisposeTokenProcPtr  disposeTokenProc,
	                           ::OSLGetMarkTokenProcPtr  getMarkTokenProc,
	                           ::OSLMarkProcPtr          markProc,
	                           ::OSLAdjustMarksProcPtr   adjustMarksProc,
	                           ::OSLGetErrDescProcPtr    getErrDescProc )
	{
		static ::OSLCompareUPP      compareUPP;
		static OSLCountUPP        countUPP;
		static OSLDisposeTokenUPP disposeTokenUPP;
		static OSLGetMarkTokenUPP getMarkTokenUPP;
		static OSLMarkUPP         markUPP;
		static OSLAdjustMarksUPP  adjustMarksUPP;
		static OSLGetErrDescUPP   getErrDescUPP;
		
		compareUPP      = compareProc      ? ::NewOSLCompareUPP     ( compareProc      ) : NULL;
		countUPP        = countProc        ? NewOSLCountUPP       ( countProc        ) : NULL;
		disposeTokenUPP = disposeTokenProc ? NewOSLDisposeTokenUPP( disposeTokenProc ) : NULL;
		getMarkTokenUPP = getMarkTokenProc ? NewOSLGetMarkTokenUPP( getMarkTokenProc ) : NULL;
		markUPP         = markProc         ? NewOSLMarkUPP        ( markProc         ) : NULL;
		adjustMarksUPP  = adjustMarksProc  ? NewOSLAdjustMarksUPP ( adjustMarksProc  ) : NULL;
		getErrDescUPP   = getErrDescProc   ? NewOSLGetErrDescUPP  ( getErrDescProc   ) : NULL;
		
		
		Nitrogen::AESetObjectCallbacks( compareUPP,
		                                countUPP,
		                                disposeTokenUPP,
		                                getMarkTokenUPP,
		                                markUPP,
		                                adjustMarksUPP,
		                                getErrDescUPP );
	}
	
#endif
	
	Nucleus::Owned< AEToken, AETokenDisposer > AEResolve( const AEObjectSpecifier&  objectSpecifier, 
	                                             AEResolveCallbackFlags    callbackFlags )
	{
		Nucleus::OnlyOnce< RegisterObjectSupportLibraryErrors >();
		
		AEToken token;
		ThrowOSStatus( ::AEResolve( &objectSpecifier, callbackFlags, &token ) );
		return Nucleus::Owned< AEToken, AETokenDisposer >::Seize( token );
	}
	
	Nucleus::Owned< OSLAccessor > AEInstallObjectAccessor( const OSLAccessor& toInstall )
	{
		Nucleus::OnlyOnce< RegisterObjectSupportLibraryErrors >();
		
		ThrowOSStatus( ::AEInstallObjectAccessor( toInstall.desiredClass,
		                                          toInstall.containerType,
		                                          toInstall.accessor,
		                                          toInstall.accessorRefCon,
		                                          toInstall.isSysHandler ) );
		
		return Nucleus::Owned< OSLAccessor >::Seize( toInstall );
	}
	
	OSLAccessor AEGetObjectAccessor( DescType desiredClass,
	                                 DescType containerType,
	                                 bool isSysHandler )
	{
		Nucleus::OnlyOnce< RegisterObjectSupportLibraryErrors >();
		
		::OSLAccessorUPP accessor;
		long accessorRefCon;
		
		ThrowOSStatus( ::AEGetObjectAccessor( desiredClass,
		                                      containerType,
		                                      &accessor,
		                                      &accessorRefCon,
		                                      isSysHandler ) );
		
		return OSLAccessor( desiredClass, containerType, accessor, accessorRefCon, isSysHandler );
	}
	
	Nucleus::Owned< AEToken, AETokenDisposer > AECallObjectAccessor( AEObjectClass   desiredClass,
	                                                                 const AEToken&  containerToken,
	                                                                 AEObjectClass   containerClass,
	                                                                 AEEnumerated    keyForm,
	                                                                 const AEDesc&   keyData )
	{
		Nucleus::OnlyOnce< RegisterObjectSupportLibraryErrors >();
		
		AEToken result;
		
		ThrowOSStatus( ::AECallObjectAccessor( desiredClass,
		                                       &containerToken,
		                                       containerClass,
		                                       keyForm,
		                                       &keyData,
		                                       &result ) );
		
		return Nucleus::Owned< AEToken, AETokenDisposer >::Seize( result );
	}
	
	Nucleus::Owned< AEToken, AETokenDisposer > AECreateToken( DescType typeCode,
	                                                          Nucleus::Owned< AEToken, AETokenDisposer > token )
	{
		AETokenEditor( token ).Get().descriptorType = typeCode;
		
		return token;
	}
	
	void RegisterObjectSupportLibraryErrors()
	{
		Nucleus::OnlyOnce< RegisterAppleEventManagerErrors >();
	}
	
}

