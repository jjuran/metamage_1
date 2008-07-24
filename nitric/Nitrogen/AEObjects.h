// Nitrogen/AEObjects.h
// --------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_AEOBJECTS_H
#define NITROGEN_AEOBJECTS_H

#ifndef NITROGEN_AEOBJECTCLASS_H
#include "Nitrogen/AEObjectClass.h"
#endif
#ifndef NITROGEN_AEPROPERTYID_H
#include "Nitrogen/AEPropertyID.h"
#endif

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __AEOBJECTS__
#include FRAMEWORK_HEADER(AE,AEObjects.h)
#endif
#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif


// Christopher Nebel personally gave his blessing to these names at WWDC 2006
typedef DescType AEObjectClass, AEPropertyID;

// These I made up
typedef DescType AEAbsoluteOrdinal;

enum
{
	typeObjectClass = 'Clas',
	typePropertyID  = 'Prop'
};


namespace Nitrogen
{
	
	typedef AppleEventManagerErrorsRegistrationDependency ObjectSupportLibraryErrorsRegistrationDependency;
	
	#pragma mark -
	#pragma mark ¥ Constant types ¥
	
	enum AERelativeDescriptor
	{
		kAENext     = ::kAENext,
		kAEPrevious = ::kAEPrevious,
		
		kAERelativeDescriptor_Max = Nucleus::Enumeration_Traits< ::DescType >::max
	};
	
	typedef AERelativeDescriptor AERelativeOrdinal;
	
	enum AEAbsoluteOrdinal
	{
		kAEFirst  = ::kAEFirst,
		kAELast   = ::kAELast,
		kAEMiddle = ::kAEMiddle,
		kAEAny    = ::kAEAny,
		kAEAll    = ::kAEAll,
		
		kAEAbsoluteOrdinal_Max = Nucleus::Enumeration_Traits< ::DescType >::max
	};
	
	enum AELogicalOperator
	{
		kAEAND = ::kAEAND,
		kAEOR  = ::kAEOR,
		kAENOT = ::kAENOT,
		
		kAELogicalOperator_Max = Nucleus::Enumeration_Traits< ::DescType >::max
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
		
		kAEComparisonOperator_Max = Nucleus::Enumeration_Traits< ::DescType >::max
	};
	
	typedef AEComparisonOperator AECompOperator;
	
	
	static const AEKeyForm formAbsolutePosition = AEKeyForm( ::formAbsolutePosition );
	static const AEKeyForm formRelativePosition = AEKeyForm( ::formRelativePosition );
	static const AEKeyForm formTest             = AEKeyForm( ::formTest             );
	static const AEKeyForm formRange            = AEKeyForm( ::formRange            );
	static const AEKeyForm formPropertyID       = AEKeyForm( ::formPropertyID       );
	static const AEKeyForm formName             = AEKeyForm( ::formName             );
	
	
	static const DescType typeObjectClass = DescType( ::typeObjectClass );
	static const DescType typePropertyID  = DescType( ::typePropertyID  );
	
	
	template <> struct DescType_Traits< typeObjectClass > : Nucleus::ConvertingPODFlattener< AEObjectClass, ::AEObjectClass >  {};
	template <> struct DescType_Traits< typePropertyID >  : Nucleus::ConvertingPODFlattener< AEPropertyID,  ::AEPropertyID  >  {};
	
	template <> struct DescType_Traits< typeAbsoluteOrdinal    > : Nucleus::ConvertingPODFlattener< AEAbsoluteOrdinal,    ::DescType    >  {};
	
	template <> struct DescType_Map_Traits< typeObjectClass     > { static const DescType result = typeType; };
	template <> struct DescType_Map_Traits< typePropertyID      > { static const DescType result = typeType; };
	template <> struct DescType_Map_Traits< typeAbsoluteOrdinal > { static const DescType result = typeType; };
	
	template <> struct AEKeyword_Traits< keyAECompOperator    > : Type_AEKeyword_Traits< AEComparisonOperator > {};
	template <> struct AEKeyword_Traits< keyAELogicalOperator > : Type_AEKeyword_Traits< AELogicalOperator    > {};
	
	template <> struct AEKeyword_Traits< keyAEDesiredClass > : Type_AEKeyword_Traits< AEObjectClass > {};
	template <> struct AEKeyword_Traits< keyAEKeyForm      > : Enum_AEKeyword_Traits< AEKeyForm     > {};
	
	enum AEResolveCallbackFlags
	{
		kAEIDoMinimum           = ::kAEIDoMinimum,
		kAEIDoWhose             = ::kAEIDoWhose,
		kAEIDoMarking           = ::kAEIDoMarking,
		kAEPassSubDescs         = ::kAEPassSubDescs,
		kAEResolveNestedLists   = ::kAEResolveNestedLists,
		kAEHandleSimpleRanges   = ::kAEHandleSimpleRanges,
		kAEUseRelativeIterators = ::kAEUseRelativeIterators,
		
