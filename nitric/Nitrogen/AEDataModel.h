// AEDataModel.h

#ifndef NITROGEN_AEDATAMODEL_H
#define NITROGEN_AEDATAMODEL_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __AEDATAMODEL__
#include FRAMEWORK_HEADER(AE,AEDataModel.h)
#endif
#ifndef __EVENTS__
#include FRAMEWORK_HEADER(HIToolbox,Events.h)
#endif

#if !TARGET_API_MAC_CARBON
#include "CarbonUnits/AEDataModel.hh"
#endif

#ifndef NUCLEUS_FLAG_H
#include "Nucleus/Flag.h"
#endif
#ifndef NUCLEUS_ID_H
#include "Nucleus/ID.h"
#endif
#ifndef NUCLEUS_NASSERT_H
#include "Nucleus/NAssert.h"
#endif
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif
#ifndef NUCLEUS_SELECTOR_H
#include "Nucleus/Selector.h"
#endif
#ifndef NUCLEUS_SELECTORTYPE_H
#include "Nucleus/SelectorType.h"
#endif

#ifndef NITROGEN_ALIASES_H
#include "Nitrogen/Aliases.h"
#endif
#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_PROCESSES_H
#include "Nitrogen/Processes.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_REFCON_H
#include "Nitrogen/RefCon.h"
#endif
#ifndef NITROGEN_UPP_H
#include "Nitrogen/UPP.h"
#endif

#include <string>


#ifdef AEPutKeyPtr
#undef AEPutKeyPtr

inline OSErr AEPutKeyPtr( AppleEvent*  theAppleEvent,
                          AEKeyword    theAEKeyword,
                          DescType     typeCode,
                          const void*  dataPtr,
                          Size         dataSize )
{
	return AEPutParamPtr( theAppleEvent,
	                      theAEKeyword,
	                      typeCode,
	                      dataPtr,
	                      dataSize );
}

#endif

#ifdef AEPutKeyDesc
#undef AEPutKeyDesc

inline OSErr AEPutKeyDesc( AppleEvent*    theAppleEvent,
                           AEKeyword      theAEKeyword,
                           const AEDesc*  theAEDesc )
{
	return AEPutParamDesc( theAppleEvent, theAEKeyword, theAEDesc );
}

#endif

#ifdef AEGetKeyPtr
#undef AEGetKeyPtr

inline OSErr AEGetKeyPtr( const AppleEvent*  theAppleEvent,
                          AEKeyword          theAEKeyword,
                          DescType           desiredType,
                          DescType*          typeCode,
                          void*              dataPtr,
                          Size               maximumSize,
                          Size*              actualSize )
{
	return AEGetParamPtr( theAppleEvent,
	                      theAEKeyword,
	                      desiredType,
	                      typeCode,
	                      dataPtr,
	                      maximumSize,
	                      actualSize );
}

#endif

#ifdef AEGetKeyDesc
#undef AEGetKeyDesc

inline OSErr AEGetKeyDesc( const AppleEvent*  theAppleEvent,
                           AEKeyword          theAEKeyword,
                           DescType           desiredType,
                           AEDesc*            result )
{
	return AEGetParamDesc( theAppleEvent,
	                       theAEKeyword,
	                       desiredType,
	                       result );
}

#endif

#ifdef AESizeOfKeyDesc
#undef AESizeOfKeyDesc

inline OSErr AESizeOfKeyDesc( const AppleEvent*  theAppleEvent,
                              AEKeyword          theAEKeyword,
                              DescType*          typeCode,
                              Size*              dataSize )
{
	return AESizeOfParam( theAppleEvent,
	                      theAEKeyword,
	                      typeCode,
	                      dataSize );
}

#endif

#ifdef AEDeleteKeyDesc
#undef AEDeleteKeyDesc

inline OSErr AEDeleteKeyDesc( AppleEvent*  theAppleEvent,
                              AEKeyword    theAEKeyword )
{
	return AEDeleteParam( theAppleEvent, theAEKeyword );
}

#endif


namespace Nitrogen
{
	
	class AppleEventManagerErrorsRegistrationDependency
	{
		public:
			AppleEventManagerErrorsRegistrationDependency();
	};
	
	#pragma mark -
	#pragma mark ¥ DescType ¥
	
	class DescType_Tag {};
	typedef Nucleus::SelectorType< DescType_Tag, ::DescType > DescType;
	
	typedef Nucleus::Selector< class AEKeyword_Tag, ::AEKeyword >::Type AEKeyword;
	
	static const AEKeyword keyTransactionIDAttr   = AEKeyword( ::keyTransactionIDAttr   );
	static const AEKeyword keyReturnIDAttr        = AEKeyword( ::keyReturnIDAttr        );
	static const AEKeyword keyEventClassAttr      = AEKeyword( ::keyEventClassAttr      );
	static const AEKeyword keyEventIDAttr         = AEKeyword( ::keyEventIDAttr         );
	static const AEKeyword keyAddressAttr         = AEKeyword( ::keyAddressAttr         );
	static const AEKeyword keyOptionalKeywordAttr = AEKeyword( ::keyOptionalKeywordAttr );
	static const AEKeyword keyTimeoutAttr         = AEKeyword( ::keyTimeoutAttr         );
	static const AEKeyword keyInteractLevelAttr   = AEKeyword( ::keyInteractLevelAttr   );
	static const AEKeyword keyEventSourceAttr     = AEKeyword( ::keyEventSourceAttr     );
	static const AEKeyword keyMissedKeywordAttr   = AEKeyword( ::keyMissedKeywordAttr   );
	static const AEKeyword keyOriginalAddressAttr = AEKeyword( ::keyOriginalAddressAttr );
	static const AEKeyword keyAcceptTimeoutAttr   = AEKeyword( ::keyAcceptTimeoutAttr   );
	
	// Constants used creating an AppleEvent
	
	typedef Nucleus::ID< class AEReturnID_Tag, ::AEReturnID >::Type AEReturnID;
	
	static const AEReturnID kAutoGenerateReturnID = AEReturnID( ::kAutoGenerateReturnID );
	
	
	typedef Nucleus::ID< class AETransactionID_Tag, ::AETransactionID >::Type AETransactionID;
	
	static const AETransactionID kAnyTransactionID = AETransactionID( ::kAnyTransactionID );
	
	
	struct AESendPriority_Tag  {};
	typedef Nucleus::FlagType< AESendPriority_Tag, ::AESendPriority, kAENormalPriority > AESendPriority;
	
	struct AESendMode_Tag  {};
	typedef Nucleus::FlagType< AESendMode_Tag, ::AESendMode > AESendMode;
	
	#pragma mark -
	#pragma mark ¥ DescType_Traits ¥
	
   template < ::DescType > struct DescType_Traits;
   
   template<> struct DescType_Traits< ::typeChar > : public Nucleus::StringFlattener< std::string > {};

   template<> struct DescType_Traits< ::typeFixed > : public FixedFlattener {};

