// AEObjects.h

#ifndef NITROGEN_AEOBJECTS_H
#define NITROGEN_AEOBJECTS_H

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

typedef DescType AELogicalDescriptor, AEAbsoluteOrdinal, AERelativeDescriptor;

// These I made up
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
	
	enum AELogicalDescriptor
	{
		kAEAND = ::kAEAND,
		kAEOR  = ::kAEOR,
		kAENOT = ::kAENOT,
		
		kAELogicalDescriptor_Max = Nucleus::Enumeration_Traits< ::DescType >::max
	};
	
	typedef AELogicalDescriptor AELogicalOperator;
	
	enum AEAbsoluteOrdinal
	{
		kAEFirst  = ::kAEFirst,
		kAELast   = ::kAELast,
		kAEMiddle = ::kAEMiddle,
		kAEAny    = ::kAEAny,
		kAEAll    = ::kAEAll,
		
		kAEAbsoluteOrdinal_Max = Nucleus::Enumeration_Traits< ::DescType >::max
	};
	
	enum AERelativeDescriptor
	{
		kAENext     = ::kAENext,
		kAEPrevious = ::kAEPrevious,
		
		kAERelativeDescriptor_Max = Nucleus::Enumeration_Traits< ::DescType >::max
	};
	
	typedef AERelativeDescriptor AERelativeOrdinal;
	
	
	static const AEKeyForm formAbsolutePosition = AEKeyForm( ::formAbsolutePosition );
	static const AEKeyForm formRelativePosition = AEKeyForm( ::formRelativePosition );
	static const AEKeyForm formTest             = AEKeyForm( ::formTest             );
	static const AEKeyForm formRange            = AEKeyForm( ::formRange            );
	static const AEKeyForm formPropertyID       = AEKeyForm( ::formPropertyID       );
	static const AEKeyForm formName             = AEKeyForm( ::formName             );
	
	
	static const DescType typeObjectClass = DescType( ::typeObjectClass );
	static const DescType typePropertyID  = DescType( ::typePropertyID  );
	
	
	typedef Nucleus::Selector< class AEObjectClass_Tag, ::DescType >::Type AEObjectClass;
	typedef Nucleus::Selector< class AEPropertyID_Tag,  ::DescType >::Type AEPropertyID;
	
	template <> struct DescType_Traits< typeObjectClass > : Nucleus::ConvertingPODFlattener< AEObjectClass, ::AEObjectClass >  {};
	template <> struct DescType_Traits< typePropertyID >  : Nucleus::ConvertingPODFlattener< AEPropertyID,  ::AEPropertyID  >  {};
	
	template <> struct DescType_Traits< typeLogicalDescriptor  > : Nucleus::ConvertingPODFlattener< AELogicalDescriptor,  ::AELogicalDescriptor  >  {};
	template <> struct DescType_Traits< typeAbsoluteOrdinal    > : Nucleus::ConvertingPODFlattener< AEAbsoluteOrdinal,    ::AEAbsoluteOrdinal    >  {};
	template <> struct DescType_Traits< typeRelativeDescriptor > : Nucleus::ConvertingPODFlattener< AERelativeDescriptor, ::AERelativeDescriptor >  {};
	
	template <> struct DescType_Map_Traits< typeObjectClass        > { static const DescType result = typeType; };
	template <> struct DescType_Map_Traits< typePropertyID         > { static const DescType result = typeType; };
	template <> struct DescType_Map_Traits< typeLogicalDescriptor  > { static const DescType result = typeType; };
	template <> struct DescType_Map_Traits< typeAbsoluteOrdinal    > { static const DescType result = typeType; };
	template <> struct DescType_Map_Traits< typeRelativeDescriptor > { static const DescType result = typeType; };
	
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
	#pragma mark ¥ AEToken ¥
	
	namespace Detail
	{
		
		struct AETokenDisposer : public std::unary_function< AEToken, void >
		{
			// parameter can't be const
			void operator()( AEToken token ) const
			{
				::AEDisposeToken( &token );
			}
		};
		
	}
	
	using Detail::AETokenDisposer;
	
}

namespace Nucleus
{
	template <>
	struct LivelinessTraits< Nitrogen::AEToken, Nitrogen::Detail::AETokenDisposer >
	{
		typedef SeizedValuesAreLive LivelinessTest;
	};
}

namespace Nitrogen
{
	
	namespace Detail
	{
		
