// Nitrogen/AEObjects.hh
// ---------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_AEOBJECTS_HH
#define NITROGEN_AEOBJECTS_HH

#ifndef __AEOBJECTS__
#include <AEObjects.h>
#endif

// Annex
#ifndef ANNEX_MACTYPES_H
#include "Annex/MacTypes.h"
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif
#ifndef NUCLEUS_OBJECTPARAMETERTRAITS_HH
#include "nucleus/object_parameter_traits.hh"
#endif

// Nitrogen
#ifndef MAC_APPLEEVENTS_FUNCTIONS_AEDISPOSETOKEN_HH
#include "Mac/AppleEvents/Functions/AEDisposeToken.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEKEYFORM_HH
#include "Mac/AppleEvents/Types/AEKeyForm.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEOBJECTCLASS_HH
#include "Mac/AppleEvents/Types/AEObjectClass.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPE_HH
#include "Mac/AppleEvents/Types/DescType.hh"
#endif

#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif
#ifndef NITROGEN_UPP_HH
#include "Nitrogen/UPP.hh"
#endif


namespace Nitrogen
{
	
	#pragma mark -
	#pragma mark ** Constant types **
	
	enum AEResolveCallbackFlags
	{
		kAEIDoMinimum           = ::kAEIDoMinimum,
		kAEIDoWhose             = ::kAEIDoWhose,
		kAEIDoMarking           = ::kAEIDoMarking,
		kAEPassSubDescs         = ::kAEPassSubDescs,
		kAEResolveNestedLists   = ::kAEResolveNestedLists,
		kAEHandleSimpleRanges   = ::kAEHandleSimpleRanges,
		kAEUseRelativeIterators = ::kAEUseRelativeIterators,
		
		kAEResolveCallbackFlags_Max = nucleus::enumeration_traits< short >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( AEResolveCallbackFlags )
	
	
	#pragma mark -
	#pragma mark ** Callbacks **
	
	struct OSLAccessorUPP_Details: Basic_UPP_Details< ::OSLAccessorUPP,
	                                                  ::OSLAccessorProcPtr,
	                                                  ::NewOSLAccessorUPP,
	                                                  ::DisposeOSLAccessorUPP,
	                                                  ::InvokeOSLAccessorUPP >
	{};
	
	typedef UPP< OSLAccessorUPP_Details > OSLAccessorUPP;
	
	inline nucleus::owned< OSLAccessorUPP > NewOSLAccessorUPP( ::OSLAccessorProcPtr p )
	{
		return NewUPP< OSLAccessorUPP >( p );
	}
	
	inline void DisposeOSLAccessorUPP( nucleus::owned< OSLAccessorUPP > )  {}
	
	inline void InvokeOSLAccessorUPP( Mac::AEObjectClass        desiredClass,
	                                  const Mac::AEDesc_Token&  containerToken,
	                                  Mac::AEObjectClass        containerClass,
	                                  Mac::AEKeyForm            keyForm,
	                                  const Mac::AEDesc_Data&   keyData,
	                                  Mac::AEDesc_Token&        value,
	                                  ::SRefCon                 accessorRefcon,
	                                  OSLAccessorUPP            userUPP )
	{
		ThrowOSStatus( userUPP( desiredClass,
		                        &containerToken,
		                        containerClass,
		                        keyForm,
		                        &keyData,
		                        &value,
		                        accessorRefcon ) );
	}
	
	template < class RefCon >
	struct RefCon_OSLAccessor
	{
		typedef nucleus::owned< Mac::AEDesc_Token > ( *type )( Mac::AEObjectClass        desiredClass,
		                                                       const Mac::AEDesc_Token&  containerToken,
		                                                       Mac::AEObjectClass        containerClass,
		                                                       Mac::AEKeyForm            keyForm,
		                                                       const Mac::AEDesc_Data&   keyData,
		                                                       RefCon                    accessorRefcon );
	};
	
