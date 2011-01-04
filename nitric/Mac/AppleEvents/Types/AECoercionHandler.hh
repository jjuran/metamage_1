/*
	Mac/AppleEvents/Types/AECoercionHandler.hh
	------------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AECOERCIONHANDLER_HH
#define MAC_APPLEEVENTS_TYPES_AECOERCIONHANDLER_HH

#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

// Annex
#ifndef ANNEX_MACTYPES_H
#include "Annex/MacTypes.h"
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif

// Nitrogen
#ifndef MAC_APPLEEVENTS_FUNCTIONS_AEDISPOSEDESC_HH
#include "Mac/AppleEvents/Functions/AEDisposeDesc.hh"
#endif
#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPE_HH
#include "Mac/AppleEvents/Types/DescType.hh"
#endif
#ifndef MAC_MIXEDMODE_TYPES_UPP_HH
#include "Mac/MixedMode/Types/UPP.hh"
#endif
#ifndef MAC_TOOLBOX_UTILITIES_THROWOSSTATUS_HH
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"
#endif


namespace Mac
{
	
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
	                                   ::SRefCon           handlerRefCon,
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
	                                  ::SRefCon       handlerRefCon,
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
		::SRefCon             handlerRefCon;
		bool                  fromTypeIsDesc;
		bool                  isSysHandler;
		
		AECoercionHandler();
		
		AECoercionHandler( DescType              fromType,
		                   DescType              toType,
		                   AECoercionHandlerUPP  handler,
		                   ::SRefCon             handlerRefCon,
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
		                   ::SRefCon        handlerRefCon,
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
		                   ::SRefCon        handlerRefCon,
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
	struct disposer< Mac::AECoercionHandler >
	{
		typedef Mac::AECoercionHandler  argument_type;
		typedef void                    result_type;
		
		void operator()( const Mac::AECoercionHandler& installation ) const
		{
			(void) ::AERemoveCoercionHandler( installation.fromType,
			                                  installation.toType,
			                                  installation.handler,
			                                  installation.isSysHandler );
		}
	};
	
	template < class Disposer >
	struct aliveness_traits< ::Mac::AECoercionHandler, Disposer >
	{
		struct aliveness_test
		{
			static bool is_live( const ::Mac::AECoercionHandler& coercion )
			{
				return coercion.handler != 0;
			}
		};
	};
	
}

namespace Mac
{
	
	// Level 0
	
	nucleus::owned< AECoercionHandler > AEInstallCoercionHandler( const AECoercionHandler& toInstall );
	
	inline nucleus::owned< AECoercionHandler >
	//
	AEInstallCoercionHandler( DescType         fromType,
	                          DescType         toType,
	                          AECoerceDescUPP  handler,
	                          long             handlerRefCon = 0,
	                          bool             isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    handler,
		                                                    (::SRefCon) handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	inline nucleus::owned< AECoercionHandler >
	//
	AEInstallCoercionHandler( DescType        fromType,
	                          DescType        toType,
	                          AECoercePtrUPP  handler,
	                          long            handlerRefCon = 0,
	                          bool            isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    handler,
		                                                    (::SRefCon) handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	inline nucleus::owned< AECoercionHandler >
	//
	AEInstallCoercionHandler( DescType         fromType,
	                          DescType         toType,
	                          AECoerceDescUPP  handler,
	                          void*            handlerRefCon,
	                          bool             isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    handler,
		                                                    (::SRefCon) handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	inline nucleus::owned< AECoercionHandler >
	//
	AEInstallCoercionHandler( DescType        fromType,
	                          DescType        toType,
	                          AECoercePtrUPP  handler,
	                          void*           handlerRefCon,
	                          bool            isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    handler,
		                                                    (::SRefCon) handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	// Level 1
	
	template < typename AECoerceDescUPP::ProcPtr handler, class RefCon >
	nucleus::owned< AECoercionHandler >
	//
	AEInstallCoercionHandler( DescType       fromType,
	                          DescType       toType,
	                          RefCon         handlerRefCon = RefCon(),
	                          bool           isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    StaticUPP< AECoerceDescUPP, handler >(),
		                                                    (::SRefCon) handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	template < typename AECoercePtrUPP::ProcPtr handler, class RefCon >
	nucleus::owned< AECoercionHandler >
	//
	AEInstallCoercionHandler( DescType       fromType,
	                          DescType       toType,
	                          RefCon         handlerRefCon = RefCon(),
	                          bool           isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    StaticUPP< AECoercePtrUPP, handler >(),
		                                                    (::SRefCon) handlerRefCon,
		                                                    isSysHandler ) );
	}
	
	template < typename AECoerceDescUPP::ProcPtr handler >
	nucleus::owned< AECoercionHandler >
	//
	AEInstallCoercionHandler( DescType       fromType,
	                          DescType       toType,
	                          bool           isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    StaticUPP< AECoerceDescUPP, handler >(),
		                                                    0,
		                                                    isSysHandler ) );
	}
	
	template < typename AECoercePtrUPP::ProcPtr handler >
	nucleus::owned< AECoercionHandler >
	//
	AEInstallCoercionHandler( DescType       fromType,
	                          DescType       toType,
	                          bool           isSysHandler  = false )
	{
		return AEInstallCoercionHandler( AECoercionHandler( fromType,
		                                                    toType,
		                                                    StaticUPP< AECoercePtrUPP, handler >(),
		                                                    0,
		                                                    isSysHandler ) );
	}
	
	inline void AERemoveCoercionHandler( nucleus::owned< AECoercionHandler > )  {}
	
	typedef AECoercionHandler AEGetCoercionHandler_Result;
	
	AECoercionHandler AEGetCoercionHandler( DescType  fromType,
	                                        DescType  toType,
	                                        bool      isSysHandler );
	
}

#endif