   class AEEnumeratedTag {};
   typedef Nucleus::SelectorType< AEEnumeratedTag, UInt32 > AEEnumerated;
   typedef AEEnumerated AEEnumeration;
   
   class AEEventClassTag {};
   typedef Nucleus::SelectorType< AEEventClassTag, ::AEEventClass > AEEventClass;

   class AEEventIDTag {};
   typedef Nucleus::SelectorType< AEEventIDTag, ::AEEventID > AEEventID;
   
   template<> struct DescType_Traits< typeNull >                   { typedef void Result; };
   
   template<> struct DescType_Traits< typeBoolean >                : BooleanFlattener                     {};
   template<> struct DescType_Traits< typeSInt16 >                 : Nucleus::PodFlattener< SInt16 >      {};
   template<> struct DescType_Traits< typeSInt32 >                 : Nucleus::PodFlattener< SInt32 >      {};
   template<> struct DescType_Traits< typeUInt32 >                 : Nucleus::PodFlattener< UInt32 >      {};
   template<> struct DescType_Traits< typeSInt64 >                 : Nucleus::PodFlattener< SInt64 >      {};
   template<> struct DescType_Traits< typeIEEE32BitFloatingPoint > : Nucleus::PodFlattener< float >       {};
   template<> struct DescType_Traits< typeIEEE64BitFloatingPoint > : Nucleus::PodFlattener< double >      {};
   template<> struct DescType_Traits< type128BitFloatingPoint >    : Nucleus::PodFlattener< long double > {};

	inline std::size_t SizeOf_AppParameters( const AppParameters& appParameters )
	{
		return sizeof (AppParameters) + appParameters.messageLength;
	}
	
   template<> struct DescType_Traits< typeEventRecord >            : Nucleus::PodFlattener< EventRecord >                      {};
   template<> struct DescType_Traits< typeAlias >                  : HandleFlattener< AliasRecord >                            {};
   template<> struct DescType_Traits< typeEnumerated >             : Nucleus::ConvertingPODFlattener< AEEnumerated, UInt32 >   {};
   template<> struct DescType_Traits< typeType >                   : Nucleus::ConvertingPODFlattener< DescType, ::DescType >   {};
   template<> struct DescType_Traits< typeAppParameters >          : Nucleus::VariableLengthPodFlattener< AppParameters, SizeOf_AppParameters > {};
   template<> struct DescType_Traits< typeFSS >                    : Nucleus::PodFlattener< FSSpec >                           {};
   template<> struct DescType_Traits< typeFSRef >                  : Nucleus::PodFlattener< FSRef >                            {};
   template<> struct DescType_Traits< typeKeyword >                : Nucleus::ConvertingPODFlattener< AEKeyword, ::AEKeyword > {};
   template<> struct DescType_Traits< typeApplSignature >          : Nucleus::ConvertingPODFlattener< OSType, ::OSType >       {};
   template<> struct DescType_Traits< typeQDRectangle >            : Nucleus::PodFlattener< Rect >                             {};
   template<> struct DescType_Traits< typeProcessSerialNumber >    : Nucleus::PodFlattener< ProcessSerialNumber >              {};
   template<> struct DescType_Traits< typeApplicationURL >         : DescType_Traits< typeChar >                               {};
   
	
	template < ::DescType type >
	struct DescType_Map_Traits
	{
		static const ::DescType result = type;
	};
	
	template < class Integer > struct Integer_DescType_Traits;
	
	template <> struct Integer_DescType_Traits< SInt16 >  { static const ::DescType descType = typeSInt16; };
	template <> struct Integer_DescType_Traits< SInt32 >  { static const ::DescType descType = typeSInt32; };
	template <> struct Integer_DescType_Traits< UInt32 >  { static const ::DescType descType = typeUInt32; };
	
	struct Type_DescType_Traits  { static const ::DescType descType = typeType; };
	
	template < class POD, class Integer >
	struct Integer_AEKeyword_Traits : Nucleus::ConvertingPODFlattener< POD, Integer >,
	                                  Integer_DescType_Traits< Integer >
	{
	};
	
	template < class POD >
	struct Type_AEKeyword_Traits : Nucleus::ConvertingPODFlattener< POD, ::FourCharCode >,
	                               Type_DescType_Traits
	{
	};
	
	template < AEKeyword key > struct AEKeyword_Traits;
	
	template <> struct AEKeyword_Traits< keyTransactionIDAttr > : Integer_AEKeyword_Traits< AETransactionID, ::AETransactionID > {};
	template <> struct AEKeyword_Traits< keyReturnIDAttr      > : Integer_AEKeyword_Traits< AEReturnID,      ::AEReturnID      > {};
	
	template <> struct AEKeyword_Traits< keyEventClassAttr > : Type_AEKeyword_Traits< AEEventClass > {};
	template <> struct AEKeyword_Traits< keyEventIDAttr    > : Type_AEKeyword_Traits< AEEventID    > {};
	
	#pragma mark -
	#pragma mark ¥ AEDesc ¥
	
	using ::AEDesc;
	using ::AEDescList;
    using ::AEKeyDesc;
	using ::AEAddressDesc;
	using ::AERecord;
	using ::AppleEvent;
	
	typedef AEDesc     AEToken;
	typedef AEDescList AETokenList;
	typedef AERecord   AEObjectSpecifier;
  }

namespace Nucleus
  {
	template <>
	struct Disposer< Nitrogen::AEDesc > : public std::unary_function< Nitrogen::AEDesc, void >,
	                                      private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		// parameter can't be const
		void operator()( Nitrogen::AEDesc desc ) const
		{
			// AEDisposeDesc() is documented as only ever returning noErr,
			// but we check anyway to be future-proof.
			
			(void) Nitrogen::AppleEventManagerErrorsRegistrationDependency();
			HandleDestructionOSStatus( ::AEDisposeDesc( &desc ) );
		}
	};

	template <>
	struct LivelinessTraits< Nitrogen::AEDesc, Disposer< Nitrogen::AEDesc > >   { typedef SeizedValuesAreLive LivelinessTest; };
	
	template <>
	struct Maker< Nitrogen::AEDesc >
	{
		Nitrogen::AEDesc operator()() const
		{
			Nitrogen::AEDesc result = { typeNull, NULL };
			return result;
		}
	};
	
	template <>
	struct Disposer< Nitrogen::AEKeyDesc > : public std::unary_function< Nitrogen::AEKeyDesc, void >,
	                                         private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		// parameter can't be const
		void operator()( Nitrogen::AEKeyDesc keyDesc ) const
		{
			Disposer< Nitrogen::AEDesc >()( keyDesc.descContent );
		}
	};
	
	template <>
	struct LivelinessTraits< Nitrogen::AEKeyDesc, Disposer< Nitrogen::AEKeyDesc > >
	{
		typedef SeizedValuesAreLive LivelinessTest;
	};
	
	template <>
	struct Maker< Nitrogen::AEKeyDesc >
	{
		Nitrogen::AEKeyDesc operator()() const
		{
			Nitrogen::AEKeyDesc result = { Nitrogen::AEKeyword(), Make< Nitrogen::AEDesc >() };
			return result;
		}
	};
  }