	template <>
	struct RefCon_OSLAccessor< void >
	{
		typedef nucleus::owned< Mac::AEDesc_Token > ( *type )( Mac::AEObjectClass        desiredClass,
		                                                       const Mac::AEDesc_Token&  containerToken,
		                                                       Mac::AEObjectClass        containerClass,
		                                                       Mac::AEKeyForm            keyForm,
		                                                       const Mac::AEDesc_Data&   keyData );
	};
	
	template < class RefCon,
	           typename RefCon_OSLAccessor< RefCon >::type handler >
	struct ObjectAccessor_Callback
	{
		static pascal OSErr Adapter( ::DescType     desiredClass,
		                             const AEDesc*  containerToken,
		                             ::DescType     containerClass,
		                             ::DescType     keyForm,
		                             const AEDesc*  keyData,
		                             AEDesc*        value,
		                             ::SRefCon      accessorRefcon )
		{
			try
			{
				*value = handler( Mac::AEObjectClass( desiredClass ),
				                  static_cast< const Mac::AEDesc_Token& >( *containerToken ),
				                  Mac::AEObjectClass( containerClass ),
				                  Mac::AEKeyForm( keyForm ),
				                  static_cast< const Mac::AEDesc_Data& >( *keyData ),
				                  reinterpret_cast< RefCon >( accessorRefcon ) ).release();
			}
			catch ( ... )
			{
				return ConvertTheExceptionToOSStatus( errAEEventFailed );
			}
			
			return noErr;
		}
	};
	
	template < RefCon_OSLAccessor< void >::type handler >
	struct ObjectAccessor_Callback< void, handler >
	{
		static pascal OSErr Adapter( ::DescType     desiredClass,
		                             const AEDesc*  containerToken,
		                             ::DescType     containerClass,
		                             ::DescType     keyForm,
		                             const AEDesc*  keyData,
		                             AEDesc*        value,
		                             ::SRefCon )
		{
			try
			{
				*value = handler( Mac::AEObjectClass( desiredClass ),
				                  static_cast< const Mac::AEDesc_Token& >( *containerToken ),
				                  Mac::AEObjectClass( containerClass ),
				                  Mac::AEKeyForm( keyForm ),
				                  static_cast< const Mac::AEDesc_Data& >( *keyData ) ).release();
			}
			catch ( ... )
			{
				return ConvertTheExceptionToOSStatus( errAEEventFailed );
			}
			
			return noErr;
		}
	};
	
#if 0
	
	struct OSLCompareUPP_Details: Basic_UPP_Details< ::OSLCompareUPP,
	                                                 ::OSLCompareProcPtr,
	                                                 ::NewOSLCompareUPP,
	                                                 ::DisposeOSLCompareUPP,
	                                                 ::InvokeOSLCompareUPP >
	{};
	
	typedef UPP< OSLCompareUPP_Details > OSLCompareUPP;
	
	inline nucleus::owned< OSLCompareUPP > NewOSLCompareUPP( ::OSLCompareProcPtr p )
	{
		return NewUPP< OSLCompareUPP >( p );
	}
	
	inline void DisposeOSLCompareUPP( nucleus::owned< OSLCompareUPP > )  {}
	
	inline void InvokeOSLCompareUPP( AECompOperator            oper,
	                                 const Mac::AEDesc_Token&  obj1,
	                                 const Mac::AEDesc_Token&  obj2,
	                                 ::Boolean&                result,
	                                 OSLCompareUPP             userUPP )
	{
		ThrowOSStatus( userUPP( oper, &obj1, &obj2, &result ) );
	}
	
	typedef bool ( *OSLCompareProcPtr )( AECompOperator            oper,
	                                     const Mac::AEDesc_Token&  obj1,
	                                     const Mac::AEDesc_Token&  obj2 );
	
#endif
	
	#pragma mark -
	#pragma mark ** Object Accessors **
	
	struct OSLAccessor
	{
		Mac::AEObjectClass  desiredClass;
		DescType            containerType;
		OSLAccessorUPP      accessor;
		::SRefCon           accessorRefCon;
		bool                isSysHandler;
		
