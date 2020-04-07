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

// nucleus
#ifndef NUCLEUS_ERRORSREGISTERED_HH
#include "nucleus/errors_registered.hh"
#endif


namespace Nitrogen
{
	
	using Mac::ThrowOSStatus;
	
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( AppleEventManager );
	
	NUCLEUS_REQUIRE_ERRORS_STATIC( AppleEventManager )
	
	
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
	
	nucleus::owned< Mac::AEDesc_Token >
	//
	AEResolve( const Mac::AEDesc_ObjectSpecifier&  objectSpecifier,
	           AEResolveCallbackFlags              callbackFlags )
	{
		Mac::AEDesc_Token token;
		
		ThrowOSStatus( ::AEResolve( &objectSpecifier, callbackFlags, &token ) );
		
		return nucleus::owned< Mac::AEDesc_Token >::seize( token );
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
	
	OSLAccessor AEGetObjectAccessor( Mac::AEObjectClass  desiredClass,
	                                 Mac::DescType       containerType,
	                                 bool                isSysHandler )
	{
		::OSLAccessorUPP accessor;
		SRefCon accessorRefCon;
		
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
	
	nucleus::owned< Mac::AEDesc_Token >
	//
	AECallObjectAccessor( Mac::AEObjectClass        desiredClass,
	                      const Mac::AEDesc_Token&  containerToken,
	                      Mac::AEObjectClass        containerClass,
	                      Mac::AEKeyForm            keyForm,
	                      const Mac::AEDesc_Data&   keyData )
	{
		Mac::AEDesc_Token result;
		
		ThrowOSStatus( ::AECallObjectAccessor( desiredClass,
		                                       &containerToken,
		                                       containerClass,
		                                       keyForm,
		                                       &keyData,
		                                       &result ) );
		
		return nucleus::owned< Mac::AEDesc_Token >::seize( result );
	}
	
#if defined( __GNUC__ ) && ( __GNUC__ * 100 + __GNUC_MINOR__ <= 303 )
	
	// Panther's g++ 3.3 can't handle this.
	
#else
	
	namespace compile_tests
	{
		
		pascal OSErr ObjectAccessor( ::DescType     desiredClass,
		                             const AEDesc*  containerToken,
		                             ::DescType     containerClass,
		                             ::DescType     keyForm,
		                             const AEDesc*  keyData,
		                             AEDesc*        value,
		                             ::SRefCon      refCon );
		
		static void install_ProcPtr()
		{
			return;
			
			Mac::AEObjectClass  desiredClass  = Mac::cWindow;
			Mac::DescType       containerType = Mac::typeNull;
			bool                isSysHandler  = false;
			
			(void) AEInstallObjectAccessor< ObjectAccessor >( desiredClass,
			                                                  containerType,
			                                                  ::SRefCon(),
			                                                  isSysHandler );
			
			(void) AEInstallObjectAccessor< ObjectAccessor >( desiredClass,
			                                                  containerType,
			                                                  ::SRefCon() );
			
			(void) AEInstallObjectAccessor< ObjectAccessor >( desiredClass,
			                                                  containerType,
			                                                  42,
			                                                  isSysHandler );
			
			(void) AEInstallObjectAccessor< ObjectAccessor >( desiredClass,
			                                                  containerType,
			                                                  42 );
			
			(void) AEInstallObjectAccessor< ObjectAccessor >( desiredClass,
			                                                  containerType,
			                                                  &desiredClass,
			                                                  isSysHandler );
			
			(void) AEInstallObjectAccessor< ObjectAccessor >( desiredClass,
			                                                  containerType,
			                                                  &install_ProcPtr );
			
			(void) AEInstallObjectAccessor< ObjectAccessor >( desiredClass,
			                                                  containerType,
			                                                  isSysHandler );
			
			(void) AEInstallObjectAccessor< ObjectAccessor >( desiredClass,
			                                                  containerType );
		}
		
		nucleus::owned< Mac::AEDesc_Token >
		//
		N_ObjectAccessor( Mac::AEObjectClass        desiredClass,
		                  const Mac::AEDesc_Token&  containerToken,
		                  Mac::AEObjectClass        containerClass,
		                  Mac::AEKeyForm            keyForm,
		                  const Mac::AEDesc_Data&   keyData );
		
		nucleus::owned< Mac::AEDesc_Token >
		//
		N_ObjectAccessor_RefCon( Mac::AEObjectClass        desiredClass,
		                         const Mac::AEDesc_Token&  containerToken,
		                         Mac::AEObjectClass        containerClass,
		                         Mac::AEKeyForm            keyForm,
		                         const Mac::AEDesc_Data&   keyData,
		                         WindowRef                 refcon );
		
		static void install_N_ProcPtr()
		{
			return;
			
			Mac::AEObjectClass  desiredClass  = Mac::cWindow;
			Mac::DescType       containerType = Mac::typeNull;
			::SRefCon           refCon        = 0;
			bool                isSysHandler  = false;
			
			(void) AEInstallObjectAccessor< WindowRef,
			                                N_ObjectAccessor_RefCon >( desiredClass,
			                                                           containerType,
			                                                           WindowRef(),
			                                                           isSysHandler );
			
			(void) AEInstallObjectAccessor< WindowRef,
			                                N_ObjectAccessor_RefCon >( desiredClass,
			                                                           containerType,
			                                                           WindowRef() );
			
			(void) AEInstallObjectAccessor< N_ObjectAccessor >( desiredClass,
			                                                    containerType,
			                                                    isSysHandler );
			
			(void) AEInstallObjectAccessor< N_ObjectAccessor >( desiredClass,
			                                                    containerType );
		}
		
	}
	
#endif  // #if defined( __GNUC__ ) && ( __GNUC__ * 100 + __GNUC_MINOR__ <= 303 )
	
}