		kAEResolveCallbackFlags_Max = Nucleus::Enumeration_Traits< short >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( AEResolveCallbackFlags )
	
	
	#pragma mark -
	#pragma mark ¥ Callbacks ¥
	
	struct OSLAccessorUPP_Details: Basic_UPP_Details< ::OSLAccessorUPP,
	                                                  ::OSLAccessorProcPtr,
	                                                  ::NewOSLAccessorUPP,
	                                                  ::DisposeOSLAccessorUPP,
	                                                  ::InvokeOSLAccessorUPP >
	{};
	
	typedef UPP< OSLAccessorUPP_Details > OSLAccessorUPP;
	
	inline Nucleus::Owned< OSLAccessorUPP > NewOSLAccessorUPP( ::OSLAccessorProcPtr p )
	{
		return NewUPP< OSLAccessorUPP >( p );
	}
	
	inline void DisposeOSLAccessorUPP( Nucleus::Owned< OSLAccessorUPP > )  {}
	
	inline void InvokeOSLAccessorUPP( AEObjectClass        desiredClass,
	                                  const AEDesc_Token&  containerToken,
	                                  AEObjectClass        containerClass,
	                                  AEEnumerated         keyForm,
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
	
	typedef Nucleus::Owned< AEDesc_Token > ( *OSLAccessorProcPtr )( AEObjectClass        desiredClass,
	                                                                const AEDesc_Token&  containerToken,
	                                                                AEObjectClass        containerClass,
	                                                                AEEnumerated         keyForm,
	                                                                const AEDesc_Data&   keyData,
	                                                                RefCon               accessorRefcon );
	
	template < OSLAccessorProcPtr handler >
	struct Adapt_OSLAccessor
	{
		static pascal OSErr ToCallback( ::DescType     desiredClass,
		                                const AEDesc*  containerToken,
		                                ::DescType     containerClass,
		                                ::DescType     keyForm,
		                                const AEDesc*  keyData,
		                                AEDesc*        value,
		                                long           accessorRefcon )
		{
			try
			{
				*value = handler( AEObjectClass( desiredClass ),
				                  static_cast< const AEDesc_Token& >( *containerToken ),
				                  AEObjectClass( containerClass ),
				                  AEKeyForm( keyForm ),
				                  static_cast< const AEDesc_Data& >( *keyData ),
				                  accessorRefcon ).Release();
			}
			catch ( OSStatus err )
			{
				return err;
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
	
	inline Nucleus::Owned< OSLCompareUPP > NewOSLCompareUPP( ::OSLCompareProcPtr p )
	{
		return NewUPP< OSLCompareUPP >( p );
	}
	
	inline void DisposeOSLCompareUPP( Nucleus::Owned< OSLCompareUPP > )  {}
	
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
	#pragma mark ¥ Object Accessors ¥
	
	struct OSLAccessor
	{
		AEObjectClass    desiredClass;
		DescType         containerType;
		OSLAccessorUPP   accessor;
		RefCon           accessorRefCon;
		bool             isSysHandler;
		
		OSLAccessor();
		
		OSLAccessor( AEObjectClass  desiredClass,
		             DescType       containerType,
		             OSLAccessorUPP accessor,
		             RefCon         accessorRefCon,
		             bool           isSysHandler )
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

namespace Nucleus
{
	
	template <> struct Converter< Nitrogen::AERelativeDescriptor, std::string > : public Nitrogen::StringToFourCharCode_Converter< Nitrogen::AERelativeDescriptor > {};
	template <> struct Converter< Nitrogen::AEAbsoluteOrdinal,    std::string > : public Nitrogen::StringToFourCharCode_Converter< Nitrogen::AEAbsoluteOrdinal    > {};
	template <> struct Converter< Nitrogen::AELogicalOperator,    std::string > : public Nitrogen::StringToFourCharCode_Converter< Nitrogen::AELogicalOperator    > {};
	template <> struct Converter< Nitrogen::AEComparisonOperator, std::string > : public Nitrogen::StringToFourCharCode_Converter< Nitrogen::AEComparisonOperator > {};
//	template <> struct Converter< Nitrogen::AEKeyForm,            std::string > : public Nitrogen::StringToFourCharCode_Converter< Nitrogen::AEKeyForm            > {};
	template <> struct Converter< Nitrogen::AEObjectClass,        std::string > : public Nitrogen::StringToFourCharCode_Converter< Nitrogen::AEObjectClass        > {};
	template <> struct Converter< Nitrogen::AEPropertyID,         std::string > : public Nitrogen::StringToFourCharCode_Converter< Nitrogen::AEPropertyID         > {};
	
	template <> struct Converter< std::string, Nitrogen::AERelativeDescriptor > : public Nitrogen::FourCharCodeToString_Converter< Nitrogen::AERelativeDescriptor > {};
	template <> struct Converter< std::string, Nitrogen::AEAbsoluteOrdinal    > : public Nitrogen::FourCharCodeToString_Converter< Nitrogen::AEAbsoluteOrdinal    > {};
	template <> struct Converter< std::string, Nitrogen::AELogicalOperator    > : public Nitrogen::FourCharCodeToString_Converter< Nitrogen::AELogicalOperator    > {};
	template <> struct Converter< std::string, Nitrogen::AEComparisonOperator > : public Nitrogen::FourCharCodeToString_Converter< Nitrogen::AEComparisonOperator > {};
//	template <> struct Converter< std::string, Nitrogen::AEKeyForm            > : public Nitrogen::FourCharCodeToString_Converter< Nitrogen::AEKeyForm            > {};
	template <> struct Converter< std::string, Nitrogen::AEObjectClass        > : public Nitrogen::FourCharCodeToString_Converter< Nitrogen::AEObjectClass        > {};
	template <> struct Converter< std::string, Nitrogen::AEPropertyID         > : public Nitrogen::FourCharCodeToString_Converter< Nitrogen::AEPropertyID         > {};
	
	template <>
	struct Disposer< Nitrogen::OSLAccessor > : public std::unary_function< Nitrogen::OSLAccessor, void >,
	                                           private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( const Nitrogen::OSLAccessor& installation ) const
		{
			HandleDestructionOSStatus
			(
				::AERemoveObjectAccessor( installation.desiredClass,
				                          installation.containerType,
				                          installation.accessor,
				                          installation.isSysHandler )
			);
		}
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
	
	Nucleus::Owned< AEDesc_Token > AEResolve( const AEDesc_ObjectSpecifier&  objectSpecifier,
	                                          AEResolveCallbackFlags         callbackFlags = AEResolveCallbackFlags() );
	
	Nucleus::Owned< OSLAccessor > AEInstallObjectAccessor( const OSLAccessor& toInstall );
	
	inline Nucleus::Owned< OSLAccessor > AEInstallObjectAccessor( AEObjectClass    desiredClass,
	                                                              DescType         containerType,
	                                                              OSLAccessorUPP   accessor,
	                                                              RefCon           accessorRefCon = RefCon(),
	                                                              bool             isSysHandler   = false )
	{
		return AEInstallObjectAccessor( OSLAccessor( desiredClass,
		                                             containerType,
		                                             accessor,
		                                             accessorRefCon,
		                                             isSysHandler ) );
	}
	
	template < typename OSLAccessorUPP::ProcPtr accessor >
	inline Nucleus::Owned< OSLAccessor > AEInstallObjectAccessor( AEObjectClass  desiredClass,
	                                                              DescType       containerType,
	                                                              RefCon         accessorRefCon = RefCon(),
	                                                              bool           isSysHandler   = false )
	{
		return AEInstallObjectAccessor( OSLAccessor( desiredClass,
		                                             containerType,
		                                             StaticUPP< OSLAccessorUPP, accessor >(),
		                                             accessorRefCon,
		                                             isSysHandler ) );
	}
	
	template < OSLAccessorProcPtr accessor >
	inline Nucleus::Owned< OSLAccessor > AEInstallObjectAccessor( AEObjectClass    desiredClass,
	                                                              DescType         containerType,
	                                                              RefCon           accessorRefCon = RefCon(),
	                                                              bool             isSysHandler   = false )
	{
		return AEInstallObjectAccessor< Adapt_OSLAccessor< accessor >::ToCallback >
		(
			desiredClass, 
			containerType, 
			accessorRefCon, 
			isSysHandler
		);
	}
	
	inline void AERemoveObjectAccessor( Nucleus::Owned< OSLAccessor > )  {}
	
	typedef OSLAccessor AEGetObjectAccessor_Result;
	OSLAccessor AEGetObjectAccessor( AEObjectClass    desiredClass,
	                                 DescType         containerType,
	                                 bool             isSysHandler );
	
	Nucleus::Owned< AEDesc_Token > AECallObjectAccessor( AEObjectClass        desiredClass,
	                                                     const AEDesc_Token&  containerToken,
	                                                     AEObjectClass        containerClass,
	                                                     AEEnumerated         keyForm,
	                                                     const AEDesc_Data&   keyData );
	
	#pragma mark -
	#pragma mark ¥ Token routines ¥
	
	inline void AEDisposeToken( Nucleus::Owned< AEDesc_Token > )
	{
	}
	
}

#endif