		OSLAccessor();
		
		OSLAccessor( Mac::AEObjectClass  desiredClass,
		             DescType            containerType,
		             OSLAccessorUPP      accessor,
		             ::SRefCon           accessorRefCon,
		             bool                isSysHandler )
		: 
			desiredClass  ( desiredClass   ),
			containerType ( containerType  ),
			accessor      ( accessor       ),
			accessorRefCon( accessorRefCon ),
			isSysHandler  ( isSysHandler   )
		{}
		
	};
	
	bool operator==( const OSLAccessor& a, const OSLAccessor& b );
	
	inline bool operator!=( const OSLAccessor& a, const OSLAccessor& b )
	{
		return !( a == b );
	}
	
}

namespace nucleus
{
	
	template <>
	struct disposer< Nitrogen::OSLAccessor >
	{
		typedef Nitrogen::OSLAccessor  argument_type;
		typedef void                   result_type;
		
		void operator()( const Nitrogen::OSLAccessor& installation ) const
		{
			(void) ::AERemoveObjectAccessor( installation.desiredClass,
			                                 installation.containerType,
			                                 installation.accessor,
			                                 installation.isSysHandler );
		}
	};
	
	template < class Disposer >
	struct aliveness_traits< ::Nitrogen::OSLAccessor, Disposer >
	{
		struct aliveness_test
		{
			static bool is_live( const ::Nitrogen::OSLAccessor& osl )
			{
				return osl.accessor != 0;
			}
		};
	};
	
}

namespace Nitrogen
{
	
	void AEObjectInit();
	
	void AESetObjectCallbacks( OSLCompareUPP        myCompareProc,
	                           OSLCountUPP          myCountProc,
	                           OSLDisposeTokenUPP   myDisposeTokenProc,
	                           OSLGetMarkTokenUPP   myGetMarkTokenProc,
	                           OSLMarkUPP           myMarkProc,
	                           OSLAdjustMarksUPP    myAdjustMarksProc,
	                           OSLGetErrDescUPP     myGetErrDescProcPtr );
	
#if TARGET_RT_MAC_CFM
	
	void AESetObjectCallbacks( ::OSLCompareProcPtr       compareProc,
	                           ::OSLCountProcPtr         countProc,
	                           ::OSLDisposeTokenProcPtr  disposeTokenProc,
	                           ::OSLGetMarkTokenProcPtr  getMarkTokenProc,
	                           ::OSLMarkProcPtr          markProc,
	                           ::OSLAdjustMarksProcPtr   adjustMarksProc,
	                           ::OSLGetErrDescProcPtr    getErrDescProc );
	
#endif
	
	nucleus::owned< Mac::AEDesc_Token >
	//
	AEResolve( const Mac::AEDesc_ObjectSpecifier&  objectSpecifier,
	           AEResolveCallbackFlags              callbackFlags = AEResolveCallbackFlags() );
	
	// Level 0
	
	nucleus::owned< OSLAccessor > AEInstallObjectAccessor( const OSLAccessor& toInstall );
	
	inline nucleus::owned< OSLAccessor >
	//
	AEInstallObjectAccessor( Mac::AEObjectClass  desiredClass,
	                         Mac::DescType       containerType,
	                         OSLAccessorUPP      accessor,
	                         long                accessorRefCon = 0,
	                         bool                isSysHandler   = false )
	{
		return AEInstallObjectAccessor( OSLAccessor( desiredClass,
		                                             containerType,
		                                             accessor,
		                                             (::SRefCon) accessorRefCon,
		                                             isSysHandler ) );
	}
	
	inline nucleus::owned< OSLAccessor >
	//
	AEInstallObjectAccessor( Mac::AEObjectClass  desiredClass,
	                         Mac::DescType       containerType,
	                         OSLAccessorUPP      accessor,
	                         void*               accessorRefCon,
	                         bool                isSysHandler   = false )
	{
		return AEInstallObjectAccessor( OSLAccessor( desiredClass,
		                                             containerType,
		                                             accessor,
		                                             (::SRefCon) accessorRefCon,
		                                             isSysHandler ) );
	}
	
