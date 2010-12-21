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

// Nitrogen
#ifndef MAC_APPLEEVENTS_TYPES_AEKEYFORM_HH
#include "Mac/AppleEvents/Types/AEKeyForm.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEOBJECTCLASS_HH
#include "Mac/AppleEvents/Types/AEObjectClass.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEPROPERTYID_HH
#include "Mac/AppleEvents/Types/AEPropertyID.hh"
#endif

#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif


namespace Nitrogen
{
	
	#pragma mark -
	#pragma mark ** Constant types **
	
	enum AELogicalOperator
	{
		kAEAND = ::kAEAND,
		kAEOR  = ::kAEOR,
		kAENOT = ::kAENOT,
		
		kAELogicalOperator_Max = nucleus::enumeration_traits< ::DescType >::max
	};
	
	//typedef AELogicalOperator AELogicalDescriptor;
	
	enum AEComparisonOperator
	{
		kAEBeginsWith        = ::kAEBeginsWith,
		kAECaseSensEquals    = ::kAECaseSensEquals,
		kAEContains          = ::kAEContains,
		kAEEndsWith          = ::kAEEndsWith,
		kAEEquals            = ::kAEEquals,
		kAEGreaterThan       = ::kAEGreaterThan,
		kAEGreaterThanEquals = ::kAEGreaterThanEquals,
		kAELessThan          = ::kAELessThan,
		kAELessThanEquals    = ::kAELessThanEquals,
		
		kAEComparisonOperator_Max = nucleus::enumeration_traits< ::DescType >::max
	};
	
	typedef AEComparisonOperator AECompOperator;
	
	
	template <> struct AEKeyword_Traits< Mac::keyAECompOperator    > : Type_AEKeyword_Traits< AEComparisonOperator > {};
	template <> struct AEKeyword_Traits< Mac::keyAELogicalOperator > : Type_AEKeyword_Traits< AELogicalOperator    > {};
	
	template <> struct AEKeyword_Traits< Mac::keyAEDesiredClass > : Type_AEKeyword_Traits< Mac::AEObjectClass > {};
	template <> struct AEKeyword_Traits< Mac::keyAEKeyForm      > : Enum_AEKeyword_Traits< Mac::AEKeyForm     > {};
	
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
	
	inline void InvokeOSLAccessorUPP( Mac::AEObjectClass   desiredClass,
	                                  const AEDesc_Token&  containerToken,
	                                  Mac::AEObjectClass   containerClass,
	                                  Mac::AEKeyForm       keyForm,
	                                  const AEDesc_Data&   keyData,
	                                  AEDesc_Token&        value,
	                                  RefCon               accessorRefcon,
	                                  OSLAccessorUPP       userUPP )
	{
		ThrowOSStatus( userUPP( desiredClass,
		                        &containerToken,
		                        containerClass,
		                        keyForm,
		                        &keyData,
		                        &value,
		                        accessorRefcon ) );
	}
	
	typedef nucleus::owned< AEDesc_Token > ( *OSLAccessorProcPtr )( Mac::AEObjectClass   desiredClass,
	                                                                const AEDesc_Token&  containerToken,
	                                                                Mac::AEObjectClass   containerClass,
	                                                                Mac::AEKeyForm       keyForm,
	                                                                const AEDesc_Data&   keyData,
	                                                                RefCon               accessorRefcon );
	
