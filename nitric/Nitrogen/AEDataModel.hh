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
#ifndef NUCLEUS_MAKE_HH
#include "nucleus/make.hh"
#endif

// Nitrogen
#ifndef MAC_ALIASES_TYPES_ALIASHANDLE_HH
#include "Mac/Aliases/Types/AliasHandle.hh"
#endif
#ifndef MAC_APPLEEVENTS_FUNCTIONS_AEDISPOSEDESC_HH
#include "Mac/AppleEvents/Functions/AEDisposeDesc.hh"
#endif
#ifndef MAC_APPLEEVENTS_FUNCTIONS_AEDISPOSETOKEN_HH
#include "Mac/AppleEvents/Functions/AEDisposeToken.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_AEKEYWORDSCRIBE_HH
#include "Mac/AppleEvents/Types/AEKeyword_scribe.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBETEXT_HH
#include "Mac/AppleEvents/Types/DescType_scribe_text.hh"
#endif
#ifndef MAC_APPLEEVENTS_UTILITIES_NONNULLAEDESCSARELIVE_HH
#include "Mac/AppleEvents/Utilities/NonNull_AEDescs_Are_Live.hh"
#endif
#ifndef MAC_TOOLBOX_TYPES_FIXED_HH
#include "Mac/Toolbox/Types/Fixed.hh"
#endif

#ifndef NITROGEN_MACMEMORY_HH
#include "Nitrogen/MacMemory.hh"
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


namespace Mac
{
	
	template <> struct DescType_scribe< typeAlias > : type_< Nitrogen::TypedHandleFlattener< AliasRecord > > {};
	
}

namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( AppleEventManager );
	
	#pragma mark -
	#pragma mark ** DescType **
	
	// Constants used creating an AppleEvent
	
	using Mac::AEReturnID;
	using Mac::kAutoGenerateReturnID;
	
	using Mac::AEReturnID_32Bit;
	
	using Mac::AETransactionID;
	using Mac::kAnyTransactionID;
	
	using Mac::AEEventClass;
	using Mac::AEEventID;
	
	
	#pragma mark -
	#pragma mark ** DescType_Traits **
	
	using Mac::DescType_parameter;
	using Mac::DescType_result;
	
	using Mac::DescType_put;
	using Mac::DescType_get;
	
	using Mac::AEKeyword_DescType;
	using Mac::AEKeyword_parameter;
	using Mac::AEKeyword_result;
	
	using Mac::AEKeyword_put;
	using Mac::AEKeyword_get;
	
	
	#pragma mark -
	#pragma mark ** AEDesc **
	
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
	struct maker< Mac::AEDesc_Data >
	{
		Mac::AEDesc_Data operator()() const
		{
			Mac::AEDesc_Data result;
			
			static_cast< AEDesc& >( result ) = make< AEDesc >();
			
			return result;
		}
	};
	
	template <>
	struct maker< Mac::AEDesc_Token >
	{
		Mac::AEDesc_Token operator()() const
		{
			Mac::AEDesc_Token result;
			
			static_cast< AEDesc& >( result ) = make< AEDesc >();
			
			return result;
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
			disposer< Mac::AEDesc_Data >()( keyDesc.descContent );
		}
	};
	
	template <>
	struct null_resource< ::AEKeyDesc >
	{
		static const ::AEKeyDesc& value;
		
		static const ::AEKeyDesc& get()  { return value; }
	};
	
	template <>
	struct aliveness_traits< AEKeyDesc, disposer< AEKeyDesc > >
	{
		typedef Mac::NonNull_AEDescs_Are_Live aliveness_test;
	};
	
}

namespace nucleus
{
	