namespace Nitrogen
  {	
	#pragma mark -
	#pragma mark ¥ Coercions ¥
	
	struct AECoerceDescUPP_Details: Basic_UPP_Details< ::AECoerceDescUPP,
	                                                   ::AECoerceDescProcPtr,
	                                                   ::NewAECoerceDescUPP,
	                                                   ::DisposeAECoerceDescUPP,
	                                                   ::InvokeAECoerceDescUPP >
	{};
	
	struct AECoercePtrUPP_Details: Basic_UPP_Details< ::AECoercePtrUPP,
	                                                  ::AECoercePtrProcPtr,
	                                                  ::NewAECoercePtrUPP,
	                                                  ::DisposeAECoercePtrUPP,
	                                                  ::InvokeAECoercePtrUPP >
	{};
	
	typedef UPP< AECoerceDescUPP_Details > AECoerceDescUPP;
	typedef UPP< AECoercePtrUPP_Details  > AECoercePtrUPP;
	
	inline Nucleus::Owned< AECoerceDescUPP > NewAECoerceDescUPP( ::AECoerceDescProcPtr p )
	{
		return NewUPP< AECoerceDescUPP >( p );
	}
	
	inline Nucleus::Owned< AECoercePtrUPP > NewAECoercePtrUPP( ::AECoercePtrProcPtr p )
	{
		return NewUPP< AECoercePtrUPP >( p );
	}
	
	inline void DisposeAECoerceDescUPP( Nucleus::Owned< AECoerceDescUPP > )  {}
	inline void DisposeAECoercePtrUPP ( Nucleus::Owned< AECoercePtrUPP  > )  {}
	
	inline void InvokeAECoerceDescUPP( const AEDesc&    fromDesc,
	                                   DescType         toType,
	                                   RefCon           handlerRefCon,
	                                   AEDesc&          toDesc,
	                                   AECoerceDescUPP  userUPP )
	{
		ThrowOSStatus( userUPP( &fromDesc,
		                        toType,
		                        handlerRefCon,
		                        &toDesc ) );
	}
	
	inline void InvokeAECoercePtrUPP( DescType        typeCode,
	                                  const void*     dataPtr,
	                                  std::size_t     dataSize,
	                                  DescType        toType,
	                                  RefCon          handlerRefCon,
	                                  AEDesc&         toDesc,
	                                  AECoercePtrUPP  userUPP )
	{
		ThrowOSStatus( userUPP( typeCode,
		                        dataPtr,
		                        dataSize,
		                        toType,
		                        handlerRefCon,
		                        &toDesc ) );
	}
	
	struct AECoercionHandlerUPP : AECoerceDescUPP
	{
		AECoercionHandlerUPP()  {}
		AECoercionHandlerUPP( ::AECoerceDescUPP p ) : AECoerceDescUPP( p )  {}
	
	#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM  ||  OPAQUE_UPP_TYPES
		
		AECoercionHandlerUPP( ::AECoercePtrUPP p )
		:
			AECoerceDescUPP( reinterpret_cast< ::AECoerceDescUPP >( p ) )
		{}
		
	#endif
	};
	
	struct AECoercionHandler
	{
		DescType              fromType;
		DescType              toType;
		AECoercionHandlerUPP  handler;
		RefCon                handlerRefCon;
		bool                  fromTypeIsDesc;
		bool                  isSysHandler;
		
		AECoercionHandler();
		
		AECoercionHandler( DescType              fromType,
		                   DescType              toType,
		                   AECoercionHandlerUPP  handler,
		                   RefCon                handlerRefCon,
		                   bool                  fromTypeIsDesc,
		                   bool                  isSysHandler )
		:
			fromType      ( fromType       ),
			toType        ( toType         ),
			handler       ( handler        ),
			handlerRefCon ( handlerRefCon  ),
			fromTypeIsDesc( fromTypeIsDesc ),
			isSysHandler  ( isSysHandler   )
		{}
		
		AECoercionHandler( DescType         fromType,
		                   DescType         toType,
		                   AECoerceDescUPP  handler,
		                   RefCon           handlerRefCon,
		                   bool             isSysHandler )
		:
			fromType      ( fromType      ),
			toType        ( toType        ),
			handler       ( handler       ),
			handlerRefCon ( handlerRefCon ),
			fromTypeIsDesc( true          ),
			isSysHandler  ( isSysHandler  )
		{}
		
		AECoercionHandler( DescType        fromType,
		                   DescType        toType,
		                   AECoercePtrUPP  handler,
		                   RefCon          handlerRefCon,
		                   bool            isSysHandler )
		:
			fromType      ( fromType      ),
			toType        ( toType        ),
			handler       ( handler       ),
			handlerRefCon ( handlerRefCon ),
			fromTypeIsDesc( false         ),
			isSysHandler  ( isSysHandler  )
		{}
	};
	
	bool operator==( const AECoercionHandler& a, const AECoercionHandler& b );
	
	inline bool operator!=( const AECoercionHandler& a, const AECoercionHandler& b )
	{
		return !( a == b );
	}
	
  }

namespace Nucleus
  {
	template <>
	struct Disposer< Nitrogen::AECoercionHandler > : public std::unary_function< Nitrogen::AECoercionHandler, void >,
												     private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( const Nitrogen::AECoercionHandler& installation ) const
		{
			HandleDestructionOSStatus( ::AERemoveCoercionHandler( installation.fromType,
			                                                      installation.toType,
			                                                      installation.handler,
			                                                      installation.isSysHandler ) );
		}
	};
  }