	template < OSLAccessorProcPtr handler >
	struct ObjectAccessor_Callback
	{
		static pascal OSErr Adapter( ::DescType     desiredClass,
		                             const AEDesc*  containerToken,
		                             ::DescType     containerClass,
		                             ::DescType     keyForm,
		                             const AEDesc*  keyData,
		                             AEDesc*        value,
		                             long           accessorRefcon )
		{
			try
			{
				*value = handler( Mac::AEObjectClass( desiredClass ),
				                  static_cast< const AEDesc_Token& >( *containerToken ),
				                  Mac::AEObjectClass( containerClass ),
				                  Mac::AEKeyForm( keyForm ),
				                  static_cast< const AEDesc_Data& >( *keyData ),
				                  accessorRefcon ).release();
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
	
	inline void InvokeOSLCompareUPP( AECompOperator       oper,
	                                 const AEDesc_Token&  obj1,
	                                 const AEDesc_Token&  obj2,
	                                 ::Boolean&           result,
	                                 OSLCompareUPP        userUPP )
	{
		ThrowOSStatus( userUPP( oper, &obj1, &obj2, &result ) );
	}
	
	typedef bool ( *OSLCompareProcPtr )( AECompOperator       oper,
	                                     const AEDesc_Token&  obj1,
	                                     const AEDesc_Token&  obj2 );
	
#endif
	
	#pragma mark -
	#pragma mark ** Object Accessors **
	
	struct OSLAccessor
	{
		Mac::AEObjectClass  desiredClass;
		DescType            containerType;
		OSLAccessorUPP      accessor;
		RefCon              accessorRefCon;
		bool                isSysHandler;
		
		OSLAccessor();
		
		OSLAccessor( Mac::AEObjectClass  desiredClass,
		             DescType            containerType,
		             OSLAccessorUPP      accessor,
		             RefCon              accessorRefCon,
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
			::Nitrogen::HandleDestructionOSStatus
			(
				::AERemoveObjectAccessor( installation.desiredClass,
				                          installation.containerType,
				                          installation.accessor,
				                          installation.isSysHandler )
			);
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
	
	nucleus::owned< AEDesc_Token > AEResolve( const AEDesc_ObjectSpecifier&  objectSpecifier,
	                                          AEResolveCallbackFlags         callbackFlags = AEResolveCallbackFlags() );
	
	nucleus::owned< OSLAccessor > AEInstallObjectAccessor( const OSLAccessor& toInstall );
	
	inline nucleus::owned< OSLAccessor >
	//
	AEInstallObjectAccessor( Mac::AEObjectClass  desiredClass,
	                         DescType            containerType,
	                         OSLAccessorUPP      accessor,
	                         RefCon              accessorRefCon = RefCon(),
	                         bool                isSysHandler   = false )
	{
		return AEInstallObjectAccessor( OSLAccessor( desiredClass,
		                                             containerType,
		                                             accessor,
		                                             accessorRefCon,
		                                             isSysHandler ) );
	}
	
	template < typename OSLAccessorUPP::ProcPtr accessor >
	inline nucleus::owned< OSLAccessor >
	//
	AEInstallObjectAccessor( Mac::AEObjectClass  desiredClass,
	                         DescType            containerType,
	                         RefCon              accessorRefCon = RefCon(),
	                         bool                isSysHandler   = false )
	{
		return AEInstallObjectAccessor( OSLAccessor( desiredClass,
		                                             containerType,
		                                             StaticUPP< OSLAccessorUPP, accessor >(),
		                                             accessorRefCon,
		                                             isSysHandler ) );
	}
	
	template < OSLAccessorProcPtr accessor >
	inline nucleus::owned< OSLAccessor >
	//
	AEInstallObjectAccessor( Mac::AEObjectClass  desiredClass,
	                         DescType            containerType,
	                         RefCon              accessorRefCon = RefCon(),
	                         bool                isSysHandler   = false )
	{
		return AEInstallObjectAccessor< ObjectAccessor_Callback< accessor >::Adapter >
		(
			desiredClass, 
			containerType, 
			accessorRefCon, 
			isSysHandler
		);
	}
	
	inline void AERemoveObjectAccessor( nucleus::owned< OSLAccessor > )  {}
	
	typedef OSLAccessor AEGetObjectAccessor_Result;
	OSLAccessor AEGetObjectAccessor( Mac::AEObjectClass  desiredClass,
	                                 DescType            containerType,
	                                 bool                isSysHandler );
	
	nucleus::owned< AEDesc_Token > AECallObjectAccessor( Mac::AEObjectClass   desiredClass,
	                                                     const AEDesc_Token&  containerToken,
	                                                     Mac::AEObjectClass   containerClass,
	                                                     Mac::AEKeyForm       keyForm,
	                                                     const AEDesc_Data&   keyData );
	
}

#endif
