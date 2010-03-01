// Nitrogen/AEObjects.cc
// ---------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/AEObjects.hh"


namespace Nitrogen
{
	
	static AppleEventManagerErrorsRegistrationDependency gAppleEventManagerErrorsRegistrationDependency;
	
	
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
	
	void AESetObjectCallbacks( OSLCompareUPP        myCompareProc,
	                           OSLCountUPP          myCountProc,
	                           OSLDisposeTokenUPP   myDisposeTokenProc,
	                           OSLGetMarkTokenUPP   myGetMarkTokenProc,
	                           OSLMarkUPP           myMarkProc,
	                           OSLAdjustMarksUPP    myAdjustMarksProc,
	                           OSLGetErrDescUPP     myGetErrDescProcPtr )
	{
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
	
	nucleus::owned< AEDesc_Token > AEResolve( const AEDesc_ObjectSpecifier&  objectSpecifier, 
	                                          AEResolveCallbackFlags         callbackFlags )
	{
		AEDesc_Token token;
		
		ThrowOSStatus( ::AEResolve( &objectSpecifier, callbackFlags, &token ) );
		
		return nucleus::owned< AEDesc_Token >::seize( token );
	}
	
	nucleus::owned< OSLAccessor > AEInstallObjectAccessor( const OSLAccessor& toInstall )
	{
		ThrowOSStatus( ::AEInstallObjectAccessor( toInstall.desiredClass,
		                                          toInstall.containerType,
		                                          toInstall.accessor,
		                                          toInstall.accessorRefCon,
		                                          toInstall.isSysHandler ) );
		
		return nucleus::owned< OSLAccessor >::seize( toInstall );
	}
	
	OSLAccessor AEGetObjectAccessor( AEObjectClass  desiredClass,
	                                 DescType       containerType,
	                                 bool           isSysHandler )
	{
		::OSLAccessorUPP accessor;
		long accessorRefCon;
		
		ThrowOSStatus( ::AEGetObjectAccessor( desiredClass,
		                                      containerType,
		                                      &accessor,
		                                      &accessorRefCon,
		                                      isSysHandler ) );
		
		return OSLAccessor( desiredClass,
		                    containerType,
		                    accessor,
		                    accessorRefCon,
		                    isSysHandler );
	}
	
	nucleus::owned< AEDesc_Token > AECallObjectAccessor( AEObjectClass        desiredClass,
	                                                     const AEDesc_Token&  containerToken,
	                                                     AEObjectClass        containerClass,
	                                                     AEEnumerated         keyForm,
	                                                     const AEDesc_Data&   keyData )
	{
		AEDesc_Token result;
		
		ThrowOSStatus( ::AECallObjectAccessor( desiredClass,
		                                       &containerToken,
		                                       containerClass,
		                                       keyForm,
		                                       &keyData,
		                                       &result ) );
		
		return nucleus::owned< AEDesc_Token >::seize( result );
	}
	
}

