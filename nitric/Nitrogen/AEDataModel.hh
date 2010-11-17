// Nitrogen/AEDataModel.hh
// -----------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2009 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_AEDATAMODEL_HH
#define NITROGEN_AEDATAMODEL_HH

#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif
#ifndef __EVENTS__
#include <Events.h>
#endif

#if CALL_NOT_IN_CARBON
#ifndef __EPPC__
#include <EPPC.h>
#endif
#endif

// iota
#include "iota/distance.hh"

// Debug
#include "debug/assert.hh"

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif
#ifndef NUCLEUS_STRING_HH
#include "nucleus/string.hh"
#endif

// Nitrogen
#ifndef MAC_ALIASES_TYPES_ALIASHANDLE_HH
#include "Mac/Aliases/Types/AliasHandle.hh"
#endif
#ifndef MAC_FILES_TYPES_FSCREATOR_HH
#include "Mac/Files/Types/FSCreator.hh"
#endif

#ifndef NITROGEN_AEKEYWORD_HH
#include "Nitrogen/AEKeyword.hh"
#endif
#ifndef NITROGEN_DESCTYPE_HH
#include "Nitrogen/DescType.hh"
#endif

#ifndef NITROGEN_MACMEMORY_HH
#include "Nitrogen/MacMemory.hh"
#endif
#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif
#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif
#ifndef NITROGEN_REFCON_HH
#include "Nitrogen/RefCon.hh"
#endif
#ifndef NITROGEN_UPP_HH
#include "Nitrogen/UPP.hh"
#endif


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
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( AppleEventManager );
	
	#pragma mark -
	#pragma mark ** DescType **
	
	// Constants used creating an AppleEvent
	
	enum AEReturnID
	{
		kAutoGenerateReturnID = ::kAutoGenerateReturnID,
		
		kAEReturnID_Max = nucleus::enumeration_traits< ::AEReturnID >::max
	};
	
	// The OSA runtime can generate 'long' return IDs greater than 0x7fff.
	// AECreateAppleEvent() can only ever accept SInt16, but our specialization
	// of AEGetAttributePtr() needs to use a 32-bit type.
	
	enum AEReturnID_32Bit
	{
		kAEReturnID_32Bit_Max = nucleus::enumeration_traits< ::SInt32 >::max
	};
	
	enum AETransactionID
	{
		kAnyTransactionID = ::kAnyTransactionID,
		
		kAETransactionID_Max = nucleus::enumeration_traits< ::AETransactionID >::max
	};
	
	enum AEEventClass
	{
		kAEEventClass_Max = nucleus::enumeration_traits< ::AEEventClass >::max
	};
	
	enum AEEventID
	{
		kAEEventID_Max = nucleus::enumeration_traits< ::AEEventID >::max
	};
	
	enum AEEnumerated
	{
		kAEEnumerated_Max = nucleus::enumeration_traits< UInt32 >::max
	};
	
	typedef AEEnumerated AEEnumeration;
	typedef AEEnumerated AEKeyForm;
	
	
	enum AESendPriority
	{
		kAENormalPriority = ::kAENormalPriority,
		kAEHighPriority   = ::kAEHighPriority,
		
		kAESendPriority_Max = nucleus::enumeration_traits< ::AESendPriority >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( AESendPriority )
	
	enum AESendMode
	{
		kAENoReply               = ::kAENoReply,
		kAEQueueReply            = ::kAEQueueReply,
		kAEWaitReply             = ::kAEWaitReply,
		kAEDontReconnect         = ::kAEDontReconnect,
		kAEWantReceipt           = ::kAEWantReceipt,
		kAENeverInteract         = ::kAENeverInteract,
		kAECanInteract           = ::kAECanInteract,
		kAEAlwaysInteract        = ::kAEAlwaysInteract,
		kAECanSwitchLayer        = ::kAECanSwitchLayer,
		kAEDontRecord            = ::kAEDontRecord,
		kAEDontExecute           = ::kAEDontExecute,
		kAEProcessNonReplyEvents = ::kAEProcessNonReplyEvents,
		
		kAESendMode_Max = nucleus::enumeration_traits< ::AESendMode >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( AESendMode )
	
	
	#pragma mark -
	#pragma mark ** DescType_Traits **
	
	template < DescType > struct DescType_Traits;
	
	template<> struct DescType_Traits< typeChar > : public nucleus::string_scribe< nucleus::mutable_string > {};
	
	template<> struct DescType_Traits< typeFixed > : public FixedFlattener {};
	
	template<> struct DescType_Traits< typeNull >                   { typedef void Result; };
	
#if CALL_NOT_IN_CARBON
	
	// TargetID is defined for Carbon, but typeTargetID is not.
	template<> struct DescType_Traits< typeTargetID > : nucleus::POD_scribe< TargetID > {};
	
#endif
	
	template<> struct DescType_Traits< typeBoolean >                : BooleanFlattener                     {};
	template<> struct DescType_Traits< typeSInt16 >                 : nucleus::POD_scribe< SInt16 >      {};
	template<> struct DescType_Traits< typeSInt32 >                 : nucleus::POD_scribe< SInt32 >      {};
	template<> struct DescType_Traits< typeUInt32 >                 : nucleus::POD_scribe< UInt32 >      {};
	template<> struct DescType_Traits< typeSInt64 >                 : nucleus::POD_scribe< SInt64 >      {};
	template<> struct DescType_Traits< typeIEEE32BitFloatingPoint > : nucleus::POD_scribe< float >       {};
	template<> struct DescType_Traits< typeIEEE64BitFloatingPoint > : nucleus::POD_scribe< double >      {};
	template<> struct DescType_Traits< type128BitFloatingPoint >    : nucleus::POD_scribe< long double > {};
	
	inline std::size_t SizeOf_AppParameters( const AppParameters& appParameters )
	{
		return sizeof (AppParameters) + appParameters.messageLength;
	}
	
	template<> struct DescType_Traits< typeEventRecord >            : nucleus::POD_scribe< EventRecord >                      {};
	template<> struct DescType_Traits< typeAlias >                  : TypedHandleFlattener< AliasRecord >                       {};
	template<> struct DescType_Traits< typeEnumerated >             : nucleus::converting_POD_scribe< AEEnumerated, UInt32 >   {};
	template<> struct DescType_Traits< typeType >                   : nucleus::converting_POD_scribe< DescType, ::DescType >   {};
	template<> struct DescType_Traits< typeAppParameters >          : nucleus::variable_length_POD_scribe< AppParameters, SizeOf_AppParameters > {};
	template<> struct DescType_Traits< typeFSS >                    : nucleus::POD_scribe< FSSpec >                           {};
	template<> struct DescType_Traits< typeFSRef >                  : nucleus::POD_scribe< FSRef >                            {};
	template<> struct DescType_Traits< typeKeyword >                : nucleus::converting_POD_scribe< AEKeyword, ::AEKeyword > {};
	template<> struct DescType_Traits< typeApplSignature >          : nucleus::converting_POD_scribe< Mac::FSCreator, ::OSType > {};
	template<> struct DescType_Traits< typeQDRectangle >            : nucleus::POD_scribe< Rect >                             {};
	template<> struct DescType_Traits< typeProcessSerialNumber >    : nucleus::POD_scribe< ProcessSerialNumber >              {};
	template<> struct DescType_Traits< typeApplicationURL >         : DescType_Traits< typeChar >                               {};
	
	
	template < DescType type >
	struct DescType_Map_Traits
	{
		static const DescType result = type;
	};
	
	template < class Char > struct Char_DescType_Traits;
	
	template <> struct Char_DescType_Traits< char >  { static const DescType descType = typeChar; };
	
	template < class Integer > struct Integer_DescType_Traits;
	
	template <> struct Integer_DescType_Traits< SInt16 >  { static const DescType descType = typeSInt16; };
	template <> struct Integer_DescType_Traits< SInt32 >  { static const DescType descType = typeSInt32; };
	template <> struct Integer_DescType_Traits< UInt32 >  { static const DescType descType = typeUInt32; };
	
	struct Type_DescType_Traits  { static const DescType descType = typeType;       };
	struct Enum_DescType_Traits  { static const DescType descType = typeEnumerated; };
	
	
	template < class Char > struct Char_AEKeyword_Traits;
	
	/*
	template < class Char >
	struct Char_AEKeyword_Traits : nucleus::string_scribe< std::basic_string< Char > >,
	                               Char_DescType_Traits< Char >
	{
	};
	*/
	
	template <>
	struct Char_AEKeyword_Traits< char > : nucleus::string_scribe< nucleus::mutable_string >,
	                                       Char_DescType_Traits< char >
	{
	};
	
	
	template < class POD, class Integer >
	struct Integer_AEKeyword_Traits : nucleus::converting_POD_scribe< POD, Integer >,
	                                  Integer_DescType_Traits< Integer >
	{
	};
	
	template < class POD >
	struct Type_AEKeyword_Traits : nucleus::converting_POD_scribe< POD, ::FourCharCode >,
	                               Type_DescType_Traits
	{
	};
	
	template < class POD >
	struct Enum_AEKeyword_Traits : nucleus::converting_POD_scribe< POD, ::FourCharCode >,
	                               Enum_DescType_Traits
	{
	};
	
	template < AEKeyword key > struct AEKeyword_Traits;
	
	template <> struct AEKeyword_Traits< keyTransactionIDAttr > : Integer_AEKeyword_Traits< AETransactionID,  ::AETransactionID > {};
	template <> struct AEKeyword_Traits< keyReturnIDAttr      > : Integer_AEKeyword_Traits< AEReturnID_32Bit, ::SInt32          > {};
	
	template <> struct AEKeyword_Traits< keyEventClassAttr > : Type_AEKeyword_Traits< AEEventClass > {};
	template <> struct AEKeyword_Traits< keyEventIDAttr    > : Type_AEKeyword_Traits< AEEventID    > {};
	
	// In AppleEvents.h due to dependency on enum AEEventSource.
	//template <> struct AEKeyword_Traits< keyEventSourceAttr > : Integer_AEKeyword_Traits< AEEventSource, SInt16 > {};
	
	template <> struct AEKeyword_Traits< keyMissedKeywordAttr > : Type_AEKeyword_Traits< AEKeyword > {};
	
	
	#pragma mark -
	#pragma mark ** AEDesc **
	
	using ::AEDesc;
	using ::AEDescList;
	using ::AERecord;
	using ::AEKeyDesc;
	
	inline void Initialize_AEDesc( AEDesc& desc )
	{
		desc.descriptorType = typeNull;
		desc.dataHandle     = NULL;
	}
	
	inline AEDesc Initialized_AEDesc()
	{
		AEDesc result;
		
		Initialize_AEDesc( result );
		
		return result;
	}
	
	class AEDesc_AlivenessTest
	{
		public:
			static bool IsLive( const AEDesc& desc )
			{
				return desc.dataHandle != NULL;
			}
			
			static bool IsLive( const AEKeyDesc& keyDesc )
			{
				return IsLive( keyDesc.descContent );
			}
	};
	
	class NonNull_AEDescs_Are_Live
	{
		public:
			static bool is_live( const AEDesc& desc )
			{
				return desc.dataHandle != NULL;
			}
	};
	
	struct AEDesc_Data : AEDesc
	{
	};
	
	typedef AEDesc_Data AEDescList_Data, AERecord_Data;
	typedef AEDesc_Data AEAddressDesc, AppleEvent;
	typedef AEDesc_Data AEDesc_ObjectSpecifier;
	
	struct AEDesc_Token : AEDesc
	{
	};
	
	typedef AEDesc_Token AEDescList_Token, AERecord_Token;
	
	inline bool operator==( const AEDesc& a, const AEDesc& b )
	{
		return    a.descriptorType == b.descriptorType
		       && a.dataHandle     == b.dataHandle;
	}
	
	inline bool operator!=( const AEDesc& a, const AEDesc& b )
	{
		return !( a == b );
	}
	
	template < class AEDesc_Type >
	struct Qualified_AEDesc_Traits
	{
		typedef AEDesc Type;
	};
	
	template < class AEDesc_Type >
	struct Qualified_AEDesc_Traits< const AEDesc_Type >
	{
		typedef const AEDesc Type;
	};
	
	template < class AEDesc_Type, class InputType >
	inline AEDesc_Type& AEDesc_Cast( InputType& desc )
	{
		typedef typename Qualified_AEDesc_Traits< AEDesc_Type >::Type Base;
		
		return static_cast< AEDesc_Type& >( static_cast< Base& >( desc ) );
	}
	
}

namespace nucleus
{
	
	template <>
	struct maker< AEDesc >
	{
		AEDesc operator()() const
		{
			AEDesc result = { typeNull, NULL };
			
			return result;
		}
	};
	
	template <>
	struct maker< Nitrogen::AEDesc_Data >
	{
		Nitrogen::AEDesc_Data operator()() const
		{
			Nitrogen::AEDesc_Data result;
			
			static_cast< AEDesc& >( result ) = make< AEDesc >();
			
			return result;
		}
	};
	
	template <>
	struct maker< Nitrogen::AEDesc_Token >
	{
		Nitrogen::AEDesc_Token operator()() const
		{
			Nitrogen::AEDesc_Token result;
			
			static_cast< AEDesc& >( result ) = make< AEDesc >();
			
			return result;
		}
	};
	
	template <>
	struct disposer< Nitrogen::AEDesc_Data >
	{
		typedef AEDesc  argument_type;
		typedef void    result_type;
		
		// parameter can't be const
		void operator()( AEDesc desc ) const
		{
			// AEDisposeDesc() is documented as only ever returning noErr,
			// but we check anyway to be future-proof.
			
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::AppleEventManager );
			
			::Nitrogen::HandleDestructionOSStatus( ::AEDisposeDesc( &desc ) );
		}
	};
	
	template <>
	struct aliveness_traits< Nitrogen::AEDesc_Data, disposer< Nitrogen::AEDesc_Data > >
	{
		typedef Nitrogen::NonNull_AEDescs_Are_Live aliveness_test;
	};
	
	template <>
	struct default_value_traits< Nitrogen::AEDesc_Data >
	{
		typedef Nitrogen::AEDesc_Data Resource;
		
		static Resource value()
		{
			Resource r;
			
			Nitrogen::Initialize_AEDesc( r );
			
			return r;
		}
	};
	
	template <>
	struct disposer< Nitrogen::AEDesc_Token >
	{
		typedef AEDesc  argument_type;
		typedef void    result_type;
		
		// parameter can't be const
		void operator()( AEDesc desc ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::AppleEventManager );
			
			::Nitrogen::HandleDestructionOSStatus( ::AEDisposeToken( &desc ) );
		}
	};
	
	template <>
	struct aliveness_traits< Nitrogen::AEDesc_Token, disposer< Nitrogen::AEDesc_Token > >
	{
		typedef Nitrogen::NonNull_AEDescs_Are_Live aliveness_test;
	};
	
	template <>
	struct default_value_traits< Nitrogen::AEDesc_Token >
	{
		typedef Nitrogen::AEDesc_Token Resource;
		
		static Resource value()
		{
			Resource r;
			
			Nitrogen::Initialize_AEDesc( r );
			
			return r;
		}
	};
	
	template <>
	struct disposer< AEKeyDesc >
	{
		typedef AEKeyDesc  argument_type;
		typedef void       result_type;
		
		// parameter can't be const
		void operator()( AEKeyDesc keyDesc ) const
		{
			disposer< Nitrogen::AEDesc_Data >()( keyDesc.descContent );
		}
	};
	
	template <>
	struct default_value_traits< AEKeyDesc >
	{
		typedef AEKeyDesc Resource;
		
		static Resource value()
		{
			Resource r;
			
			Nitrogen::Initialize_AEDesc( r.descContent );
			
			return r;
		}
	};
	
	template <>
	struct aliveness_traits< AEKeyDesc, disposer< AEKeyDesc > >
	{
		typedef Nitrogen::NonNull_AEDescs_Are_Live aliveness_test;
	};
	
}