namespace Nitrogen
  {
	typedef Nucleus::Owned< AEDesc > ( *AECoerceDescProcPtr )( const AEDesc&  fromDesc,
	                                                           DescType       toType,
	                                                           RefCon         refCon );
	
	typedef Nucleus::Owned< AEDesc > ( *AECoercePtrProcPtr )( DescType     typeCode,
	                                                          const void*  dataPtr,
	                                                          std::size_t  datasize,
	                                                          DescType     toType,
	                                                          RefCon       refCon );
	
	template < AECoerceDescProcPtr handler >
	struct Adapt_AECoerceDesc
	{
		static pascal OSErr ToCallback( const AEDesc*  fromDesc,
		                                ::DescType     toType,
		                                long           refCon,
		                                AEDesc*        result )
		{
			try
			{
				*result = handler( *fromDesc, toType, refCon ).Release();
			}
			catch ( OSStatus err )
			{
				return err.Get();
			}
			
			return noErr;
		}
	};
	
	template < AECoercePtrProcPtr handler >
	struct Adapt_AECoercePtr
	{
		static pascal OSErr ToCallback( ::DescType   typeCode,
		                                const void*  dataPtr,
		                                ::Size       dataSize,
		                                ::DescType   toType,
		                                long         refCon,
		                                AEDesc*      result )
		{
			try
			{
				*result = handler( typeCode,
				                   dataPtr,
				                   dataSize,
				                   toType,
				                   refCon ).Release();
			}
			catch ( OSStatus err )
			{
				return err.Get();
			}
			
			return noErr;
		}
	};
	
	// 388
	Nucleus::Owned< AECoercionHandler > AEInstallCoercionHandler( const AECoercionHandler& toInstall );
	
	inline Nucleus::Owned< AECoercionHandler > AEInstallCoercionHandler( DescType fromType,
	                                                                     DescType toType,
	                                                                     AECoerceDescUPP handler,
	                                                                     RefCon handlerRefCon = RefCon(),
	                                                                     Boolean isSysHandler = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    handler,
		                                                    handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	inline Nucleus::Owned< AECoercionHandler > AEInstallCoercionHandler( DescType        fromType,
	                                                                     DescType        toType,
	                                                                     AECoercePtrUPP  handler,
	                                                                     RefCon          handlerRefCon = RefCon(),
	                                                                     Boolean         isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    handler,
		                                                    handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	template < typename AECoerceDescUPP::ProcPtr handler >
	Nucleus::Owned< AECoercionHandler > AEInstallCoercionHandler( DescType  fromType,
	                                                              DescType  toType,
	                                                              RefCon    handlerRefCon = RefCon(),
	                                                              Boolean   isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    StaticUPP< AECoerceDescUPP, handler >(),
		                                                    handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	template < typename AECoercePtrUPP::ProcPtr handler >
	Nucleus::Owned< AECoercionHandler > AEInstallCoercionHandler( DescType  fromType,
	                                                              DescType  toType,
	                                                              RefCon    handlerRefCon = RefCon(),
	                                                              Boolean   isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    StaticUPP< AECoercePtrUPP, handler >(),
		                                                    handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	template < AECoerceDescProcPtr handler >
	Nucleus::Owned< AECoercionHandler > AEInstallCoercionHandler( DescType  fromType,
	                                                              DescType  toType,
	                                                              RefCon    handlerRefCon = RefCon(),
	                                                              Boolean   isSysHandler  = false )
	{
		return AEInstallCoercionHandler< Adapt_AECoerceDesc< handler >::ToCallback >( fromType,
		                                                                              toType,
		                                                                              handlerRefCon,
		                                                                              isSysHandler );
	}
	
	template < AECoercePtrProcPtr handler >
	Nucleus::Owned< AECoercionHandler > AEInstallCoercionHandler( DescType  fromType,
	                                                              DescType  toType,
	                                                              RefCon    handlerRefCon = RefCon(),
	                                                              Boolean   isSysHandler  = false )
	{
		return AEInstallCoercionHandler< Adapt_AECoercePtr< handler >::ToCallback >( fromType,
		                                                                             toType,
		                                                                             handlerRefCon,
		                                                                             isSysHandler );
	}
	
	// 406
	inline void AERemoveCoercionHandler( Nucleus::Owned< AECoercionHandler > )  {}
	
	typedef AECoercionHandler AEGetCoercionHandler_Result;
	
	AECoercionHandler AEGetCoercionHandler( DescType  fromType,
	                                        DescType  toType,
	                                        bool      isSysHandler );
	
	// 444
	Nucleus::Owned< AEDesc > AECoercePtr( DescType     typeCode,
	                                      const void*  dataPtr,
	                                      Size         dataSize,
	                                      DescType     toType );
	
	// 461
	Nucleus::Owned< AEDesc > AECoerceDesc( const AEDesc& desc, DescType toType );
	
	#pragma mark -
	#pragma mark ¥ AEDescs ¥
	
	namespace Detail
	{
		
		class AEDescEditor
		{
			private:
				Nucleus::Owned< AEDesc >& desc;
				AEDesc workingCopy;
			
			public:
				AEDescEditor( Nucleus::Owned< AEDesc >& desc )
				:
					desc       ( desc           ), 
					workingCopy( desc.Release() )
				{}
				~AEDescEditor()  { desc = Nucleus::Owned< AEDesc >::Seize( workingCopy ); }
				
				AEDesc& Get()       { return workingCopy; }
				operator AEDesc&()  { return Get();       }
		};
		
	}
	
	struct AEDesc_Info
	{
		DescType typeCode;
		Size dataSize;
		
		operator Size() const  { return dataSize; }
	};
	
	typedef AEDesc_Info AESizeOfNthItem_Result,
	                    AESizeOfKeyDesc_Result,
	                    AEGetKeyPtr_Result,
	                    AESizeOfParam_Result,
	                    AESizeOfAttribute_Result,
	                    AEGetParamPtr_Result,
	                    AEGetAttributePtr_Result;

	// 484
	inline Nucleus::Owned< AEDesc > AEInitializeDesc()
	{
		return Nucleus::Owned< AEDesc >::Seize( Nucleus::Make< AEDesc >() );
	}
	
	Nucleus::Owned< AEDesc > AECreateDesc( DescType     typeCode,
	                                       const void*  dataPtr,
	                                       Size         dataSize );
	
	Nucleus::Owned< AEDesc > AECreateDesc( DescType  typeCode,
	                                       Handle    handle );
	
	Nucleus::Owned< AEDesc > AECreateDesc( DescType         typeCode,
	                                       Nucleus::Owned< Handle >  handle );
	
	template < class T >
	Nucleus::Owned< AEDesc > AECreateDesc( DescType typeCode, T** handle )
	{
		return AECreateDesc( typeCode, Handle( handle ) );
	}
	
	template < class T >
	Nucleus::Owned< AEDesc > AECreateDesc( DescType                                            typeCode,
	                                       Nucleus::Owned< T**, Nucleus::Disposer< Handle > >  handle )
	{
		return AECreateDesc( typeCode, Nucleus::Owned< Handle >( handle ) );
	}
	
	Nucleus::Owned< AEDesc > AECreateDesc( DescType typeCode, Nucleus::Owned< AEDesc > desc );
	
	
	inline void AEDisposeDesc( Nucleus::Owned< AEDesc > )  {}
	
	Nucleus::Owned< AEDesc > AEDuplicateDesc( const AEDesc& desc );
	
	#pragma mark -
	#pragma mark ¥ AEDescLists ¥
	
	Nucleus::Owned< AEDesc > AECreateList( bool isRecord = false );
	
	Nucleus::Owned< AEDesc > AECreateList( const void*  factoringPtr,
	                                       std::size_t  factoredSize,
	                                       bool         isRecord );
	
	inline Nucleus::Owned< AEDesc > AECreateRecord()  { return AECreateList( true ); }
	
	template < bool isRecord >
	Nucleus::Owned< AEDesc > AECreateList()  { return AECreateList( isRecord ); }
	
	template < bool isRecord >
	Nucleus::Owned< AEDesc > AECreateList( DescType typeCode )
	{
		return AECreateDesc( typeCode, AECreateList< isRecord >() );
	}
	
	long AECountItems( const AEDesc& desc );
	
	void AEPutPtr( AEDescList&  list,
	               long         index,
	               DescType     type,
	               const void*  dataPtr,
	               Size         dataSize );
	
	void AEPutPtr( Nucleus::Owned< AEDescList >&  list,
	               long                           index,
	               DescType                       type,
	               const void*                    dataPtr,
	               Size                           dataSize );
	
	void AEPutDesc( AEDescList&    list,
	                long           index,
	                const AEDesc&  desc );
	
	void AEPutDesc( Nucleus::Owned< AEDescList >&  list,
	                long                           index,
	                const AEDesc&                  desc );
	
	struct GetNthPtr_Result
	{
		AEKeyword keyword;
		DescType typeCode;
		Size actualSize;
	};
	
	GetNthPtr_Result AEGetNthPtr( const AEDesc&  listDesc,
	                              long           index,
	                              DescType       desiredType,
	                              void*          dataPtr,
	                              Size           maximumSize );
	
	Nucleus::Owned< AEDesc > AEGetNthDesc( const AEDesc&  listDesc,
	                                       long           index,
	                                       DescType       desiredType = typeWildCard,
	                                       ::AEKeyword*   keyword     = NULL );
	
	Nucleus::Owned< AEDesc > AEGetNthDesc( const AEDesc&  listDesc,
	                                       long           index,
	                                       ::AEKeyword*   keyword );
	
	AESizeOfNthItem_Result AESizeOfNthItem( const AEDescList& list, long index );
	
	void AEDeleteItem(                 AEDescList  & listDesc, long index );
	void AEDeleteItem( Nucleus::Owned< AEDescList >& listDesc, long index );
	
	#pragma mark -
	#pragma mark ¥ AERecords ¥
	
	inline bool AECheckIsRecord( const AEDesc& theDesc )
	{
		return ::AECheckIsRecord( &theDesc );
	}
	
	void AEPutKeyPtr( AERecord&    record,
	                  AEKeyword    keyword,
	                  DescType     typeCode,
	                  const void*  dataPtr,
	                  std::size_t  dataSize );
	
	void AEPutKeyPtr( Nucleus::Owned< AERecord >&  record,
	                  AEKeyword                    keyword,
	                  DescType                     typeCode,
	                  const void*                  dataPtr,
	                  std::size_t                  dataSize );
	
	void AEPutKeyDesc( AERecord&      record,
	                   AEKeyword      keyword,
	                   const AEDesc&  desc );
	
	void AEPutKeyDesc( AERecord&         record,
	                   const AEKeyDesc&  keyDesc );
	
	void AEPutKeyDesc( Nucleus::Owned< AERecord >&  record,
	                   AEKeyword                    keyword,
	                   const AEDesc&                desc );
	
	void AEPutKeyDesc( Nucleus::Owned< AERecord >&  record,
	                   const AEKeyDesc&             keyDesc );
	
	AEGetKeyPtr_Result AEGetKeyPtr( const AERecord&  record,
	                                AEKeyword        keyword,
	                                DescType         desiredType,
	                                void*            dataPtr,
	                                std::size_t      maximumSize );
	
	Nucleus::Owned< AEDesc > AEGetKeyDesc( const AERecord&  record,
	                                       AEKeyword        keyword,
	                                       DescType         desiredType );
	
	AESizeOfKeyDesc_Result AESizeOfKeyDesc( const AERecord&  record,
	                                        AEKeyword        keyword );
	
	void AEDeleteKeyDesc(                 AERecord  & record, AEKeyword keyword );
	void AEDeleteKeyDesc( Nucleus::Owned< AERecord >& record, AEKeyword keyword );
	
	#pragma mark -
	#pragma mark ¥ AppleEvents ¥
	
	Nucleus::Owned< AppleEvent > AECreateAppleEvent( AEEventClass          eventClass,
	                                                 AEEventID             eventID,
	                                                 const AEAddressDesc&  target,
	                                                 AEReturnID            returnID      = AEReturnID(),
	                                                 AETransactionID       transactionID = AETransactionID() );
	
	void AEPutParamPtr( AppleEvent&  appleEvent,
	                    AEKeyword    keyword,
	                    DescType     typeCode,
	                    const void*  dataPtr,
	                    std::size_t  dataSize );
	
	void AEPutParamPtr( Nucleus::Owned< AppleEvent >&  appleEvent,
	                    AEKeyword                      keyword,
	                    DescType                       typeCode,
	                    const void*                    dataPtr,
	                    std::size_t                    dataSize );
	
	void AEPutParamDesc( AppleEvent&    appleEvent,
	                     AEKeyword      keyword,
	                     const AEDesc&  desc );
	
	void AEPutParamDesc( Nucleus::Owned< AppleEvent >&  appleEvent,
	                     AEKeyword                      keyword,
	                     const AEDesc&                  desc );
	
	AEGetParamPtr_Result AEGetParamPtr( const AppleEvent&  appleEvent,
	                                    AEKeyword          keyword,
	                                    DescType           desiredType,
	                                    void*              dataPtr,
	                                    std::size_t        maximumSize );
	
	Nucleus::Owned< AEDesc > AEGetParamDesc( const AppleEvent&  appleEvent,
	                                         AEKeyword          keyword,
	                                         DescType           desiredType );
	
	AESizeOfParam_Result AESizeOfParam( const AppleEvent&  appleEvent,
	                                    AEKeyword          keyword );
	
	void AEDeleteParam(                 AppleEvent  & appleEvent, AEKeyword keyword );
	void AEDeleteParam( Nucleus::Owned< AppleEvent >& appleEvent, AEKeyword keyword );
	
	void AEPutAttributePtr( AppleEvent&  appleEvent,
	                        AEKeyword    keyword,
	                        DescType     typeCode,
	                        const void*  dataPtr,
	                        std::size_t  dataSize );
	
	void AEPutAttributePtr( Nucleus::Owned< AppleEvent >&  appleEvent,
	                        AEKeyword                      keyword,
	                        DescType                       typeCode,
	                        const void*                    dataPtr,
	                        std::size_t                    dataSize );
	
	void AEPutAttributeDesc( AppleEvent&    appleEvent,
	                         AEKeyword      keyword,
	                         const AEDesc&  desc );
	
	void AEPutAttributeDesc( Nucleus::Owned< AppleEvent >&  appleEvent,
	                         AEKeyword                      keyword,
	                         const AEDesc&                  desc );
	
	AEGetAttributePtr_Result AEGetAttributePtr( const AppleEvent&  appleEvent,
	                                            AEKeyword          keyword,
	                                            DescType           desiredType,
	                                            void*              dataPtr,
	                                            std::size_t        maximumSize );
	
	Nucleus::Owned< AEDesc > AEGetAttributeDesc( const AppleEvent&  appleEvent,
	                                             AEKeyword          keyword,
	                                             DescType           desiredType );
	
	AESizeOfAttribute_Result AESizeOfAttribute( const AppleEvent&  appleEvent,
	                                            AEKeyword          keyword );
	
	#pragma mark -
	#pragma mark ¥ Opaque data ¥
	
	void AEGetDescData( const AEDesc&  desc,
	                    void*          dataPtr,
	                    std::size_t    maximumSize );
	
	inline std::size_t AEGetDescDataSize( const AEDesc& desc )
	{
		return ::AEGetDescDataSize( &desc );
	}
	
	void AEReplaceDescData( DescType     typeCode,
	                        const void*  dataPtr,
	                        std::size_t  dataSize,
	                        AEDesc&      result );
	
	void AEReplaceDescData( DescType                   typeCode,
	                        const void*                dataPtr,
	                        std::size_t                dataSize,
	                        Nucleus::Owned< AEDesc >&  result );
	
	
	#pragma mark -
	#pragma mark ¥ AEEventHandlerUPP ¥
	
   struct AEEventHandlerUPP_Details: Basic_UPP_Details< ::AEEventHandlerUPP,
                                                        ::AEEventHandlerProcPtr,
                                                        ::NewAEEventHandlerUPP,
                                                        ::DisposeAEEventHandlerUPP,
                                                        ::InvokeAEEventHandlerUPP >
     {};
   
   typedef UPP< AEEventHandlerUPP_Details > AEEventHandlerUPP;
   
   //using ::AEEventHandlerProcPtr;
   
   inline Nucleus::Owned< AEEventHandlerUPP > NewAEEventHandlerUPP( ::AEEventHandlerProcPtr p )
     {
      return NewUPP< AEEventHandlerUPP >( p );
     }

   inline void DisposeAEEventHandlerUPP( Nucleus::Owned< AEEventHandlerUPP > )
     {
     }
   
   inline void InvokeAEEventHandlerUPP( const AppleEvent& theAppleEvent,
                                        AppleEvent& reply,
                                        RefCon handlerRefCon,
                                        AEEventHandlerUPP userUPP )
     {
      ThrowOSStatus( userUPP( &theAppleEvent, &reply, handlerRefCon ) );
     }
	
	#pragma mark -
	#pragma mark ¥ Powered by DescType_Traits ¥
	
	class AECoercePtr_Putter
	{
		private:
			DescType fromType;
			DescType toType;
			Nucleus::Owned< AEDesc >& theDesc;
		
		public:
			AECoercePtr_Putter( DescType                   from,
			                    DescType                   to,
			                    Nucleus::Owned< AEDesc >&  desc )
			: fromType( from ),
			  toType  ( to   ),
			  theDesc ( desc )  {}
			
			void operator()( const void *begin, const void *end ) const
			{
				theDesc = AECoercePtr( fromType, begin, Detail::Distance( begin, end ), toType );
			}
	};
	
	template < ::DescType type >
	Nucleus::Owned< AEDesc > AECoercePtr( typename DescType_Traits< type >::Parameter  data,
	                                      DescType                                     toType )
	{
		Nucleus::Owned< AEDesc > result;
		
		DescType_Traits< type >().Put( data,
		                               AECoercePtr_Putter( DescType( DescType_Map_Traits< type >::result ),
		                                                   toType,
		                                                   result ) );
		
		return result;
	}
	
	
	class AECreateDesc_Putter
	{
		private:
			DescType theType;
			Nucleus::Owned< AEDesc >& theDesc;
		
		public:
			AECreateDesc_Putter( DescType type, Nucleus::Owned< AEDesc >& desc ) : theType( type ), theDesc( desc )  {}
			
			void operator()( const void *begin, const void *end ) const
			{
				theDesc = AECreateDesc( theType, begin, Detail::Distance( begin, end ) );
			}
	};
	
	template < ::DescType type >
	Nucleus::Owned< AEDesc > AECreateDesc( typename DescType_Traits< type >::Parameter data )
	{
		Nucleus::Owned< AEDesc > result;
		
		DescType_Traits< type >().Put( data,
		                               AECreateDesc_Putter( DescType( DescType_Map_Traits< type >::result ),
		                                                    result ) );
		
		return result;
	}
	
	
	class AEPutPtr_Putter
	{
		private:
			AEDescList& theList;
			long        theIndex;
			DescType    theType;
		
		public:
			AEPutPtr_Putter( AEDescList&  list,
			                 long         index,
			                 DescType     type )
			: theList ( list  ),
			  theIndex( index ),
			  theType ( type  )  {}
			
			void operator()( const void *begin, const void *end ) const
			{
				AEPutPtr( theList, theIndex, theType, begin, Detail::Distance( begin, end ) );
			}
	};
	
	template < ::DescType type >
	void AEPutPtr( AEDescList&                                  list,
	               long                                         index,
	               typename DescType_Traits< type >::Parameter  data )
	{
		DescType_Traits< type >().Put( data,
		                               AEPutPtr_Putter( list,
		                               index,
		                               DescType( DescType_Map_Traits< type >::result ) ) );
	}
	
	template < ::DescType type, class Disposer >
	void AEPutPtr( Nucleus::Owned< AEDescList, Disposer >&               list,
	               long                                                  index,
	               typename DescType_Traits< type >::Parameter           data )
	{
		DescType_Traits< type >().Put( data,
		                               AEPutPtr_Putter( Detail::AEDescEditor( list ),
		                                                index,
		                                                DescType( DescType_Map_Traits< type >::result ) ) );
	}
	
	
	template < ::DescType type >
	class AEGetNthPtr_Getter
	{
		private:
			const AEDescList& theList;
			long              theIndex;
		
		public:
			AEGetNthPtr_Getter( const AEDescList& list, long index )
			: theList ( list  ),
			  theIndex( index )  {}
			
			std::size_t size() const
			{
				AESizeOfNthItem_Result info = AESizeOfNthItem( theList, theIndex );
				
				if ( DescType_Traits< type >::hasStaticSize )
				{
					return sizeof (typename DescType_Traits< type >::Buffer);
				}
				
				ASSERT( info.typeCode == type );
				
				return info.dataSize;
			}
			
			void operator()( void *begin, void *end ) const
			{
				AEGetNthPtr( theList, theIndex, type, begin, Detail::Distance( begin, end ) );
			}
	};
	
	template < ::DescType type >
	typename DescType_Traits< type >::Result
	AEGetNthPtr( const AEDescList&  listDesc,
	             long               index )
	{
		return DescType_Traits< type >().Get( AEGetNthPtr_Getter< DescType_Map_Traits< type >::result >( listDesc, index ) );
	}
	
	
	class AEPutKeyPtr_Putter
	{
		private:
			AERecord& theRecord;
			AEKeyword theKeyword;
			DescType  theType;
		
		public:
			AEPutKeyPtr_Putter( AERecord&  record,
			                    AEKeyword  keyword,
			                    DescType   type )
			: theRecord ( record  ),
			  theKeyword( keyword ),
			  theType   ( type    )  {}
			
			void operator()( const void *begin, const void *end ) const
			{
				AEPutKeyPtr( theRecord, theKeyword, theType, begin, Detail::Distance( begin, end ) );
			}
	};
	
	/*
	template < ::DescType type >
	void AEPutKeyPtr( AERecord&                                    record,
	                  AEKeyword                                    keyword,
	                  typename DescType_Traits< type >::Parameter  data )
	{
		DescType_Traits< type >().Put( data,
		                               AEPutKeyPtr_Putter( record,
		                                                   keyword,
		                                                   DescType( DescType_Map_Traits< type >::result ) ) );
	}
	
	template < ::DescType type, class Disposer >
	void AEPutKeyPtr( Nucleus::Owned< AERecord, Disposer >&                 record,
	                  AEKeyword                                             keyword,
	                  typename DescType_Traits< type >::Parameter           data )
	{
		DescType_Traits< type >().Put( data,
		                               AEPutKeyPtr_Putter( Detail::AEDescEditor( record ),
		                                                   keyword,
		                                                   DescType( DescType_Map_Traits< type >::result ) ) );
	}
	*/
	
	template < AEKeyword key >
	void AEPutKeyPtr( AERecord&                                    record,
	                  typename AEKeyword_Traits< key >::Parameter  data )
	{
		AEKeyword_Traits< key >().Put( data,
		                               AEPutKeyPtr_Putter( record,
		                                                   key,
		                                                   DescType( AEKeyword_Traits< key >::descType ) ) );
	}
	
	template < AEKeyword key, class Disposer >
	void AEPutKeyPtr( Nucleus::Owned< AERecord, Disposer >&        record,
	                  typename AEKeyword_Traits< key >::Parameter  data )
	{
		AEPutKeyPtr< key >( Detail::AEDescEditor( record ), data );
	}
	
	
	template < ::DescType type >
	class AEGetKeyPtr_Getter
	{
		private:
			const AERecord& theRecord;
			AEKeyword       theKeyword;
		
		public:
			AEGetKeyPtr_Getter( const AERecord& record, AEKeyword keyword )
			: theRecord ( record  ),
			  theKeyword( keyword )  {}
			
			std::size_t size() const
			{
				AESizeOfKeyDesc_Result info = AESizeOfKeyDesc( theRecord, theKeyword );
				
				if ( DescType_Traits< type >::hasStaticSize )
				{
					return sizeof (typename DescType_Traits< type >::Buffer);
				}
				
				ASSERT( info.typeCode == type );
				
				return info.dataSize;
			}
			
			void operator()( void *begin, void *end ) const
			{
				AEGetKeyPtr( theRecord, theKeyword, type, begin, Detail::Distance( begin, end ) );
			}
	};
	
	/*
	template < ::DescType type >
	typename DescType_Traits< type >::Result
	AEGetKeyPtr( const AERecord&  record,
	             AEKeyword        keyword )
	{
		return DescType_Traits< type >().Get( AEGetKeyPtr_Getter< DescType_Map_Traits< type >::result >( record, keyword ) );
	}
	*/
	
	template < AEKeyword key >
	typename AEKeyword_Traits< key >::Result
	AEGetKeyPtr( const AERecord& record )
	{
		return AEKeyword_Traits< key >().Get( AEGetKeyPtr_Getter< AEKeyword_Traits< key >::descType >( record, key ) );
	}
	
	
	class AEPutParamPtr_Putter
	{
		private:
			AppleEvent& theAppleEvent;
			AEKeyword   theKeyword;
			DescType    theType;
		
		public:
			AEPutParamPtr_Putter( AppleEvent&  appleEvent,
			                      AEKeyword    keyword,
			                      DescType     type )
			: theAppleEvent( appleEvent  ),
			  theKeyword   ( keyword     ),
			  theType      ( type        )  {}
			
			void operator()( const void *begin, const void *end ) const
			{
				AEPutParamPtr( theAppleEvent, theKeyword, theType, begin, Detail::Distance( begin, end ) );
			}
	};
	
	/*
	template < ::DescType type >
	void AEPutParamPtr( AppleEvent&                                  appleEvent,
	                    AEKeyword                                    keyword,
	                    typename DescType_Traits< type >::Parameter  data )
	{
		DescType_Traits< type >().Put( data,
		                               AEPutParamPtr_Putter( appleEvent,
		                                                     keyword,
		                                                     DescType( DescType_Map_Traits< type >::result ) ) );
	}
	
	template < ::DescType type, class Disposer >
	void AEPutParamPtr( Nucleus::Owned< AppleEvent, Disposer >&               appleEvent,
	                    AEKeyword                                             keyword,
	                    typename DescType_Traits< type >::Parameter           data )
	{
		DescType_Traits< type >().Put( data,
		                               AEPutParamPtr_Putter( Detail::AEDescEditor( appleEvent ),
		                                                     keyword,
		                                                     DescType( DescType_Map_Traits< type >::result ) ) );
	}
	*/
	
	template < AEKeyword key >
	void AEPutParamPtr( AppleEvent&                                  appleEvent,
	                    typename AEKeyword_Traits< key >::Parameter  data )
	{
		AEKeyword_Traits< key >().Put( data,
		                               AEPutParamPtr_Putter( appleEvent,
		                                                     key,
		                                                     DescType( AEKeyword_Traits< key >::descType ) ) );
	}
	
	template < AEKeyword key, class Disposer >
	void AEPutParamPtr( Nucleus::Owned< AppleEvent, Disposer >&      appleEvent,
	                    typename AEKeyword_Traits< key >::Parameter  data )
	{
		AEPutParamPtr< key >( Detail::AEDescEditor( appleEvent ), data );
	}
	
	
	template < ::DescType type >
	class AEGetParamPtr_Getter
	{
		private:
			const AppleEvent& theAppleEvent;
			AEKeyword         theKeyword;
		
		public:
			AEGetParamPtr_Getter( const AppleEvent& appleEvent, AEKeyword keyword )
			: theAppleEvent( appleEvent ),
			  theKeyword   ( keyword    )  {}
			
			std::size_t size() const
			{
				AESizeOfParam_Result info = AESizeOfParam( theAppleEvent, theKeyword );
				
				if ( DescType_Traits< type >::hasStaticSize )
				{
					return sizeof (typename DescType_Traits< type >::Buffer);
				}
				
				ASSERT( info.typeCode == type );
				
				return info.dataSize;
			}
			
			void operator()( void *begin, void *end ) const
			{
				AEGetParamPtr( theAppleEvent, theKeyword, type, begin, Detail::Distance( begin, end ) );
			}
	};
	
	/*
	template < ::DescType type >
	typename DescType_Traits< type >::Result
	AEGetParamPtr( const AppleEvent&  appleEvent,
	               AEKeyword          keyword )
	{
		return DescType_Traits< type >().Get( AEGetParamPtr_Getter< DescType_Map_Traits< type >::result >( appleEvent, keyword ) );
	}
	*/
	
	template < AEKeyword key >
	typename AEKeyword_Traits< key >::Result
	AEGetParamPtr( const AppleEvent& appleEvent )
	{
		return AEKeyword_Traits< key >().Get( AEGetParamPtr_Getter< AEKeyword_Traits< key >::descType >( appleEvent, key ) );
	}
	
	
	class AEPutAttributePtr_Putter
	{
		private:
			AppleEvent& theAppleEvent;
			AEKeyword   theKeyword;
			DescType    theType;
		
		public:
			AEPutAttributePtr_Putter( AppleEvent&  appleEvent,
			                          AEKeyword    keyword,
			                          DescType     type )
			: theAppleEvent( appleEvent  ),
			  theKeyword   ( keyword     ),
			  theType      ( type        )  {}
			
			void operator()( const void *begin, const void *end ) const
			{
				AEPutAttributePtr( theAppleEvent, theKeyword, theType, begin, Detail::Distance( begin, end ) );
			}
	};
	
	/*
	template < ::DescType type >
	void AEPutAttributePtr( AppleEvent&                                  appleEvent,
	                        AEKeyword                                    keyword,
	                        typename DescType_Traits< type >::Parameter  data )
	{
		DescType_Traits< type >().Put( data,
		                               AEPutAttributePtr_Putter( appleEvent,
		                               keyword,
		                               DescType( DescType_Map_Traits< type >::result ) ) );
	}
	
	template < ::DescType type, class Disposer >
	void AEPutAttributePtr( Nucleus::Owned< AppleEvent, Disposer >&               appleEvent,
	                        AEKeyword                                             keyword,
	                        typename DescType_Traits< type >::Parameter           data )
	{
		DescType_Traits< type >().Put( data,
		                               AEPutAttributePtr_Putter( Detail::AEDescEditor( appleEvent ),
		                                                         keyword,
		                                                         DescType( DescType_Map_Traits< type >::result ) ) );
	}
	*/
	
	template < AEKeyword key >
	void AEPutAttributePtr( AppleEvent&                                  appleEvent,
	                        typename AEKeyword_Traits< key >::Parameter  data )
	{
		AEKeyword_Traits< key >().Put( data,
		                               AEPutAttributePtr_Putter( appleEvent,
		                                                         key,
		                                                         DescType( AEKeyword_Traits< key >::descType ) ) );
	}
	
	template < AEKeyword key, class Disposer >
	void AEPutAttributePtr( Nucleus::Owned< AppleEvent, Disposer >&      appleEvent,
	                        typename AEKeyword_Traits< key >::Parameter  data )
	{
		AEPutAttributePtr< key >( Detail::AEDescEditor( appleEvent ), data );
	}
	
	
	template < ::DescType type >
	class AEGetAttributePtr_Getter
	{
		private:
			const AppleEvent& theAppleEvent;
			AEKeyword         theKeyword;
		
		public:
			AEGetAttributePtr_Getter( const AppleEvent& appleEvent, AEKeyword keyword )
			: theAppleEvent( appleEvent ),
			  theKeyword   ( keyword    )  {}
			
			std::size_t size() const
			{
				AESizeOfAttribute_Result info = AESizeOfAttribute( theAppleEvent, theKeyword );
				
				if ( DescType_Traits< type >::hasStaticSize )
				{
					return sizeof (typename DescType_Traits< type >::Buffer);
				}
				
				ASSERT( info.typeCode == type );
				
				return info.dataSize;
			}
			
			void operator()( void *begin, void *end ) const
			{
				AEGetAttributePtr( theAppleEvent, theKeyword, type, begin, Detail::Distance( begin, end ) );
			}
	};
	
	/*
	template < ::DescType type >
	typename DescType_Traits< type >::Result
	AEGetAttributePtr( const AppleEvent&  appleEvent,
	                   AEKeyword          keyword )
	{
		return DescType_Traits< type >().Get( AEGetAttributePtr_Getter< DescType_Map_Traits< type >::result >( appleEvent, keyword ) );
	}
	*/
	
	template < AEKeyword key >
	typename AEKeyword_Traits< key >::Result
	AEGetAttributePtr( const AppleEvent& appleEvent )
	{
		return AEKeyword_Traits< key >().Get( AEGetAttributePtr_Getter< AEKeyword_Traits< key >::descType >( appleEvent, key ) );
	}
	
	
	class AEGetDescData_Getter
	{
		private:
			const AEDesc& theDesc;
		
		public:
			AEGetDescData_Getter( const AEDesc& desc = AEInitializeDesc() ) : theDesc( desc )  {}
			
			std::size_t size() const  { return AEGetDescDataSize( theDesc ); }
			
			void operator()( void *begin, void *end ) const
			{
				AEGetDescData( theDesc, begin, Detail::Distance( begin, end ) );
			}
	};
	
	template < ::DescType type >
	typename DescType_Traits< type >::Result AEGetDescData( const AEDesc&  desc,
	                                                        DescType       requiredType = DescType_Map_Traits< type >::result )
	{
		ASSERT( requiredType == typeWildCard || requiredType == desc.descriptorType );
		
		return DescType_Traits< type >().Get( AEGetDescData_Getter( desc ) );
	}
	
	template < ::DescType type >
	typename DescType_Traits< type >::Result AECoerceDescData( const AEDesc& desc )
	{
		return AEGetDescData< type >( AECoerceDesc( desc, DescType_Map_Traits< type >::result ) );
	}
	
	
	class AEReplaceDescData_Putter
	{
		private:
			DescType theType;
			Nucleus::Owned< AEDesc >& theDesc;
		
		public:
			AEReplaceDescData_Putter( DescType                   type,
			                          Nucleus::Owned< AEDesc >&  desc )
			: theType( type ),
			  theDesc( desc )  {}
			
			void operator()( const void *begin, const void *end ) const
			{
				AEReplaceDescData( theType, begin, Detail::Distance( begin, end ), theDesc );
			}
	};
	
	template < ::DescType type >
	void AEReplaceDescData( typename DescType_Traits< type >::Parameter  data,
	                        AEDesc&                                      result )
	{
		DescType_Traits< type >().Put( data,
		                               AEReplaceDescData_Putter( DescType_Map_Traits< type >::result,
		                                                         result ) );
	}
	
	template < ::DescType type, class Disposer >
	void AEReplaceDescData( typename DescType_Traits< type >::Parameter           data,
	                        Nucleus::Owned< AEDesc, Disposer >&                   result )
	{
		DescType_Traits< type >().Put( data,
		                               AEReplaceDescData_Putter( DescType_Map_Traits< type >::result,
		                                                         Detail::AEDescEditor( result ) ) );
	}
	
}

#endif