		class AETokenEditor
		{
			private:
				Nucleus::Owned< AEToken, AETokenDisposer >& token;
				AEToken workingCopy;
			
			public:
				AETokenEditor( Nucleus::Owned< AEToken, AETokenDisposer >& token )
				:
					token      ( token           ),
					workingCopy( token.Release() )
				{}
				
				~AETokenEditor()
				{
					token = Nucleus::Owned< AEToken, AETokenDisposer >::Seize( workingCopy );
				}
				
				AEToken& Get()       { return workingCopy; }
				operator AEToken&()  { return Get();       }
		};
		
	}
	
	typedef AEDescList AETokenList;
	typedef DescType AECompOperator;
	
	
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
	
	inline void InvokeOSLAccessorUPP( AEObjectClass   desiredClass,
	                                  const AEToken&  containerToken,
	                                  AEObjectClass   containerClass,
	                                  AEEnumerated    keyForm,
	                                  const AEDesc&   keyData,
	                                  AEToken&        value,
	                                  RefCon          accessorRefcon,
	                                  OSLAccessorUPP  userUPP )
	{
		ThrowOSStatus( userUPP( desiredClass,
		                        &containerToken,
		                        containerClass,
		                        keyForm,
		                        &keyData,
		                        &value,
		                        accessorRefcon ) );
	}
	
	typedef Nucleus::Owned< AEToken, AETokenDisposer > ( *OSLAccessorProcPtr )( AEObjectClass   desiredClass,
	                                                                            const AEToken&  containerToken,
	                                                                            AEObjectClass   containerClass,
	                                                                            AEEnumerated    keyForm,
	                                                                            const AEDesc&   keyData,
	                                                                            RefCon          accessorRefcon );
	