namespace nucleus
{
	
	template <>
	struct maker< AEKeyDesc >
	{
		AEKeyDesc operator()() const
		{
			AEKeyDesc result = { Nitrogen::AEKeyword(), make< AEDesc >() };
			
			return result;
		}
	};
	
}

namespace Nitrogen
{
	
	#pragma mark -
	#pragma mark ** Coercions **
	
	struct AECoerceDescUPP_Details: Basic_UPP_Details< ::AECoerceDescUPP,
	                                                   ::AECoerceDescProcPtr,
	                                                   ::NewAECoerceDescUPP,
	                                                   ::DisposeAECoerceDescUPP,
	                                                   ::InvokeAECoerceDescUPP >
	{
	};
	
	struct AECoercePtrUPP_Details: Basic_UPP_Details< ::AECoercePtrUPP,
	                                                  ::AECoercePtrProcPtr,
	                                                  ::NewAECoercePtrUPP,
	                                                  ::DisposeAECoercePtrUPP,
	                                                  ::InvokeAECoercePtrUPP >
	{
	};
	
	typedef UPP< AECoerceDescUPP_Details > AECoerceDescUPP;
	typedef UPP< AECoercePtrUPP_Details  > AECoercePtrUPP;
	
	inline nucleus::owned< AECoerceDescUPP > NewAECoerceDescUPP( ::AECoerceDescProcPtr p )
	{
		return NewUPP< AECoerceDescUPP >( p );
	}
	