	// Level 1
	
	template < typename OSLAccessorUPP::ProcPtr accessor, class RefCon >
	inline nucleus::owned< OSLAccessor >
	//
	AEInstallObjectAccessor( Mac::AEObjectClass  desiredClass,
	                         Mac::DescType       containerType,
	                         RefCon              accessorRefCon,
	                         bool                isSysHandler   = false )
	{
		return AEInstallObjectAccessor( OSLAccessor( desiredClass,
		                                             containerType,
		                                             StaticUPP< OSLAccessorUPP, accessor >(),
		                                             (::SRefCon) accessorRefCon,
		                                             isSysHandler ) );
	}
	
	template < typename OSLAccessorUPP::ProcPtr accessor >
	inline nucleus::owned< OSLAccessor >
	//
	AEInstallObjectAccessor( Mac::AEObjectClass  desiredClass,
	                         Mac::DescType       containerType,
	                         bool                isSysHandler   = false )
	{
		return AEInstallObjectAccessor( OSLAccessor( desiredClass,
		                                             containerType,
		                                             StaticUPP< OSLAccessorUPP, accessor >(),
		                                             0,
		                                             isSysHandler ) );
	}
	
	// Level 2
	
	template < class Object,
	           typename RefCon_OSLAccessor< Object >::type accessor >
	inline nucleus::owned< OSLAccessor >
	//
	AEInstallObjectAccessor( Mac::AEObjectClass                                         desiredClass,
	                         Mac::DescType                                              containerType,
	                         typename nucleus::object_parameter_traits< Object >::type  accessorRefCon = typename nucleus::object_parameter_traits< Object >::type(),
	                         bool                                                       isSysHandler   = false )
	{
		return AEInstallObjectAccessor< ObjectAccessor_Callback< Object, accessor >::Adapter >
		(
			desiredClass,
			containerType,
			nucleus::object_parameter_traits< Object >::convert_to_pointer( accessorRefCon ),
			isSysHandler
		);
	}
	
	template < class Object,
	           typename RefCon_OSLAccessor< Object >::type accessor >
	inline nucleus::owned< OSLAccessor >
	//
	AEInstallObjectAccessor( Mac::AEObjectClass  desiredClass,
	                         Mac::DescType       containerType,
	                         bool                isSysHandler )
	{
		typedef typename nucleus::object_parameter_traits< Object >::type object_type;
		
		return AEInstallObjectAccessor< ObjectAccessor_Callback< void, accessor >::Adapter >
		(
			desiredClass,
			containerType,
			nucleus::object_parameter_traits< Object >::convert_to_pointer( object_type() ),
			isSysHandler
		);
	}
	
	template < RefCon_OSLAccessor< void >::type accessor >
	inline nucleus::owned< OSLAccessor >
	//
	AEInstallObjectAccessor( Mac::AEObjectClass  desiredClass,
	                         Mac::DescType       containerType,
	                         bool                isSysHandler   = false )
	{
		return AEInstallObjectAccessor< void, accessor >( desiredClass,
		                                                  containerType,
		                                                  isSysHandler );
	}
	
	inline void AERemoveObjectAccessor( nucleus::owned< OSLAccessor > )  {}
	
	typedef OSLAccessor AEGetObjectAccessor_Result;
	OSLAccessor AEGetObjectAccessor( Mac::AEObjectClass  desiredClass,
	                                 Mac::DescType       containerType,
	                                 bool                isSysHandler );
	
	nucleus::owned< Mac::AEDesc_Token >
	//
	AECallObjectAccessor( Mac::AEObjectClass        desiredClass,
	                      const Mac::AEDesc_Token&  containerToken,
	                      Mac::AEObjectClass        containerClass,
	                      Mac::AEKeyForm            keyForm,
	                      const Mac::AEDesc_Data&   keyData );
	
}

#endif