	template <>
	struct maker< AEKeyDesc >
	{
		AEKeyDesc operator()() const
		{
			AEKeyDesc result = { Mac::AEKeyword(), make< AEDesc >() };
			
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
	
	inline void InvokeAECoerceDescUPP( const Mac::AEDesc_Data&  fromDesc,
	                                   Mac::DescType            toType,
	                                   RefCon                   handlerRefCon,
	                                   Mac::AEDesc_Data&        toDesc,
	                                   AECoerceDescUPP          userUPP )
	{
		ThrowOSStatus( userUPP( &fromDesc,
		                        toType,
		                        handlerRefCon,
		                        &toDesc ) );
	}
	
	inline void InvokeAECoercePtrUPP( Mac::DescType      typeCode,
	                                  const void*        dataPtr,
	                                  std::size_t        dataSize,
	                                  Mac::DescType      toType,
	                                  RefCon             handlerRefCon,
	                                  Mac::AEDesc_Data&  toDesc,
	                                  AECoercePtrUPP     userUPP )
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
		Mac::DescType         fromType;
		Mac::DescType         toType;
		AECoercionHandlerUPP  handler;
		RefCon                handlerRefCon;
		bool                  fromTypeIsDesc;
		bool                  isSysHandler;
		
		AECoercionHandler();
		
		AECoercionHandler( Mac::DescType         fromType,
		                   Mac::DescType         toType,
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
		
		AECoercionHandler( Mac::DescType    fromType,
		                   Mac::DescType    toType,
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
		
		AECoercionHandler( Mac::DescType   fromType,
		                   Mac::DescType   toType,
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
	
	typedef nucleus::owned< Mac::AEDesc_Data > ( *AECoerceDescProcPtr )( const Mac::AEDesc_Data&  fromDesc,
	                                                                     Mac::DescType            toType,
	                                                                     RefCon                   refCon );
	
	typedef nucleus::owned< Mac::AEDesc_Data > ( *AECoercePtrProcPtr )( Mac::DescType  typeCode,
	                                                                    const void*    dataPtr,
	                                                                    std::size_t    datasize,
	                                                                    Mac::DescType  toType,
	                                                                    RefCon         refCon );
	
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
				*result = handler( static_cast< const Mac::AEDesc_Data& >( *fromDesc ),
				                   Mac::DescType( toType ),
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
				*result = handler( Mac::DescType( typeCode ),
				                   dataPtr,
				                   dataSize,
				                   Mac::DescType( toType ),
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
	
	inline nucleus::owned< AECoercionHandler >
	//
	AEInstallCoercionHandler( Mac::DescType    fromType,
	                          Mac::DescType    toType,
	                          AECoerceDescUPP  handler,
	                          RefCon           handlerRefCon = RefCon(),
	                          bool             isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    handler,
		                                                    handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	inline nucleus::owned< AECoercionHandler >
	//
	AEInstallCoercionHandler( Mac::DescType   fromType,
	                          Mac::DescType   toType,
	                          AECoercePtrUPP  handler,
	                          RefCon          handlerRefCon = RefCon(),
	                          bool            isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    handler,
		                                                    handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	template < typename AECoerceDescUPP::ProcPtr handler >
	nucleus::owned< AECoercionHandler >
	//
	AEInstallCoercionHandler( Mac::DescType  fromType,
	                          Mac::DescType  toType,
	                          RefCon         handlerRefCon = RefCon(),
	                          bool           isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    StaticUPP< AECoerceDescUPP, handler >(),
		                                                    handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	template < typename AECoercePtrUPP::ProcPtr handler >
	nucleus::owned< AECoercionHandler >
	//
	AEInstallCoercionHandler( Mac::DescType  fromType,
	                          Mac::DescType  toType,
	                          RefCon         handlerRefCon = RefCon(),
	                          bool           isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    StaticUPP< AECoercePtrUPP, handler >(),
		                                                    handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	template < AECoerceDescProcPtr handler >
	nucleus::owned< AECoercionHandler >
	//
	AEInstallCoercionHandler( Mac::DescType  fromType,
	                          Mac::DescType  toType,
	                          RefCon         handlerRefCon = RefCon(),
	                          bool           isSysHandler  = false )
	{
		return AEInstallCoercionHandler< AECoercionHandler_Desc_Callback< handler >::Adapter >( fromType,
		                                                                                        toType,
		                                                                                        handlerRefCon,
		                                                                                        isSysHandler );
	}
	
	template < AECoercePtrProcPtr handler >
	nucleus::owned< AECoercionHandler >
	//
	AEInstallCoercionHandler( Mac::DescType  fromType,
	                          Mac::DescType  toType,
	                          RefCon         handlerRefCon = RefCon(),
	                          bool           isSysHandler  = false )
	{
		return AEInstallCoercionHandler< AECoercionHandler_Ptr_Callback< handler >::Adapter >( fromType,
		                                                                                       toType,
		                                                                                       handlerRefCon,
		                                                                                       isSysHandler );
	}
	
	// 406
	inline void AERemoveCoercionHandler( nucleus::owned< AECoercionHandler > )  {}
	
	typedef AECoercionHandler AEGetCoercionHandler_Result;
	
	AECoercionHandler AEGetCoercionHandler( Mac::DescType  fromType,
	                                        Mac::DescType  toType,
	                                        bool           isSysHandler );
	
	// 444
	nucleus::owned< Mac::AEDesc_Data > AECoercePtr( Mac::DescType  typeCode,
	                                                const void*    dataPtr,
	                                                std::size_t    dataSize,
	                                                Mac::DescType  toType );
	
	// 461
	nucleus::owned< Mac::AEDesc_Data > AECoerceDesc( const AEDesc& desc, Mac::DescType toType );
	
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
		Mac::DescType  typeCode;
		std::size_t    dataSize;
		
		operator std::size_t() const  { return dataSize; }
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
		
		return nucleus::owned< AEDesc_Type >();
	}
	
	namespace Detail
	{
		
		AEDesc AECreateDesc_Unowned( Mac::DescType  typeCode,
		                             const void*    dataPtr,
		                             std::size_t    dataSize );
		
		AEDesc AECreateDesc_Unowned( Mac::DescType typeCode, Handle handle );
		
		AEDesc AECreateDesc_Unowned( Mac::DescType typeCode, nucleus::owned< Handle > handle );
		
		AEDesc AEDuplicateDesc_Unowned( const AEDesc& desc );
		
		AEDesc AECreateList_Unowned( const void*  factoringPtr,
		                             std::size_t  factoredSize,
		                             bool         isRecord );
		
		AEDesc AEGetNthDesc_Unowned( const AEDesc&  listDesc,
		                             long           index,
		                             Mac::DescType  desiredType,
		                             ::AEKeyword*   keyword );
		
		AEDesc AEGetKeyDesc_Unowned( const AERecord&  record,
		                             Mac::AEKeyword   keyword,
		                             Mac::DescType    desiredType );
		
	}
	
	template < class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateDesc( Mac::DescType  typeCode,
	                                                   const void*    dataPtr,
	                                                   std::size_t    dataSize )
	{
		// AEDesc_Type must be a subclass of AEDesc
		(void) static_cast< const AEDesc& >( AEDesc_Type() );
		
		AEDesc desc = Detail::AECreateDesc_Unowned( typeCode, dataPtr, dataSize );
		
		return nucleus::owned< AEDesc_Type >::seize( AEDesc_Cast< AEDesc_Type >( desc ) );
	}
	
	template < class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateDesc( Mac::DescType  typeCode,
	                                                   Handle         handle )
	{
		// AEDesc_Type must be a subclass of AEDesc
		(void) static_cast< const AEDesc& >( AEDesc_Type() );
		
		AEDesc desc = Detail::AECreateDesc_Unowned( typeCode, handle );
		
		return nucleus::owned< AEDesc_Type >::seize( AEDesc_Cast< AEDesc_Type >( desc ) );
	}
	
	template < class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateDesc( Mac::DescType             typeCode,
	                                                   nucleus::owned< Handle >  handle )
	{
		// AEDesc_Type must be a subclass of AEDesc
		(void) static_cast< const AEDesc& >( AEDesc_Type() );
		
		AEDesc desc = Detail::AECreateDesc_Unowned( typeCode, handle );
		
		return nucleus::owned< AEDesc_Type >::seize( AEDesc_Cast< AEDesc_Type >( desc ) );
	}
	
	template < class T, class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateDesc( Mac::DescType typeCode, T** handle )
	{
		return AECreateDesc< AEDesc_Type >( typeCode, Handle( handle ) );
	}
	
	template < class T, class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateDesc( Mac::DescType                                       typeCode,
	                                                   nucleus::owned< T**, nucleus::disposer< Handle > >  handle )
	{
		return AECreateDesc< AEDesc_Type >( typeCode, nucleus::owned< Handle >( handle ) );
	}
	
	template < class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateDesc( Mac::DescType typeCode, nucleus::owned< AEDesc_Type > desc )
	{
		Detail::AEDescEditor< AEDesc_Type >( desc ).Get().descriptorType = typeCode;
		
		return desc;
	}
	
	template < class T >
	inline nucleus::owned< Mac::AEDesc_Data > AECreateDesc( Mac::DescType typeCode, T** handle )
	{
		return AECreateDesc< T, Mac::AEDesc_Data >( typeCode, handle );
	}
	
	template < class T >
	inline nucleus::owned< Mac::AEDesc_Data > AECreateDesc( Mac::DescType                                       typeCode,
	                                                        nucleus::owned< T**, nucleus::disposer< Handle > >  handle )
	{
		return AECreateDesc< T, Mac::AEDesc_Data >( typeCode, handle );
	}
	
	namespace MetrowerksHack
	{
		
		// CW Pro 6 complains of illegal function overloading without this namespace
		
		inline nucleus::owned< Mac::AEDesc_Data > AECreateDesc( Mac::DescType  typeCode,
		                                                        const void*    dataPtr,
		                                                        std::size_t    dataSize )
		{
			return Nitrogen::AECreateDesc< Mac::AEDesc_Data >( typeCode, dataPtr, dataSize );
		}
		
		inline nucleus::owned< Mac::AEDesc_Data > AECreateDesc( Mac::DescType  typeCode,
		                                                        Handle         handle )
		{
			return Nitrogen::AECreateDesc< Mac::AEDesc_Data >( typeCode, handle );
		}
		
		inline nucleus::owned< Mac::AEDesc_Data > AECreateDesc( Mac::DescType             typeCode,
		                                                        nucleus::owned< Handle >  handle )
		{
			return Nitrogen::AECreateDesc< Mac::AEDesc_Data >( typeCode, handle );
		}
		
	}
	
	using namespace MetrowerksHack;
	
	
	using Mac::AEDisposeDesc;
	
	inline nucleus::owned< Mac::AEDesc_Data > AEDuplicateDesc( const AEDesc& original )
	{
		AEDesc desc = Detail::AEDuplicateDesc_Unowned( original );
		
		return nucleus::owned< Mac::AEDesc_Data >::seize( static_cast< const Mac::AEDesc_Data& >( desc ) );
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
	inline nucleus::owned< AEDesc_Type > AECreateList( Mac::DescType typeCode, bool isRecord )
	{
		return AECreateDesc( typeCode, AECreateList< AEDesc_Type >( isRecord ) );
	}
	
	namespace MetrowerksHack
	{
		
		// CW Pro 6 complains of illegal function overloading without this namespace
		
		inline nucleus::owned< Mac::AEDesc_Data > AECreateList( const void*  factoringPtr,
		                                                        std::size_t  factoredSize,
		                                                        bool         isRecord )
		{
			return Nitrogen::AECreateList< Mac::AEDesc_Data >( factoringPtr, factoredSize, isRecord );
		}
		
		inline nucleus::owned< Mac::AEDesc_Data > AECreateList( bool isRecord = false )
		{
			return Nitrogen::AECreateList< Mac::AEDesc_Data >( isRecord );
		}
		
		inline nucleus::owned< Mac::AEDesc_Data > AECreateList( Mac::DescType typeCode, bool isRecord )
		{
			return Nitrogen::AECreateList< Mac::AEDesc_Data >( typeCode, isRecord );
		}
		
	}
	
	using namespace MetrowerksHack;
	
	long AECountItems( const AEDesc& desc );
	
	void AEPutPtr( AEDescList&    list,
	               long           index,
	               Mac::DescType  type,
	               const void*    dataPtr,
	               std::size_t    dataSize );
	
	inline void AEPutPtr( nucleus::owned< Mac::AEDescList_Data >&  list,
	                      long                                     index,
	                      Mac::DescType                            type,
	                      const void*                              dataPtr,
	                      std::size_t                              dataSize )
	{
		AEPutPtr( Detail::AEDescEditor< Mac::AEDescList_Data >( list ),
		          index,
		          type,
		          dataPtr,
		          dataSize );
	}
	
	void AEPutDesc( AEDescList&    list,
	                long           index,
	                const AEDesc&  desc );
	
	inline void AEPutDesc( nucleus::owned< Mac::AEDescList_Data >&  list,
	                       long                                     index,
	                       const Mac::AEDesc_Data&                  desc )
	{
		AEPutDesc( Detail::AEDescEditor< Mac::AEDescList_Data >( list ), index, desc );
	}
	
	inline void AEPutDesc( nucleus::owned< Mac::AEDescList_Token >&  list,
	                       long                                      index,
	                       nucleus::owned< Mac::AEDesc_Token >       desc )
	{
		// This is the only variant of AEPutDesc() supported for tokens.
		// The desc argument must be an Owned token to ensure that its tokenness
		// is managed correctly (i.e. disposed exactly once).
		// Since a token list's token disposal function calls AEDisposeToken()
		// on each member token, we must not do so here.  But we do need to
		// dispose the AEDesc.  Got it?
		
		AEPutDesc( AEDesc_Cast< Mac::AEDescList_Data >( Detail::AEDescEditor< Mac::AEDescList_Token >( list ).Get() ),
		           index,
		           AEDesc_Cast< const Mac::AEDesc_Data >( desc.get() ) );
		
		nucleus::disposer< Mac::AEDesc_Data >()( desc.release() );
	}
	
	struct GetNthPtr_Result
	{
		Mac::AEKeyword  keyword;
		Mac::DescType   typeCode;
		std::size_t     actualSize;
	};
	
	GetNthPtr_Result AEGetNthPtr( const AEDesc&  listDesc,
	                              long           index,
	                              Mac::DescType  desiredType,
	                              void*          dataPtr,
	                              std::size_t    maximumSize );
	
	inline nucleus::owned< Mac::AEDesc_Data > AEGetNthDesc( const AEDesc&  listDesc,
	                                                        long           index,
	                                                        Mac::DescType  desiredType = Mac::typeWildCard,
	                                                        ::AEKeyword*   keyword     = NULL )
	{
		AEDesc desc = Detail::AEGetNthDesc_Unowned( listDesc, index, desiredType, keyword );
		
		return nucleus::owned< Mac::AEDesc_Data >::seize( static_cast< const Mac::AEDesc_Data& >( desc ) );
	}
	
	inline nucleus::owned< Mac::AEDesc_Data > AEGetNthDesc( const AEDesc&  listDesc,
	                                                        long           index,
	                                                        ::AEKeyword*   keyword )
	{
		return AEGetNthDesc( listDesc, index, Mac::typeWildCard, keyword );
	}
	
	AESizeOfNthItem_Result AESizeOfNthItem( const AEDescList& list, long index );
	
	// If you have a reason for deleting an item in a token list, please let me know
	
	void AEDeleteItem(                 Mac::AEDescList_Data  & listDesc, long index );
	void AEDeleteItem( nucleus::owned< Mac::AEDescList_Data >& listDesc, long index );
	
	#pragma mark -
	#pragma mark ** AERecords **
	
	inline bool AECheckIsRecord( const AEDesc& theDesc )
	{
		return ::AECheckIsRecord( &theDesc );
	}
	
	void AEPutKeyPtr( AERecord&       record,
	                  Mac::AEKeyword  keyword,
	                  Mac::DescType   typeCode,
	                  const void*     dataPtr,
	                  std::size_t     dataSize );
	
	inline void AEPutKeyPtr( nucleus::owned< Mac::AERecord_Data >&  record,
	                         Mac::AEKeyword                         keyword,
	                         Mac::DescType                          typeCode,
	                         const void*                            dataPtr,
	                         std::size_t                            dataSize )
	{
		AEPutKeyPtr( Detail::AEDescEditor< Mac::AERecord_Data >( record ),
		             keyword,
		             typeCode,
		             dataPtr,
		             dataSize );
	}
	
	void AEPutKeyDesc( AERecord&       record,
	                   Mac::AEKeyword  keyword,
	                   const AEDesc&   desc );
	
	void AEPutKeyDesc( AERecord&         record,
	                   const AEKeyDesc&  keyDesc );
	
	inline void AEPutKeyDesc( nucleus::owned< Mac::AERecord_Data >&  record,
	                          Mac::AEKeyword                         keyword,
	                          const Mac::AEDesc_Data&                desc )
	{
		AEPutKeyDesc( Detail::AEDescEditor< Mac::AERecord_Data >( record ), keyword, desc );
	}
	
	inline void AEPutKeyDesc( nucleus::owned< Mac::AERecord_Token >&  record,
	                          Mac::AEKeyword                          keyword,
	                          nucleus::owned< Mac::AEDesc_Token >     token )
	{
		AEPutKeyDesc( AEDesc_Cast< Mac::AERecord_Data >( Detail::AEDescEditor< Mac::AERecord_Token >( record ).Get() ),
		              keyword,
		              AEDesc_Cast< const Mac::AEDesc_Data >( token ) );
		
		nucleus::disposer< Mac::AEDesc_Data >()( token.release() );
	}
	
	inline void AEPutKeyDesc( nucleus::owned< Mac::AERecord_Data >&  record,
	                          const AEKeyDesc&                       keyDesc )
	{
		AEPutKeyDesc( Detail::AEDescEditor< Mac::AERecord_Data >( record ), keyDesc );
	}
	
	class AEPutKeyDesc_Binding
	{
		private:
			Mac::AEKeyword  itsKey;
			const AEDesc&   itsData;
		
		public:
			AEPutKeyDesc_Binding( Mac::AEKeyword key, const AEDesc& data ) : itsKey( key ), itsData( data )  {}
			
			Mac::AEKeyword Key() const  { return itsKey; }
			
			const AEDesc& Data() const  { return itsData; }
	};
	
	inline AEPutKeyDesc_Binding AEPutKeyDesc( Mac::AEKeyword key, const AEDesc& data )
	{
		return AEPutKeyDesc_Binding( key, data );
	}
	
	AEGetKeyPtr_Result AEGetKeyPtr( const AERecord&  record,
	                                Mac::AEKeyword   keyword,
	                                Mac::DescType    desiredType,
	                                void*            dataPtr,
	                                std::size_t      maximumSize );
	
	inline nucleus::owned< Mac::AEDesc_Data > AEGetKeyDesc( const AERecord&  record,
	                                                        Mac::AEKeyword   keyword,
	                                                        Mac::DescType    desiredType = Mac::typeWildCard )
	{
		AEDesc desc = Detail::AEGetKeyDesc_Unowned( record, keyword, desiredType );
		
		return nucleus::owned< Mac::AEDesc_Data >::seize( AEDesc_Cast< Mac::AEDesc_Data >( desc ) );
	}
	
	AESizeOfKeyDesc_Result AESizeOfKeyDesc( const AERecord&  record,
	                                        Mac::AEKeyword   keyword );
	
	void AEDeleteKeyDesc( AERecord& record, Mac::AEKeyword keyword );
	
	inline void AEDeleteKeyDesc( nucleus::owned< Mac::AERecord_Data >& record, Mac::AEKeyword keyword )
	{
		AEDeleteKeyDesc( Detail::AEDescEditor< Mac::AERecord_Data >( record ), keyword );
	}
	
	#pragma mark -
	#pragma mark ** AppleEvents **
	
	nucleus::owned< Mac::AppleEvent >
	//
	AECreateAppleEvent( Mac::AEEventClass          eventClass,
	                    Mac::AEEventID             eventID,
	                    const Mac::AEAddressDesc&  target,
	                    Mac::AEReturnID            returnID      = Mac::AEReturnID(),
	                    Mac::AETransactionID       transactionID = Mac::AETransactionID() );
	
	void AEPutParamPtr( Mac::AppleEvent&  appleEvent,
	                    Mac::AEKeyword    keyword,
	                    Mac::DescType     typeCode,
	                    const void*       dataPtr,
	                    std::size_t       dataSize );
	
	void AEPutParamPtr( nucleus::owned< Mac::AppleEvent >&  appleEvent,
	                    Mac::AEKeyword                      keyword,
	                    Mac::DescType                       typeCode,
	                    const void*                         dataPtr,
	                    std::size_t                         dataSize );
	
	void AEPutParamDesc( Mac::AppleEvent&  appleEvent,
	                     Mac::AEKeyword    keyword,
	                     const AEDesc&     desc );
	
	void AEPutParamDesc( nucleus::owned< Mac::AppleEvent >&  appleEvent,
	                     Mac::AEKeyword                      keyword,
	                     const AEDesc&                       desc );
	
	AEGetParamPtr_Result AEGetParamPtr( const Mac::AppleEvent&  appleEvent,
	                                    Mac::AEKeyword          keyword,
	                                    Mac::DescType           desiredType,
	                                    void*                   dataPtr,
	                                    std::size_t             maximumSize );
	
	nucleus::owned< Mac::AEDesc_Data >
	//
	AEGetParamDesc( const Mac::AppleEvent&  appleEvent,
	                Mac::AEKeyword          keyword,
	                Mac::DescType           desiredType = Mac::typeWildCard );
	
	AESizeOfParam_Result AESizeOfParam( const Mac::AppleEvent&  appleEvent,
	                                    Mac::AEKeyword          keyword );
	
	void AEDeleteParam(                 Mac::AppleEvent  & appleEvent, Mac::AEKeyword keyword );
	void AEDeleteParam( nucleus::owned< Mac::AppleEvent >& appleEvent, Mac::AEKeyword keyword );
	
	void AEPutAttributePtr( Mac::AppleEvent&  appleEvent,
	                        Mac::AEKeyword    keyword,
	                        Mac::DescType     typeCode,
	                        const void*       dataPtr,
	                        std::size_t       dataSize );
	
	void AEPutAttributePtr( nucleus::owned< Mac::AppleEvent >&  appleEvent,
	                        Mac::AEKeyword                      keyword,
	                        Mac::DescType                       typeCode,
	                        const void*                         dataPtr,
	                        std::size_t                         dataSize );
	
	void AEPutAttributeDesc( Mac::AppleEvent&  appleEvent,
	                         Mac::AEKeyword    keyword,
	                         const AEDesc&     desc );
	
	void AEPutAttributeDesc( nucleus::owned< Mac::AppleEvent >&  appleEvent,
	                         Mac::AEKeyword                      keyword,
	                         const AEDesc&                       desc );
	
	AEGetAttributePtr_Result AEGetAttributePtr( const Mac::AppleEvent&  appleEvent,
	                                            Mac::AEKeyword          keyword,
	                                            Mac::DescType           desiredType,
	                                            void*                   dataPtr,
	                                            std::size_t             maximumSize );
	
	nucleus::owned< Mac::AEDesc_Data >
	//
	AEGetAttributeDesc( const Mac::AppleEvent&  appleEvent,
	                    Mac::AEKeyword          keyword,
	                    Mac::DescType           desiredType = Mac::typeWildCard );
	
	AESizeOfAttribute_Result AESizeOfAttribute( const Mac::AppleEvent&  appleEvent,
	                                            Mac::AEKeyword          keyword );
	
	#pragma mark -
	#pragma mark ** Opaque data **
	
	void AEGetDescData( const AEDesc&  desc,
	                    void*          dataPtr,
	                    std::size_t    maximumSize );
	
	inline std::size_t AEGetDescDataSize( const AEDesc& desc )
	{
		return ::AEGetDescDataSize( &desc );
	}
	
	void AEReplaceDescData( Mac::DescType  typeCode,
	                        const void*    dataPtr,
	                        std::size_t    dataSize,
	                        AEDesc&        result );
	
	void AEReplaceDescData( Mac::DescType                        typeCode,
	                        const void*                          dataPtr,
	                        std::size_t                          dataSize,
	                        nucleus::owned< Mac::AEDesc_Data >&  result );
	
	
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
	
	inline nucleus::owned< AEEventHandlerUPP > NewAEEventHandlerUPP( ::AEEventHandlerProcPtr p )
	{
		return NewUPP< AEEventHandlerUPP >( p );
	}
	
	inline void DisposeAEEventHandlerUPP( nucleus::owned< AEEventHandlerUPP > )
	{
	}
	
	inline void InvokeAEEventHandlerUPP( const Mac::AppleEvent& theAppleEvent,
	                                     Mac::AppleEvent& reply,
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
			Mac::DescType                        fromType;
			Mac::DescType                        toType;
			nucleus::owned< Mac::AEDesc_Data >&  theDesc;
		
		public:
			AECoercePtr_Putter( Mac::DescType                        from,
			                    Mac::DescType                        to,
			                    nucleus::owned< Mac::AEDesc_Data >&  desc )
			:
				fromType( from ),
				toType  ( to   ),
				theDesc ( desc )
			{
			}
			
			void operator()( const void *begin, const void *end ) const
			{
				theDesc = AECoercePtr( fromType, begin, iota::distance( begin, end ), toType );
			}
	};
	
	template < Mac::DescType type >
	inline nucleus::owned< Mac::AEDesc_Data >
	//
	AECoercePtr( typename DescType_parameter< type >::type  data,
	             Mac::DescType                              toType )
	{
		nucleus::owned< Mac::AEDesc_Data > result;
		
		DescType_put< type >( data,
		                      AECoercePtr_Putter( type, toType, result ) );
		
		return result;
	}
	
	
	template < class AEDesc_Type >
	class AECreateDesc_Putter
	{
		private:
			Mac::DescType                   theType;
			nucleus::owned< AEDesc_Type >&  theDesc;
		
		public:
			AECreateDesc_Putter( Mac::DescType                   type,
			                     nucleus::owned< AEDesc_Type >&  desc ) : theType( type ),
			                                                              theDesc( desc )
			{
			}
			
			void operator()( const void *begin, const void *end ) const
			{
				theDesc = AECreateDesc< AEDesc_Type >( theType, begin, iota::distance( begin, end ) );
			}
	};
	
	template < Mac::DescType type, class AEDesc_Type >
	inline nucleus::owned< AEDesc_Type > AECreateDesc( typename DescType_parameter< type >::type data )
	{
		nucleus::owned< AEDesc_Type > result;
		
		DescType_put< type >( data,
		                      AECreateDesc_Putter< AEDesc_Type >( type, result ) );
		
		return result;
	}
	
	template < Mac::DescType type >
	inline nucleus::owned< Mac::AEDesc_Data >
	//
	AECreateDesc( typename DescType_parameter< type >::type data )
	{
		return AECreateDesc< type, Mac::AEDesc_Data >( data );
	}
	
	
	class AEPutPtr_Putter
	{
		private:
			AEDescList&    theList;
			long           theIndex;
			Mac::DescType  theType;
		
		public:
			AEPutPtr_Putter( AEDescList&    list,
			                 long           index,
			                 Mac::DescType  type )
			:
				theList ( list  ),
				theIndex( index ),
				theType ( type  )
			{
			}
			
			void operator()( const void *begin, const void *end ) const
			{
				AEPutPtr( theList, theIndex, theType, begin, iota::distance( begin, end ) );
			}
	};
	
	template < Mac::DescType type >
	inline void AEPutPtr( AEDescList&                                list,
	                      long                                       index,
	                      typename DescType_parameter< type >::type  data )
	{
		DescType_put< type >( data,
		                      AEPutPtr_Putter( list, index, type ) );
	}
	
	template < Mac::DescType type >
	inline void AEPutPtr( nucleus::owned< Mac::AEDescList_Data >&    list,
	                      long                                       index,
	                      typename DescType_parameter< type >::type  data )
	{
		DescType_put< type >( data,
		                      AEPutPtr_Putter( Detail::AEDescEditor< Mac::AEDescList_Data >( list ),
		                                       index,
		                                       type ) );
	}
	
	
	template < Mac::DescType type >
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
				
				typedef typename Mac::DescType_scribe< type >::type scribe;
				
				if ( nucleus::scribe_has_static_size< scribe >::value )
				{
					return scribe::static_size;
				}
				
				ASSERT( info.typeCode == type );
				
				return info.dataSize;
			}
			
			void operator()( void *begin, void *end ) const
			{
				AEGetNthPtr( theList, theIndex, type, begin, iota::distance( begin, end ) );
			}
	};
	
	template < Mac::DescType type >
	inline typename DescType_result< type >::type
	AEGetNthPtr( const AEDescList&  listDesc,
	             long               index )
	{
		return DescType_get< type >( AEGetNthPtr_Getter< type >( listDesc, index ) );
	}
	
	
	class AEPutKeyPtr_Putter
	{
		private:
			AERecord&       theRecord;
			Mac::AEKeyword  theKeyword;
			Mac::DescType   theType;
		
		public:
			AEPutKeyPtr_Putter( AERecord&       record,
			                    Mac::AEKeyword  keyword,
			                    Mac::DescType   type )
			: theRecord ( record  ),
			  theKeyword( keyword ),
			  theType   ( type    )  {}
			
			void operator()( const void *begin, const void *end ) const
			{
				AEPutKeyPtr( theRecord, theKeyword, theType, begin, iota::distance( begin, end ) );
			}
	};
	
	template < Mac::DescType type >
	inline void AEPutKeyPtr( AERecord&                                  record,
	                         Mac::AEKeyword                             keyword,
	                         typename DescType_parameter< type >::type  data )
	{
		DescType_put< type >( data,
		                      AEPutKeyPtr_Putter( record,
		                                          keyword,
		                                          type ) );
	}
	
	template < Mac::DescType type >
	inline void AEPutKeyPtr( nucleus::owned< Mac::AERecord_Data >&      record,
	                         Mac::AEKeyword                             keyword,
	                         typename DescType_parameter< type >::type  data )
	{
		DescType_put< type >( data,
		                      AEPutKeyPtr_Putter( Detail::AEDescEditor< Mac::AERecord_Data >( record ),
		                                          keyword,
		                                          type ) );
	}
	
	template < Mac::AEKeyword key >
	inline void AEPutKeyPtr( AERecord&                                  record,
	                         typename AEKeyword_parameter< key >::type  data )
	{
		AEKeyword_put< key >( data,
		                      AEPutKeyPtr_Putter( record,
		                                          key,
		                                          AEKeyword_DescType< key >::value ) );
	}
	
	template < Mac::AEKeyword key >
	inline void AEPutKeyPtr( nucleus::owned< Mac::AERecord_Data >&      record,
	                         typename AEKeyword_parameter< key >::type  data )
	{
		AEPutKeyPtr< key >( Detail::AEDescEditor< Mac::AERecord_Data >( record ), data );
	}
	
	template < Mac::AEKeyword key >
	class AEPutKeyPtr_Binding
	{
		private:
			typedef typename AEKeyword_parameter< key >::type Parameter;
			
			Parameter itsData;
		
		public:
			AEPutKeyPtr_Binding( Parameter data ) : itsData( data )  {}
			
			Parameter Data() const  { return itsData; }
	};
	
	template < Mac::AEKeyword key >
	inline AEPutKeyPtr_Binding< key > AEPutKeyPtr( typename AEKeyword_parameter< key >::type data )
	{
		return AEPutKeyPtr_Binding< key >( data );
	}
	
	
	template < Mac::DescType type >
	class AEGetKeyPtr_Getter
	{
		private:
			const AERecord&  theRecord;
			Mac::AEKeyword   theKeyword;
		
		public:
			AEGetKeyPtr_Getter( const AERecord&  record,
			                    Mac::AEKeyword   keyword ) : theRecord ( record  ),
			                                                 theKeyword( keyword )
			{
			}
			
			std::size_t size() const
			{
				AESizeOfKeyDesc_Result info = AESizeOfKeyDesc( theRecord, theKeyword );
				
				typedef typename Mac::DescType_scribe< type >::type scribe;
				
				if ( nucleus::scribe_has_static_size< scribe >::value )
				{
					return scribe::static_size;
				}
				
				ASSERT( info.typeCode == type );
				
				return info.dataSize;
			}
			
			void operator()( void *begin, void *end ) const
			{
				AEGetKeyPtr( theRecord, theKeyword, type, begin, iota::distance( begin, end ) );
			}
	};
	
	template < Mac::DescType type >
	inline typename DescType_result< type >::type
	AEGetKeyPtr( const AERecord&  record,
	             Mac::AEKeyword   keyword )
	{
		return DescType_get< type >( AEGetKeyPtr_Getter< type >( record, keyword ) );
	}
	
	template < Mac::AEKeyword key >
	inline typename AEKeyword_result< key >::type
	AEGetKeyPtr( const AERecord& record )
	{
		return AEKeyword_get< key >( AEGetKeyPtr_Getter< AEKeyword_DescType< key >::value >( record, key ) );
	}
	
	
	class AEPutParamPtr_Putter
	{
		private:
			Mac::AppleEvent&  theAppleEvent;
			Mac::AEKeyword    theKeyword;
			Mac::DescType     theType;
		
		public:
			AEPutParamPtr_Putter( Mac::AppleEvent&  appleEvent,
			                      Mac::AEKeyword    keyword,
			                      Mac::DescType     type )
			:
				theAppleEvent( appleEvent  ),
				theKeyword   ( keyword     ),
				theType      ( type        )
			{
			}
			
			void operator()( const void *begin, const void *end ) const
			{
				AEPutParamPtr( theAppleEvent, theKeyword, theType, begin, iota::distance( begin, end ) );
			}
	};
	
	template < Mac::DescType type >
	inline void AEPutParamPtr( Mac::AppleEvent&                           appleEvent,
	                           Mac::AEKeyword                             keyword,
	                           typename DescType_parameter< type >::type  data )
	{
		DescType_put< type >( data,
		                      AEPutParamPtr_Putter( appleEvent,
		                                            keyword,
		                                            type ) );
	}
	
	template < Mac::DescType type >
	inline void AEPutParamPtr( nucleus::owned< Mac::AppleEvent >&         appleEvent,
	                           Mac::AEKeyword                             keyword,
	                           typename DescType_parameter< type >::type  data )
	{
		DescType_put< type >( data,
		                      AEPutParamPtr_Putter( Detail::AEDescEditor< Mac::AppleEvent >( appleEvent ),
		                                            keyword,
		                                            type ) );
	}
	
	template < Mac::AEKeyword key >
	inline void AEPutParamPtr( Mac::AppleEvent&                           appleEvent,
	                           typename AEKeyword_parameter< key >::type  data )
	{
		AEKeyword_put< key >( data,
		                      AEPutParamPtr_Putter( appleEvent,
		                                            key,
		                                            AEKeyword_DescType< key >::value ) );
	}
	
	template < Mac::AEKeyword key >
	inline void AEPutParamPtr( nucleus::owned< Mac::AppleEvent >&         appleEvent,
	                           typename AEKeyword_parameter< key >::type  data )
	{
		AEPutParamPtr< key >( Detail::AEDescEditor< Mac::AppleEvent >( appleEvent ), data );
	}
	
	
	template < Mac::DescType type >
	class AEGetParamPtr_Getter
	{
		private:
			const Mac::AppleEvent&  theAppleEvent;
			Mac::AEKeyword          theKeyword;
		
		public:
			AEGetParamPtr_Getter( const Mac::AppleEvent&  appleEvent,
			                      Mac::AEKeyword          keyword )
			:
				theAppleEvent( appleEvent ),
				theKeyword   ( keyword    )
			{
			}
			
			std::size_t size() const
			{
				AESizeOfParam_Result info = AESizeOfParam( theAppleEvent, theKeyword );
				
				typedef typename Mac::DescType_scribe< type >::type scribe;
				
				if ( nucleus::scribe_has_static_size< scribe >::value )
				{
					return scribe::static_size;
				}
				
				ASSERT( info.typeCode == type );
				
				return info.dataSize;
			}
			
			void operator()( void *begin, void *end ) const
			{
				AEGetParamPtr( theAppleEvent, theKeyword, type, begin, iota::distance( begin, end ) );
			}
	};
	
	template < Mac::DescType type >
	inline typename DescType_result< type >::type
	//
	AEGetParamPtr( const Mac::AppleEvent&  appleEvent,
	               Mac::AEKeyword          keyword )
	{
		return DescType_get< type >( AEGetParamPtr_Getter< type >( appleEvent, keyword ) );
	}
	
	template < Mac::AEKeyword key >
	inline typename AEKeyword_result< key >::type
	//
	AEGetParamPtr( const Mac::AppleEvent& appleEvent )
	{
		return AEKeyword_get< key >( AEGetParamPtr_Getter< AEKeyword_DescType< key >::value >( appleEvent, key ) );
	}
	
	
	class AEPutAttributePtr_Putter
	{
		private:
			Mac::AppleEvent&  theAppleEvent;
			Mac::AEKeyword    theKeyword;
			Mac::DescType     theType;
		
		public:
			AEPutAttributePtr_Putter( Mac::AppleEvent&  appleEvent,
			                          Mac::AEKeyword    keyword,
			                          Mac::DescType     type )
			:
				theAppleEvent( appleEvent ),
				theKeyword   ( keyword    ),
				theType      ( type       )
			{
			}
			
			void operator()( const void *begin, const void *end ) const
			{
				AEPutAttributePtr( theAppleEvent, theKeyword, theType, begin, iota::distance( begin, end ) );
			}
	};
	
	template < Mac::DescType type >
	inline void AEPutAttributePtr( Mac::AppleEvent&                           appleEvent,
	                               Mac::AEKeyword                             keyword,
	                               typename DescType_parameter< type >::type  data )
	{
		DescType_put< type >( data,
		                      AEPutAttributePtr_Putter( appleEvent,
		                                                keyword,
		                                                type ) );
	}
	
	template < Mac::DescType type >
	inline void AEPutAttributePtr( nucleus::owned< Mac::AppleEvent >&         appleEvent,
	                               Mac::AEKeyword                             keyword,
	                               typename DescType_parameter< type >::type  data )
	{
		DescType_put< type >( data,
		                      AEPutAttributePtr_Putter( Detail::AEDescEditor< Mac::AppleEvent >( appleEvent ),
		                                                keyword,
		                                                type ) );
	}
	
	template < Mac::AEKeyword key >
	inline void AEPutAttributePtr( Mac::AppleEvent&                           appleEvent,
	                               typename AEKeyword_parameter< key >::type  data )
	{
		AEKeyword_put< key >( data,
		                      AEPutAttributePtr_Putter( appleEvent,
		                                                key,
		                                                AEKeyword_DescType< key >::value ) );
	}
	
	template < Mac::AEKeyword key >
	inline void AEPutAttributePtr( nucleus::owned< Mac::AppleEvent >&         appleEvent,
	                               typename AEKeyword_parameter< key >::type  data )
	{
		AEPutAttributePtr< key >( Detail::AEDescEditor< Mac::AppleEvent >( appleEvent ), data );
	}
	
	
	template < Mac::DescType type >
	class AEGetAttributePtr_Getter
	{
		private:
			const Mac::AppleEvent&  theAppleEvent;
			Mac::AEKeyword          theKeyword;
		
		public:
			AEGetAttributePtr_Getter( const Mac::AppleEvent&  appleEvent,
			                          Mac::AEKeyword          keyword )
			:
				theAppleEvent( appleEvent ),
				theKeyword   ( keyword    )
			{
			}
			
			std::size_t size() const
			{
				AESizeOfAttribute_Result info = AESizeOfAttribute( theAppleEvent, theKeyword );
				
				typedef typename Mac::DescType_scribe< type >::type scribe;
				
				if ( nucleus::scribe_has_static_size< scribe >::value )
				{
					return scribe::static_size;
				}
				
				ASSERT( info.typeCode == type );
				
				return info.dataSize;
			}
			
			void operator()( void *begin, void *end ) const
			{
				AEGetAttributePtr( theAppleEvent, theKeyword, type, begin, iota::distance( begin, end ) );
			}
	};
	
	template < Mac::DescType type >
	inline typename DescType_result< type >::type
	//
	AEGetAttributePtr( const Mac::AppleEvent&  appleEvent,
	                   Mac::AEKeyword          keyword )
	{
		return DescType_get< type >( AEGetAttributePtr_Getter< type >( appleEvent, keyword ) );
	}
	
	template < Mac::AEKeyword key >
	inline typename AEKeyword_result< key >::type
	//
	AEGetAttributePtr( const Mac::AppleEvent& appleEvent )
	{
		return AEKeyword_get< key >( AEGetAttributePtr_Getter< AEKeyword_DescType< key >::value >( appleEvent, key ) );
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
	
	template < Mac::DescType type >
	inline typename DescType_result< type >::type
	AEGetDescData( const AEDesc&  desc,
	               Mac::DescType  requiredType = type )
	{
		ASSERT( requiredType == Mac::typeWildCard || requiredType == desc.descriptorType );
		
		(void) requiredType;
		
		return DescType_get< type >( AEGetDescData_Getter( desc ) );
	}
	
	
	class AEReplaceDescData_Putter
	{
		private:
			Mac::DescType                        theType;
			nucleus::owned< Mac::AEDesc_Data >&  theDesc;
		
		public:
			AEReplaceDescData_Putter( Mac::DescType                        type,
			                          nucleus::owned< Mac::AEDesc_Data >&  desc )
			:
				theType( type ),
				theDesc( desc )
			{
			}
			
			void operator()( const void *begin, const void *end ) const
			{
				AEReplaceDescData( theType, begin, iota::distance( begin, end ), theDesc );
			}
	};
	
	template < Mac::DescType type >
	inline void AEReplaceDescData( typename DescType_parameter< type >::type  data,
	                               AEDesc&                                    result )
	{
		DescType_put< type >( data,
		                      AEReplaceDescData_Putter( type, result ) );
	}
	
	template < Mac::DescType type >
	inline void AEReplaceDescData( typename DescType_parameter< type >::type  data,
	                               nucleus::owned< Mac::AEDesc_Data >&        result )
	{
		DescType_put< type >( data,
		                      AEReplaceDescData_Putter( type,
		                                                Detail::AEDescEditor< Mac::AEDesc_Data >( result ) ) );
	}
	
}

#endif