	inline nucleus::owned< AECoercePtrUPP > NewAECoercePtrUPP( ::AECoercePtrProcPtr p )
	{
		return NewUPP< AECoercePtrUPP >( p );
	}
	
	inline void DisposeAECoerceDescUPP( nucleus::owned< AECoerceDescUPP > )  {}
	inline void DisposeAECoercePtrUPP ( nucleus::owned< AECoercePtrUPP  > )  {}
	
	inline void InvokeAECoerceDescUPP( const AEDesc_Data&  fromDesc,
	                                   DescType            toType,
	                                   RefCon              handlerRefCon,
	                                   AEDesc_Data&        toDesc,
	                                   AECoerceDescUPP     userUPP )
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
	                                  AEDesc_Data&    toDesc,
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
		
		AECoercionHandlerUPP( ::AECoercePtrUPP p )
		:
			AECoerceDescUPP( reinterpret_cast< ::AECoerceDescUPP >( p ) )
		{
		}
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
		{
		}
		
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
		{
		}
		
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
		{
		}
	};
	
	bool operator==( const AECoercionHandler& a, const AECoercionHandler& b );
	
	inline bool operator!=( const AECoercionHandler& a, const AECoercionHandler& b )
	{
		return !( a == b );
	}
	
}

namespace nucleus
{
	
	template <>
	struct disposer< Nitrogen::AECoercionHandler >
	{
		typedef Nitrogen::AECoercionHandler  argument_type;
		typedef void                         result_type;
		
		void operator()( const Nitrogen::AECoercionHandler& installation ) const
		{
			::Nitrogen::HandleDestructionOSStatus( ::AERemoveCoercionHandler( installation.fromType,
			                                                                  installation.toType,
			                                                                  installation.handler,
			                                                                  installation.isSysHandler ) );
		}
	};
	
	template < class Disposer >
	struct aliveness_traits< ::Nitrogen::AECoercionHandler, Disposer >
	{
		struct aliveness_test
		{
			static bool is_live( const ::Nitrogen::AECoercionHandler& coercion )
			{
				return coercion.handler != 0;
			}
		};
	};
	
}

namespace Nitrogen
{
	
	typedef nucleus::owned< AEDesc_Data > ( *AECoerceDescProcPtr )( const AEDesc_Data&  fromDesc,
	                                                                DescType            toType,
	                                                                RefCon              refCon );
	
	typedef nucleus::owned< AEDesc_Data > ( *AECoercePtrProcPtr )( DescType     typeCode,
	                                                               const void*  dataPtr,
	                                                               std::size_t  datasize,
	                                                               DescType     toType,
	                                                               RefCon       refCon );
	
	template < AECoerceDescProcPtr handler >
	struct AECoercionHandler_Desc_Callback
	{
		static pascal OSErr Adapter( const AEDesc*  fromDesc,
		                             ::DescType     toType,
		                             long           refCon,
		                             AEDesc*        result )
		{
			try
			{
				*result = handler( static_cast< const AEDesc_Data& >( *fromDesc ),
				                   DescType( toType ),
				                   refCon ).release();
			}
			catch ( ... )
			{
				return ConvertTheExceptionToOSStatus( errAEEventFailed );
			}
			
			return noErr;
		}
	};
	
	template < AECoercePtrProcPtr handler >
	struct AECoercionHandler_Ptr_Callback
	{
		static pascal OSErr Adapter( ::DescType   typeCode,
		                             const void*  dataPtr,
		                             ::Size       dataSize,
		                             ::DescType   toType,
		                             long         refCon,
		                             AEDesc*      result )
		{
			try
			{
				*result = handler( DescType( typeCode ),
				                   dataPtr,
				                   dataSize,
				                   DescType( toType ),
				                   refCon ).release();
			}
			catch ( ... )
			{
				return ConvertTheExceptionToOSStatus( errAEEventFailed );
			}
			
			return noErr;
		}
	};
	
	// 388
	nucleus::owned< AECoercionHandler > AEInstallCoercionHandler( const AECoercionHandler& toInstall );
	