	template < OSLAccessorProcPtr handler >
	struct Adapt_OSLAccessor
	{
		static pascal OSErr ToCallback( ::DescType      desiredClass,
		                                const AEToken*  containerToken,
		                                ::DescType      containerClass,
		                                ::DescType      keyForm,
		                                const AEDesc*   keyData,
		                                AEToken*         value,
		                                long            accessorRefcon )
		{
			try
			{
				*value = handler( AEObjectClass( desiredClass ),
				                  *containerToken,
				                  AEObjectClass( containerClass ),
				                  AEKeyForm( keyForm ),
				                  *keyData,
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
	
	inline void InvokeOSLCompareUPP( AECompOperator  oper,
	                                 const AEToken&  obj1,
	                                 const AEToken&  obj2,
	                                 ::Boolean&      result,
	                                 OSLCompareUPP   userUPP )
	{
		ThrowOSStatus( userUPP( oper, &obj1, &obj2, &result ) );
	}
	
	typedef bool ( *OSLCompareProcPtr )( AECompOperator  oper,
	                                     const AEToken&  obj1,
	                                     const AEToken&  obj2 );
	
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
	
	Nucleus::Owned< AEToken, AETokenDisposer > AEResolve( const AEObjectSpecifier&  objectSpecifier,
	                                                      AEResolveCallbackFlags    callbackFlags = AEResolveCallbackFlags() );
	
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
	Nucleus::Owned< OSLAccessor > AEInstallObjectAccessor( AEObjectClass  desiredClass,
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
	Nucleus::Owned< OSLAccessor > AEInstallObjectAccessor( AEObjectClass    desiredClass,
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
	
	Nucleus::Owned< AEToken, AETokenDisposer > AECallObjectAccessor( AEObjectClass   desiredClass,
	                                                                 const AEToken&  containerToken,
	                                                                 AEObjectClass   containerClass,
	                                                                 AEEnumerated    keyForm,
	                                                                 const AEDesc&   keyData );
	
	#pragma mark -
	#pragma mark ¥ AEToken routines ¥
	
	inline Nucleus::Owned< AEToken, AETokenDisposer > AEInitializeToken()
	{
		return Nucleus::Owned< AEToken, AETokenDisposer >::Seize( Nucleus::Make< AEToken >() );
	}
	
	inline Nucleus::Owned< AEToken, AETokenDisposer > AECreateToken( DescType     typeCode,
	                                                                 const void*  dataPtr,
	                                                                 Size         dataSize )
	{
		return Nucleus::Owned< AEToken, AETokenDisposer >::Seize( AECreateDesc( typeCode,
		                                                                        dataPtr,
		                                                                        dataSize ).Release() );
	}
	
	inline Nucleus::Owned< AEToken, AETokenDisposer > AECreateToken( DescType  typeCode,
	                                                                 Handle    handle )
	{
		return Nucleus::Owned< AEToken, AETokenDisposer >::Seize( AECreateDesc( typeCode,
		                                                                        handle ).Release() );
	}
	
	inline Nucleus::Owned< AEToken, AETokenDisposer > AECreateToken( DescType                  typeCode,
	                                                                 Nucleus::Owned< Handle >  handle )
	{
		return Nucleus::Owned< AEToken, AETokenDisposer >::Seize( AECreateDesc( typeCode,
		                                                                        handle ).Release() );
	}
	
	template < class Data >
	Nucleus::Owned< AEToken, AETokenDisposer > AECreateToken( DescType  typeCode,
	                                                          Data**    handle )
	{
		return AECreateToken( typeCode, Handle( handle ) );
	}
	
	template < class T >
	Nucleus::Owned< AEToken, AETokenDisposer > AECreateToken( DescType typeCode,
	                                                          Nucleus::Owned< T**, Nucleus::Disposer< Handle > > handle )
	{
		return AECreateToken( typeCode, Nucleus::Owned< Handle >( handle ) );
	}
	
	Nucleus::Owned< AEToken, AETokenDisposer > AECreateToken( DescType typeCode,
	                                                          Nucleus::Owned< AEToken, AETokenDisposer > token );
	
	template < DescType type >
	Nucleus::Owned< AEToken, AETokenDisposer > AECreateToken( typename DescType_Traits< type >::Parameter data )
	{
		return Nucleus::Owned< AEToken, AETokenDisposer >::Seize( AECreateDesc< type >( data ).Release() );
	}
	
	inline Nucleus::Owned< AEToken, AETokenDisposer > AEDuplicateToken( const AEToken& token )
	{
		return Nucleus::Owned< AEToken, AETokenDisposer >::Seize( AEDuplicateDesc( token ).Release() );
	}
	
	inline Nucleus::Owned< AEToken, AETokenDisposer > AECreateTokenList( bool isRecord = false )
	{
		return Nucleus::Owned< AEToken, AETokenDisposer >::Seize( AECreateList( isRecord ).Release() );
	}
	
	template < bool isRecord >
	Nucleus::Owned< AEToken, AETokenDisposer > AECreateTokenList()
	{
		return AECreateTokenList( isRecord );
	}
	
	inline void AEPutPtr( Nucleus::Owned< AETokenList, AETokenDisposer >&  tokenList,
	                      long                                             index,
	                      DescType                                         typeCode,
	                      const void*                                      dataPtr,
	                      Size                                             dataSize )
	{
		AEPutPtr( Detail::AETokenEditor( tokenList ),
		          index,
		          typeCode,
		          dataPtr,
		          dataSize );
	}
	
	inline void AEPutDesc( Nucleus::Owned< AETokenList, AETokenDisposer >&  tokenList,
	                       long                                             index,
	                       const AEToken&                                   token )
	{
		AEPutDesc( Detail::AETokenEditor( tokenList ), index, token );
	}
	
	inline Nucleus::Owned< AETokenList, AETokenDisposer > AEGetNthToken( const AETokenList&  tokenList,
	                                                                     long                index )
	{
		return Nucleus::Owned< AEToken, AETokenDisposer >::Seize( AEGetNthDesc( tokenList, index ).Release() );
	}
	
	inline void AEPutKeyPtr( Nucleus::Owned< AERecord, AETokenDisposer >&  record,
	                         AEKeyword                                     keyword,
	                         DescType                                      typeCode,
	                         const void*                                   dataPtr,
	                         std::size_t                                   dataSize )
	{
		AEPutKeyPtr( Detail::AETokenEditor( record ),
		             keyword,
		             typeCode,
		             dataPtr,
		             dataSize );
	}
	
	inline void AEPutKeyDesc( Nucleus::Owned< AERecord, AETokenDisposer >&  record,
	                          AEKeyword                                     keyword,
	                          const AEDesc&                                 desc )
	{
		AEPutKeyDesc( Detail::AETokenEditor( record ), keyword, desc );
	}
	
	inline void AEPutKeyDesc( Nucleus::Owned< AERecord, AETokenDisposer >&  record,
	                          const AEKeyDesc&                              keyDesc )
	{
		AEPutKeyDesc( Detail::AETokenEditor( record ), keyDesc );
	}
	
	inline void AEDisposeToken( Nucleus::Owned< AEToken, AETokenDisposer > )  {}
	
}

#endif