	inline nucleus::owned< AECoercionHandler > AEInstallCoercionHandler( DescType fromType,
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
	
	inline nucleus::owned< AECoercionHandler > AEInstallCoercionHandler( DescType        fromType,
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
	nucleus::owned< AECoercionHandler > AEInstallCoercionHandler( DescType  fromType,
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
	nucleus::owned< AECoercionHandler > AEInstallCoercionHandler( DescType  fromType,
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
	nucleus::owned< AECoercionHandler > AEInstallCoercionHandler( DescType  fromType,
	                                                              DescType  toType,
	                                                              RefCon    handlerRefCon = RefCon(),
	                                                              Boolean   isSysHandler  = false )
	{
		return AEInstallCoercionHandler< AECoercionHandler_Desc_Callback< handler >::Adapter >( fromType,
		                                                                                        toType,
		                                                                                        handlerRefCon,
		                                                                                        isSysHandler );
	}
	
	template < AECoercePtrProcPtr handler >
	nucleus::owned< AECoercionHandler > AEInstallCoercionHandler( DescType  fromType,
	                                                              DescType  toType,
	                                                              RefCon    handlerRefCon = RefCon(),
	                                                              Boolean   isSysHandler  = false )
	{
		return AEInstallCoercionHandler< AECoercionHandler_Ptr_Callback< handler >::Adapter >( fromType,
		                                                                                       toType,
		                                                                                       handlerRefCon,
		                                                                                       isSysHandler );
	}
	
	// 406
	inline void AERemoveCoercionHandler( nucleus::owned< AECoercionHandler > )  {}
	
	typedef AECoercionHandler AEGetCoercionHandler_Result;
	
	AECoercionHandler AEGetCoercionHandler( DescType  fromType,
	                                        DescType  toType,
	                                        bool      isSysHandler );
	
	// 444
	nucleus::owned< AEDesc_Data > AECoercePtr( DescType     typeCode,
	                                           const void*  dataPtr,
	                                           Size         dataSize,
	                                           DescType     toType );
	
	// 461
	nucleus::owned< AEDesc_Data > AECoerceDesc( const AEDesc& desc, DescType toType );
	
	#pragma mark -
	#pragma mark ** AEDescs **
	
	namespace Detail
	{
		
		template < class AEDesc_Type >
		class AEDescEditor
		{
			private:
				nucleus::owned< AEDesc_Type >& itsDesc;
				AEDesc_Type                    itsWorkingCopy;
			
			public:
				AEDescEditor( nucleus::owned< AEDesc_Type >& desc )
				:
					itsDesc       ( desc           ), 
					itsWorkingCopy( desc.release() )
				{
				}
				
				~AEDescEditor()  { itsDesc = nucleus::owned< AEDesc_Type >::seize( itsWorkingCopy ); }
				
				AEDesc_Type& Get()       { return itsWorkingCopy; }
				operator AEDesc_Type&()  { return Get();          }
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
	inline AEDesc AEInitializeDesc()
	{
		return nucleus::make< AEDesc >();
	}
	
	template < class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AEInitializeDesc()
	{
		// AEDesc_Type must be a subclass of AEDesc
		(void) static_cast< const AEDesc& >( AEDesc_Type() );
		
		AEDesc desc = AEInitializeDesc();
		
		return nucleus::owned< AEDesc_Type >::seize( AEDesc_Cast< AEDesc_Type >( desc ) );
	}
	
	namespace Detail
	{
		
		AEDesc AECreateDesc_Unowned( DescType     typeCode,
		                             const void*  dataPtr,
		                             Size         dataSize );
		
		AEDesc AECreateDesc_Unowned( DescType typeCode, Handle handle );
		
		AEDesc AECreateDesc_Unowned( DescType typeCode, nucleus::owned< Handle > handle );
		
		AEDesc AEDuplicateDesc_Unowned( const AEDesc& desc );
		
		AEDesc AECreateList_Unowned( const void*  factoringPtr,
		                             std::size_t  factoredSize,
		                             bool         isRecord );
		
		AEDesc AEGetNthDesc_Unowned( const AEDesc&  listDesc,
		                             long           index,
		                             DescType       desiredType,
		                             ::AEKeyword*   keyword );
		
		AEDesc AEGetKeyDesc_Unowned( const AERecord&  record,
		                             AEKeyword        keyword,
		                             DescType         desiredType );
		
	}
	
	template < class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateDesc( DescType     typeCode,
	                                                   const void*  dataPtr,
	                                                   Size         dataSize )
	{
		// AEDesc_Type must be a subclass of AEDesc
		(void) static_cast< const AEDesc& >( AEDesc_Type() );
		
		AEDesc desc = Detail::AECreateDesc_Unowned( typeCode, dataPtr, dataSize );
		
		return nucleus::owned< AEDesc_Type >::seize( AEDesc_Cast< AEDesc_Type >( desc ) );
	}
	
	template < class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateDesc( DescType  typeCode,
	                                                   Handle    handle )
	{
		// AEDesc_Type must be a subclass of AEDesc
		(void) static_cast< const AEDesc& >( AEDesc_Type() );
		
		AEDesc desc = Detail::AECreateDesc_Unowned( typeCode, handle );
		
		return nucleus::owned< AEDesc_Type >::seize( AEDesc_Cast< AEDesc_Type >( desc ) );
	}
	
	template < class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateDesc( DescType                  typeCode,
	                                                   nucleus::owned< Handle >  handle )
	{
		// AEDesc_Type must be a subclass of AEDesc
		(void) static_cast< const AEDesc& >( AEDesc_Type() );
		
		AEDesc desc = Detail::AECreateDesc_Unowned( typeCode, handle );
		
		return nucleus::owned< AEDesc_Type >::seize( AEDesc_Cast< AEDesc_Type >( desc ) );
	}
	
	template < class T, class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateDesc( DescType typeCode, T** handle )
	{
		return AECreateDesc< AEDesc_Type >( typeCode, Handle( handle ) );
	}
	
	template < class T, class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateDesc( DescType                                            typeCode,
	                                                   nucleus::owned< T**, nucleus::disposer< Handle > >  handle )
	{
		return AECreateDesc< AEDesc_Type >( typeCode, nucleus::owned< Handle >( handle ) );
	}
	
	template < class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateDesc( DescType typeCode, nucleus::owned< AEDesc_Type > desc )
	{
		Detail::AEDescEditor< AEDesc_Type >( desc ).Get().descriptorType = typeCode;
		
		return desc;
	}
	
	template < class T >
	inline nucleus::owned< AEDesc_Data > AECreateDesc( DescType typeCode, T** handle )
	{
		return AECreateDesc< T, AEDesc_Data >( typeCode, handle );
	}
	
	template < class T >
	inline nucleus::owned< AEDesc_Data > AECreateDesc( DescType                                            typeCode,
	                                                   nucleus::owned< T**, nucleus::disposer< Handle > >  handle )
	{
		return AECreateDesc< T, AEDesc_Data >( typeCode, handle );
	}
	
	namespace MetrowerksHack
	{
		
		// CW Pro 6 complains of illegal function overloading without this namespace
		
		inline nucleus::owned< AEDesc_Data > AECreateDesc( DescType     typeCode,
		                                                   const void*  dataPtr,
		                                                   Size         dataSize )
		{
			return Nitrogen::AECreateDesc< AEDesc_Data >( typeCode, dataPtr, dataSize );
		}
		
		inline nucleus::owned< AEDesc_Data > AECreateDesc( DescType  typeCode,
		                                                   Handle    handle )
		{
			return Nitrogen::AECreateDesc< AEDesc_Data >( typeCode, handle );
		}
		
		inline nucleus::owned< AEDesc_Data > AECreateDesc( DescType                  typeCode,
		                                                   nucleus::owned< Handle >  handle )
		{
			return Nitrogen::AECreateDesc< AEDesc_Data >( typeCode, handle );
		}
		
	}
	
	using namespace MetrowerksHack;
	
	
	inline void AEDisposeDesc( nucleus::owned< AEDesc_Data > )
	{
	}
	
	inline nucleus::owned< AEDesc_Data > AEDuplicateDesc( const AEDesc& original )
	{
		AEDesc desc = Detail::AEDuplicateDesc_Unowned( original );
		
		return nucleus::owned< AEDesc_Data >::seize( static_cast< const AEDesc_Data& >( desc ) );
	}
	
	#pragma mark -
	#pragma mark ** AEDescLists **
	
	template < class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateList( const void*  factoringPtr,
	                                                   std::size_t  factoredSize,
	                                                   bool         isRecord )
	{
		// Generally speaking we won't use factoring for tokens, but since the
		// unfactored version calls this one it's simpler to be general.
		
		AEDesc desc = Detail::AECreateList_Unowned( factoringPtr, factoredSize, isRecord );
		
		return nucleus::owned< AEDesc_Type >::seize( AEDesc_Cast< AEDesc_Type >( desc ) );
	}
	
	template < class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateList( bool isRecord = false )
	{
		return AECreateList< AEDesc_Type >( NULL, 0, isRecord );
	}
	
	template < class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateList( DescType typeCode, bool isRecord )
	{
		return AECreateDesc( typeCode, AECreateList< AEDesc_Type >( isRecord ) );
	}
	
	namespace MetrowerksHack
	{
		
		// CW Pro 6 complains of illegal function overloading without this namespace
		
		inline nucleus::owned< AEDesc_Data > AECreateList( const void*  factoringPtr,
		                                                   std::size_t  factoredSize,
		                                                   bool         isRecord )
		{
			return Nitrogen::AECreateList< AEDesc_Data >( factoringPtr, factoredSize, isRecord );
		}
		
		inline nucleus::owned< AEDesc_Data > AECreateList( bool isRecord = false )
		{
			return Nitrogen::AECreateList< AEDesc_Data >( isRecord );
		}
		
		inline nucleus::owned< AEDesc_Data > AECreateList( DescType typeCode, bool isRecord )
		{
			return Nitrogen::AECreateList< AEDesc_Data >( typeCode, isRecord );
		}
		
	}
	
	using namespace MetrowerksHack;
	
	long AECountItems( const AEDesc& desc );
	
	void AEPutPtr( AEDescList&  list,
	               long         index,
	               DescType     type,
	               const void*  dataPtr,
	               Size         dataSize );
	
	inline void AEPutPtr( nucleus::owned< AEDescList_Data >&  list,
	                      long                                index,
	                      DescType                            type,
	                      const void*                         dataPtr,
	                      Size                                dataSize )
	{
		AEPutPtr( Detail::AEDescEditor< AEDescList_Data >( list ),
		          index,
		          type,
		          dataPtr,
		          dataSize );
	}
	
	void AEPutDesc( AEDescList&    list,
	                long           index,
	                const AEDesc&  desc );
	
	inline void AEPutDesc( nucleus::owned< AEDescList_Data >&  list,
	                       long                                index,
	                       const AEDesc_Data&                  desc )
	{
		AEPutDesc( Detail::AEDescEditor< AEDescList_Data >( list ), index, desc );
	}
	
	inline void AEPutDesc( nucleus::owned< AEDescList_Token >&  list,
	                       long                                 index,
	                       nucleus::owned< AEDesc_Token >       desc )
	{
		// This is the only variant of AEPutDesc() supported for tokens.
		// The desc argument must be an Owned token to ensure that its tokenness
		// is managed correctly (i.e. disposed exactly once).
		// Since a token list's token disposal function calls AEDisposeToken()
		// on each member token, we must not do so here.  But we do need to
		// dispose the AEDesc.  Got it?
		
		AEPutDesc( AEDesc_Cast< AEDescList_Data >( Detail::AEDescEditor< AEDescList_Token >( list ).Get() ),
		           index,
		           AEDesc_Cast< const AEDesc_Data >( desc.get() ) );
		
		nucleus::disposer< AEDesc_Data >()( desc.release() );
	}
	
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
	
	inline nucleus::owned< AEDesc_Data > AEGetNthDesc( const AEDesc&  listDesc,
	                                                   long           index,
	                                                   DescType       desiredType = typeWildCard,
	                                                   ::AEKeyword*   keyword     = NULL )
	{
		AEDesc desc = Detail::AEGetNthDesc_Unowned( listDesc, index, desiredType, keyword );
		
		return nucleus::owned< AEDesc_Data >::seize( static_cast< const AEDesc_Data& >( desc ) );
	}
	
	inline nucleus::owned< AEDesc_Data > AEGetNthDesc( const AEDesc&  listDesc,
	                                                   long           index,
	                                                   ::AEKeyword*   keyword )
	{
		return AEGetNthDesc( listDesc, index, typeWildCard, keyword );
	}
	
	AESizeOfNthItem_Result AESizeOfNthItem( const AEDescList& list, long index );
	
	// If you have a reason for deleting an item in a token list, please let me know
	
	void AEDeleteItem(                 AEDescList_Data  & listDesc, long index );
	void AEDeleteItem( nucleus::owned< AEDescList_Data >& listDesc, long index );
	
	#pragma mark -
	#pragma mark ** AERecords **
	
	inline bool AECheckIsRecord( const AEDesc& theDesc )
	{
		return ::AECheckIsRecord( &theDesc );
	}
	
	void AEPutKeyPtr( AERecord&    record,
	                  AEKeyword    keyword,
	                  DescType     typeCode,
	                  const void*  dataPtr,
	                  std::size_t  dataSize );
	
	inline void AEPutKeyPtr( nucleus::owned< AERecord_Data >&  record,
	                         AEKeyword                         keyword,
	                         DescType                          typeCode,
	                         const void*                       dataPtr,
	                         std::size_t                       dataSize )
	{
		AEPutKeyPtr( Detail::AEDescEditor< AERecord_Data >( record ),
		             keyword,
		             typeCode,
		             dataPtr,
		             dataSize );
	}
	
	void AEPutKeyDesc( AERecord&      record,
	                   AEKeyword      keyword,
	                   const AEDesc&  desc );
	
	void AEPutKeyDesc( AERecord&         record,
	                   const AEKeyDesc&  keyDesc );
	
	inline void AEPutKeyDesc( nucleus::owned< AERecord_Data >&  record,
	                          AEKeyword                         keyword,
	                          const AEDesc_Data&                desc )
	{
		AEPutKeyDesc( Detail::AEDescEditor< AERecord_Data >( record ), keyword, desc );
	}
	
	inline void AEPutKeyDesc( nucleus::owned< AERecord_Token >&  record,
	                          AEKeyword                          keyword,
	                          nucleus::owned< AEDesc_Token >     token )
	{
		AEPutKeyDesc( AEDesc_Cast< AERecord_Data >( Detail::AEDescEditor< AERecord_Token >( record ).Get() ),
		              keyword,
		              AEDesc_Cast< const AEDesc_Data >( token ) );
		
		nucleus::disposer< AEDesc_Data >()( token.release() );
	}
	
	inline void AEPutKeyDesc( nucleus::owned< AERecord_Data >&  record,
	                          const AEKeyDesc&                  keyDesc )
	{
		AEPutKeyDesc( Detail::AEDescEditor< AERecord_Data >( record ), keyDesc );
	}
	
	class AEPutKeyDesc_Binding
	{
		private:
			AEKeyword      itsKey;
			const AEDesc&  itsData;
		
		public:
			AEPutKeyDesc_Binding( AEKeyword key, const AEDesc& data ) : itsKey( key ), itsData( data )  {}
			
			AEKeyword Key() const  { return itsKey; }
			
			const AEDesc& Data() const  { return itsData; }
	};
	
	inline AEPutKeyDesc_Binding AEPutKeyDesc( AEKeyword key, const AEDesc& data )
	{
		return AEPutKeyDesc_Binding( key, data );
	}
	
	AEGetKeyPtr_Result AEGetKeyPtr( const AERecord&  record,
	                                AEKeyword        keyword,
	                                DescType         desiredType,
	                                void*            dataPtr,
	                                std::size_t      maximumSize );
	
	inline nucleus::owned< AEDesc_Data > AEGetKeyDesc( const AERecord&  record,
	                                                   AEKeyword        keyword,
	                                                   DescType         desiredType = typeWildCard )
	{
		AEDesc desc = Detail::AEGetKeyDesc_Unowned( record, keyword, desiredType );
		
		return nucleus::owned< AEDesc_Data >::seize( AEDesc_Cast< AEDesc_Data >( desc ) );
	}
	
	AESizeOfKeyDesc_Result AESizeOfKeyDesc( const AERecord&  record,
	                                        AEKeyword        keyword );
	
	void AEDeleteKeyDesc( AERecord& record, AEKeyword keyword );
	
	inline void AEDeleteKeyDesc( nucleus::owned< AERecord_Data >& record, AEKeyword keyword )
	{
		AEDeleteKeyDesc( Detail::AEDescEditor< AERecord_Data >( record ), keyword );
	}
	
	#pragma mark -
	#pragma mark ** AppleEvents **
	
	nucleus::owned< AppleEvent > AECreateAppleEvent( AEEventClass          eventClass,
	                                                 AEEventID             eventID,
	                                                 const AEAddressDesc&  target,
	                                                 AEReturnID            returnID      = AEReturnID(),
	                                                 AETransactionID       transactionID = AETransactionID() );
	
	void AEPutParamPtr( AppleEvent&  appleEvent,
	                    AEKeyword    keyword,
	                    DescType     typeCode,
	                    const void*  dataPtr,
	                    std::size_t  dataSize );
	
	void AEPutParamPtr( nucleus::owned< AppleEvent >&  appleEvent,
	                    AEKeyword                      keyword,
	                    DescType                       typeCode,
	                    const void*                    dataPtr,
	                    std::size_t                    dataSize );
	
	void AEPutParamDesc( AppleEvent&    appleEvent,
	                     AEKeyword      keyword,
	                     const AEDesc&  desc );
	
	void AEPutParamDesc( nucleus::owned< AppleEvent >&  appleEvent,
	                     AEKeyword                      keyword,
	                     const AEDesc&                  desc );
	
	AEGetParamPtr_Result AEGetParamPtr( const AppleEvent&  appleEvent,
	                                    AEKeyword          keyword,
	                                    DescType           desiredType,
	                                    void*              dataPtr,
	                                    std::size_t        maximumSize );
	
	nucleus::owned< AEDesc_Data > AEGetParamDesc( const AppleEvent&  appleEvent,
	                                              AEKeyword          keyword,
	                                              DescType           desiredType = typeWildCard );
	
	AESizeOfParam_Result AESizeOfParam( const AppleEvent&  appleEvent,
	                                    AEKeyword          keyword );
	
	void AEDeleteParam(                 AppleEvent  & appleEvent, AEKeyword keyword );
	void AEDeleteParam( nucleus::owned< AppleEvent >& appleEvent, AEKeyword keyword );
	
	void AEPutAttributePtr( AppleEvent&  appleEvent,
	                        AEKeyword    keyword,
	                        DescType     typeCode,
	                        const void*  dataPtr,
	                        std::size_t  dataSize );
	
	void AEPutAttributePtr( nucleus::owned< AppleEvent >&  appleEvent,
	                        AEKeyword                      keyword,
	                        DescType                       typeCode,
	                        const void*                    dataPtr,
	                        std::size_t                    dataSize );
	
	void AEPutAttributeDesc( AppleEvent&    appleEvent,
	                         AEKeyword      keyword,
	                         const AEDesc&  desc );
	
	void AEPutAttributeDesc( nucleus::owned< AppleEvent >&  appleEvent,
	                         AEKeyword                      keyword,
	                         const AEDesc&                  desc );
	
	AEGetAttributePtr_Result AEGetAttributePtr( const AppleEvent&  appleEvent,
	                                            AEKeyword          keyword,
	                                            DescType           desiredType,
	                                            void*              dataPtr,
	                                            std::size_t        maximumSize );
	
	nucleus::owned< AEDesc_Data > AEGetAttributeDesc( const AppleEvent&  appleEvent,
	                                                  AEKeyword          keyword,
	                                                  DescType           desiredType = typeWildCard );
	
	AESizeOfAttribute_Result AESizeOfAttribute( const AppleEvent&  appleEvent,
	                                            AEKeyword          keyword );
	
	#pragma mark -
	#pragma mark ** Opaque data **
	
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
	
	void AEReplaceDescData( DescType                        typeCode,
	                        const void*                     dataPtr,
	                        std::size_t                     dataSize,
	                        nucleus::owned< AEDesc_Data >&  result );
	
	
	#pragma mark -
	#pragma mark ** AEEventHandlerUPP **
	
	struct AEEventHandlerUPP_Details: Basic_UPP_Details< ::AEEventHandlerUPP,
	                                                     ::AEEventHandlerProcPtr,
	                                                     ::NewAEEventHandlerUPP,
	                                                     ::DisposeAEEventHandlerUPP,
	                                                     ::InvokeAEEventHandlerUPP >
	{
	};
	
	typedef UPP< AEEventHandlerUPP_Details > AEEventHandlerUPP;
	
	//using ::AEEventHandlerProcPtr;
	
	inline nucleus::owned< AEEventHandlerUPP > NewAEEventHandlerUPP( ::AEEventHandlerProcPtr p )
	{
		return NewUPP< AEEventHandlerUPP >( p );
	}
	
	inline void DisposeAEEventHandlerUPP( nucleus::owned< AEEventHandlerUPP > )
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
	#pragma mark ** Powered by DescType_Traits **
	
	class AECoercePtr_Putter
	{
		private:
			DescType fromType;
			DescType toType;
			nucleus::owned< AEDesc_Data >& theDesc;
		
		public:
			AECoercePtr_Putter( DescType                        from,
			                    DescType                        to,
			                    nucleus::owned< AEDesc_Data >&  desc ) : fromType( from ),
			                                                             toType  ( to   ),
			                                                             theDesc ( desc )
			{
			}
			
			void operator()( const void *begin, const void *end ) const
			{
				theDesc = AECoercePtr( fromType, begin, iota::distance( begin, end ), toType );
			}
	};
	
	template < DescType type >
	inline nucleus::owned< AEDesc_Data > AECoercePtr( typename DescType_Traits< type >::Parameter  data,
	                                                  DescType                                     toType )
	{
		nucleus::owned< AEDesc_Data > result;
		
		DescType_Traits< type >::Put( data,
		                              AECoercePtr_Putter( DescType_Map_Traits< type >::result,
		                                                  toType,
		                                                  result ) );
		
		return result;
	}
	
	
	template < class AEDesc_Type >
	class AECreateDesc_Putter
	{
		private:
			DescType                        theType;
			nucleus::owned< AEDesc_Type >&  theDesc;
		
		public:
			AECreateDesc_Putter( DescType                        type,
			                     nucleus::owned< AEDesc_Type >&  desc ) : theType( type ),
			                                                              theDesc( desc )
			{
			}
			
			void operator()( const void *begin, const void *end ) const
			{
				theDesc = AECreateDesc< AEDesc_Type >( theType, begin, iota::distance( begin, end ) );
			}
	};
	
	template < DescType type, class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateDesc( typename DescType_Traits< type >::Parameter  data )
	{
		nucleus::owned< AEDesc_Type > result;
		
		DescType_Traits< type >::Put( data,
		                              AECreateDesc_Putter< AEDesc_Type >( DescType_Map_Traits< type >::result,
		                                                                   result ) );
		
		return result;
	}
	
	template < DescType type >
	inline nucleus::owned< AEDesc_Data > AECreateDesc( typename DescType_Traits< type >::Parameter  data )
	{
		return AECreateDesc< type, AEDesc_Data >( data );
	}
	
	
	class AEPutPtr_Putter
	{
		private:
			AEDescList&  theList;
			long         theIndex;
			DescType     theType;
		
		public:
			AEPutPtr_Putter( AEDescList&  list,
			                 long         index,
			                 DescType     type ) : theList ( list  ),
			                                       theIndex( index ),
			                                       theType ( type  )
			{
			}
			
			void operator()( const void *begin, const void *end ) const
			{
				AEPutPtr( theList, theIndex, theType, begin, iota::distance( begin, end ) );
			}
	};
	
	template < DescType type >
	inline void AEPutPtr( AEDescList&                                  list,
	                      long                                         index,
	                      typename DescType_Traits< type >::Parameter  data )
	{
		DescType_Traits< type >::Put( data,
		                              AEPutPtr_Putter( list,
		                              index,
		                              DescType( DescType_Map_Traits< type >::result ) ) );
	}
	
	template < DescType type >
	inline void AEPutPtr( nucleus::owned< AEDescList_Data >&           list,
	                      long                                         index,
	                      typename DescType_Traits< type >::Parameter  data )
	{
		DescType_Traits< type >::Put( data,
		                              AEPutPtr_Putter( Detail::AEDescEditor< AEDescList_Data >( list ),
		                                               index,
		                                               DescType( DescType_Map_Traits< type >::result ) ) );
	}
	
	
	template < DescType type >
	class AEGetNthPtr_Getter
	{
		private:
			const AEDescList&  theList;
			long               theIndex;
		
		public:
			AEGetNthPtr_Getter( const AEDescList&  list,
			                    long               index ) : theList ( list  ),
			                                                 theIndex( index )
			{
			}
			
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
				AEGetNthPtr( theList, theIndex, type, begin, iota::distance( begin, end ) );
			}
	};
	
	template < DescType type >
	inline typename DescType_Traits< type >::Result
	AEGetNthPtr( const AEDescList&  listDesc,
	             long               index )
	{
		return DescType_Traits< type >::Get( AEGetNthPtr_Getter< DescType_Map_Traits< type >::result >( listDesc, index ) );
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
				AEPutKeyPtr( theRecord, theKeyword, theType, begin, iota::distance( begin, end ) );
			}
	};
	
	template < DescType type >
	inline void AEPutKeyPtr( AERecord&                                    record,
	                         AEKeyword                                    keyword,
	                         typename DescType_Traits< type >::Parameter  data )
	{
		DescType_Traits< type >::Put( data,
		                              AEPutKeyPtr_Putter( record,
		                                                  keyword,
		                                                  DescType( DescType_Map_Traits< type >::result ) ) );
	}
	
	template < DescType type >
	inline void AEPutKeyPtr( nucleus::owned< AERecord_Data >&             record,
	                         AEKeyword                                    keyword,
	                         typename DescType_Traits< type >::Parameter  data )
	{
		DescType_Traits< type >::Put( data,
		                              AEPutKeyPtr_Putter( Detail::AEDescEditor< AERecord_Data >( record ),
		                                                  keyword,
		                                                  DescType( DescType_Map_Traits< type >::result ) ) );
	}
	
	template < AEKeyword key >
	inline void AEPutKeyPtr( AERecord&                                    record,
	                         typename AEKeyword_Traits< key >::Parameter  data )
	{
		AEKeyword_Traits< key >::Put( data,
		                              AEPutKeyPtr_Putter( record,
		                                                  key,
		                                                  DescType( AEKeyword_Traits< key >::descType ) ) );
	}
	
	template < AEKeyword key >
	inline void AEPutKeyPtr( nucleus::owned< AERecord_Data >&             record,
	                         typename AEKeyword_Traits< key >::Parameter  data )
	{
		AEPutKeyPtr< key >( Detail::AEDescEditor< AERecord_Data >( record ), data );
	}
	
	template < AEKeyword key >
	class AEPutKeyPtr_Binding
	{
		private:
			typedef typename AEKeyword_Traits< key >::Parameter Parameter;
			
			Parameter itsData;
		
		public:
			AEPutKeyPtr_Binding( Parameter data ) : itsData( data )  {}
			
			Parameter Data() const  { return itsData; }
	};
	
	template < AEKeyword key >
	inline AEPutKeyPtr_Binding< key > AEPutKeyPtr( typename AEKeyword_Traits< key >::Parameter data )
	{
		return AEPutKeyPtr_Binding< key >( data );
	}
	
	
	template < DescType type >
	class AEGetKeyPtr_Getter
	{
		private:
			const AERecord& theRecord;
			AEKeyword       theKeyword;
		
		public:
			AEGetKeyPtr_Getter( const AERecord&  record,
			                    AEKeyword        keyword ) : theRecord ( record  ),
			                                                 theKeyword( keyword )
			{
			}
			
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
				AEGetKeyPtr( theRecord, theKeyword, type, begin, iota::distance( begin, end ) );
			}
	};
	
	template < DescType type >
	inline typename DescType_Traits< type >::Result
	AEGetKeyPtr( const AERecord&  record,
	             AEKeyword        keyword )
	{
		return DescType_Traits< type >::Get( AEGetKeyPtr_Getter< DescType_Map_Traits< type >::result >( record, keyword ) );
	}
	
	template < AEKeyword key >
	inline typename AEKeyword_Traits< key >::Result
	AEGetKeyPtr( const AERecord& record )
	{
		return AEKeyword_Traits< key >::Get( AEGetKeyPtr_Getter< AEKeyword_Traits< key >::descType >( record, key ) );
	}
	
	
	class AEPutParamPtr_Putter
	{
		private:
			AppleEvent&  theAppleEvent;
			AEKeyword    theKeyword;
			DescType     theType;
		
		public:
			AEPutParamPtr_Putter( AppleEvent&  appleEvent,
			                      AEKeyword    keyword,
			                      DescType     type ) : theAppleEvent( appleEvent  ),
			                                            theKeyword   ( keyword     ),
			                                            theType      ( type        )
			{
			}
			
			void operator()( const void *begin, const void *end ) const
			{
				AEPutParamPtr( theAppleEvent, theKeyword, theType, begin, iota::distance( begin, end ) );
			}
	};
	
	template < DescType type >
	inline void AEPutParamPtr( AppleEvent&                                  appleEvent,
	                           AEKeyword                                    keyword,
	                           typename DescType_Traits< type >::Parameter  data )
	{
		DescType_Traits< type >::Put( data,
		                              AEPutParamPtr_Putter( appleEvent,
		                                                    keyword,
		                                                    DescType( DescType_Map_Traits< type >::result ) ) );
	}
	
	template < DescType type >
	inline void AEPutParamPtr( nucleus::owned< AppleEvent >&                appleEvent,
	                           AEKeyword                                    keyword,
	                           typename DescType_Traits< type >::Parameter  data )
	{
		DescType_Traits< type >::Put( data,
		                              AEPutParamPtr_Putter( Detail::AEDescEditor< AppleEvent >( appleEvent ),
		                                                    keyword,
		                                                    DescType( DescType_Map_Traits< type >::result ) ) );
	}
	
	template < AEKeyword key >
	inline void AEPutParamPtr( AppleEvent&                                  appleEvent,
	                           typename AEKeyword_Traits< key >::Parameter  data )
	{
		AEKeyword_Traits< key >::Put( data,
		                              AEPutParamPtr_Putter( appleEvent,
		                                                    key,
		                                                    DescType( AEKeyword_Traits< key >::descType ) ) );
	}
	
	template < AEKeyword key >
	inline void AEPutParamPtr( nucleus::owned< AppleEvent >&                appleEvent,
	                           typename AEKeyword_Traits< key >::Parameter  data )
	{
		AEPutParamPtr< key >( Detail::AEDescEditor< AppleEvent >( appleEvent ), data );
	}
	
	
	template < DescType type >
	class AEGetParamPtr_Getter
	{
		private:
			const AppleEvent&  theAppleEvent;
			AEKeyword          theKeyword;
		
		public:
			AEGetParamPtr_Getter( const AppleEvent&  appleEvent,
			                      AEKeyword          keyword ) : theAppleEvent( appleEvent ),
			                                                     theKeyword   ( keyword    )
			{
			}
			
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
				AEGetParamPtr( theAppleEvent, theKeyword, type, begin, iota::distance( begin, end ) );
			}
	};
	
	template < DescType type >
	inline typename DescType_Traits< type >::Result
	AEGetParamPtr( const AppleEvent&  appleEvent,
	               AEKeyword          keyword )
	{
		return DescType_Traits< type >::Get( AEGetParamPtr_Getter< DescType_Map_Traits< type >::result >( appleEvent, keyword ) );
	}
	
	template < AEKeyword key >
	inline typename AEKeyword_Traits< key >::Result
	AEGetParamPtr( const AppleEvent& appleEvent )
	{
		return AEKeyword_Traits< key >::Get( AEGetParamPtr_Getter< AEKeyword_Traits< key >::descType >( appleEvent, key ) );
	}
	
	
	class AEPutAttributePtr_Putter
	{
		private:
			AppleEvent&  theAppleEvent;
			AEKeyword    theKeyword;
			DescType     theType;
		
		public:
			AEPutAttributePtr_Putter( AppleEvent&  appleEvent,
			                          AEKeyword    keyword,
			                          DescType     type ) : theAppleEvent( appleEvent  ),
			                                                theKeyword   ( keyword     ),
			                                                theType      ( type        )
			{
			}
			
			void operator()( const void *begin, const void *end ) const
			{
				AEPutAttributePtr( theAppleEvent, theKeyword, theType, begin, iota::distance( begin, end ) );
			}
	};
	
	template < DescType type >
	inline void AEPutAttributePtr( AppleEvent&                                  appleEvent,
	                               AEKeyword                                    keyword,
	                               typename DescType_Traits< type >::Parameter  data )
	{
		DescType_Traits< type >::Put( data,
		                              AEPutAttributePtr_Putter( appleEvent,
		                                                        keyword,
		                                                        DescType( DescType_Map_Traits< type >::result ) ) );
	}
	
	template < DescType type >
	inline void AEPutAttributePtr( nucleus::owned< AppleEvent >&                appleEvent,
	                               AEKeyword                                    keyword,
	                               typename DescType_Traits< type >::Parameter  data )
	{
		DescType_Traits< type >::Put( data,
		                              AEPutAttributePtr_Putter( Detail::AEDescEditor< AppleEvent >( appleEvent ),
		                                                        keyword,
		                                                        DescType( DescType_Map_Traits< type >::result ) ) );
	}
	
	template < AEKeyword key >
	inline void AEPutAttributePtr( AppleEvent&                                  appleEvent,
	                               typename AEKeyword_Traits< key >::Parameter  data )
	{
		AEKeyword_Traits< key >::Put( data,
		                              AEPutAttributePtr_Putter( appleEvent,
		                                                        key,
		                                                        DescType( AEKeyword_Traits< key >::descType ) ) );
	}
	
	template < AEKeyword key >
	inline void AEPutAttributePtr( nucleus::owned< AppleEvent >&                appleEvent,
	                               typename AEKeyword_Traits< key >::Parameter  data )
	{
		AEPutAttributePtr< key >( Detail::AEDescEditor< AppleEvent >( appleEvent ), data );
	}
	
	
	template < DescType type >
	class AEGetAttributePtr_Getter
	{
		private:
			const AppleEvent& theAppleEvent;
			AEKeyword         theKeyword;
		
		public:
			AEGetAttributePtr_Getter( const AppleEvent&  appleEvent,
			                          AEKeyword          keyword ) : theAppleEvent( appleEvent ),
			                                                         theKeyword   ( keyword    )
			{
			}
			
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
				AEGetAttributePtr( theAppleEvent, theKeyword, type, begin, iota::distance( begin, end ) );
			}
	};
	
	template < DescType type >
	inline typename DescType_Traits< type >::Result
	AEGetAttributePtr( const AppleEvent&  appleEvent,
	                   AEKeyword          keyword )
	{
		return DescType_Traits< type >::Get( AEGetAttributePtr_Getter< DescType_Map_Traits< type >::result >( appleEvent, keyword ) );
	}
	
	template < AEKeyword key >
	inline typename AEKeyword_Traits< key >::Result
	AEGetAttributePtr( const AppleEvent& appleEvent )
	{
		return AEKeyword_Traits< key >::Get( AEGetAttributePtr_Getter< AEKeyword_Traits< key >::descType >( appleEvent, key ) );
	}
	
	
	class AEGetDescData_Getter
	{
		private:
			const AEDesc& theDesc;
		
		public:
			AEGetDescData_Getter( const AEDesc& desc = AEInitializeDesc() ) : theDesc( desc )
			{
			}
			
			std::size_t size() const  { return AEGetDescDataSize( theDesc ); }
			
			void operator()( void *begin, void *end ) const
			{
				AEGetDescData( theDesc, begin, iota::distance( begin, end ) );
			}
	};
	
	template < DescType type >
	inline typename DescType_Traits< type >::Result
	AEGetDescData( const AEDesc&  desc,
	               DescType       requiredType = DescType_Map_Traits< type >::result )
	{
		ASSERT( requiredType == typeWildCard || requiredType == desc.descriptorType );
		
		(void) requiredType;
		
		return DescType_Traits< type >::Get( AEGetDescData_Getter( desc ) );
	}
	
	template < DescType type >
	inline typename DescType_Traits< type >::Result AECoerceDescData( const AEDesc& desc )
	{
		return AEGetDescData< type >( AECoerceDesc( desc, DescType_Map_Traits< type >::result ) );
	}
	
	
	class AEReplaceDescData_Putter
	{
		private:
			DescType                        theType;
			nucleus::owned< AEDesc_Data >&  theDesc;
		
		public:
			AEReplaceDescData_Putter( DescType                        type,
			                          nucleus::owned< AEDesc_Data >&  desc ) : theType( type ),
			                                                                   theDesc( desc )
			{
			}
			
			void operator()( const void *begin, const void *end ) const
			{
				AEReplaceDescData( theType, begin, iota::distance( begin, end ), theDesc );
			}
	};
	
	template < DescType type >
	inline void AEReplaceDescData( typename DescType_Traits< type >::Parameter  data,
	                               AEDesc&                                      result )
	{
		DescType_Traits< type >::Put( data,
		                              AEReplaceDescData_Putter( DescType_Map_Traits< type >::result,
		                                                        result ) );
	}
	
	template < DescType type >
	inline void AEReplaceDescData( typename DescType_Traits< type >::Parameter  data,
	                               nucleus::owned< AEDesc_Data >&               result )
	{
		DescType_Traits< type >::Put( data,
		                              AEReplaceDescData_Putter( DescType_Map_Traits< type >::result,
		                                                        Detail::AEDescEditor< AEDesc_Data >( result ) ) );
	}
	
}

#endif

