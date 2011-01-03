// Nitrogen/AppleEvents.hh
// -----------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_APPLEEVENTS_HH
#define NITROGEN_APPLEEVENTS_HH

#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif

// nucleus
#ifndef NUCLEUS_OBJECTPARAMETERTRAITS_HH
#include "nucleus/object_parameter_traits.hh"
#endif

// Nitrogen
#ifndef NITROGEN_AEDATAMODEL_HH
#include "Nitrogen/AEDataModel.hh"
#endif
#ifndef NITROGEN_AEINTERACTION_HH
#include "Nitrogen/AEInteraction.hh"
#endif


namespace Nitrogen
{
	
	struct AEEventHandler
	{
		AEEventClass       theAEEventClass;
		AEEventID          theAEEventID;
		AEEventHandlerUPP  handler;
		RefCon             handlerRefCon;
		bool               isSysHandler;
		
		AEEventHandler();
		
		AEEventHandler( AEEventClass       theAEEventClass,
		                AEEventID          theAEEventID,
		                AEEventHandlerUPP  handler,
		                RefCon             handlerRefCon = RefCon(),
		                bool               isSysHandler  = false )
		:
			theAEEventClass( theAEEventClass ),
			theAEEventID   ( theAEEventID    ),
			handler        ( handler         ),
			handlerRefCon  ( handlerRefCon   ),
			isSysHandler   ( isSysHandler    )
		{
		}
	};
	
	bool operator==( const AEEventHandler&, const AEEventHandler& );
	
	inline bool operator!=( const AEEventHandler& a, const AEEventHandler& b )
	{
		return !( a == b );
	}
	
}

namespace nucleus
{
	
	template <>
	struct disposer< Nitrogen::AEEventHandler >
	{
		typedef Nitrogen::AEEventHandler  argument_type;
		typedef void                      result_type;
		
		void operator()( const Nitrogen::AEEventHandler& installation ) const
		{
			::Nitrogen::HandleDestructionOSStatus( ::AERemoveEventHandler( installation.theAEEventClass,
			                                                               installation.theAEEventID,
			                                                               installation.handler,
			                                                               installation.isSysHandler ) );
		}
	};
	
	template < class Disposer >
	struct aliveness_traits< ::Nitrogen::AEEventHandler, Disposer >
	{
		struct aliveness_test
		{
			static bool is_live( const ::Nitrogen::AEEventHandler& ae )
			{
				return ae.handler != 0;
			}
		};
	};
	
}

namespace Nitrogen
{
	
	//typedef void ( *AEEventHandlerProcPtr )( const AppleEvent& appleEvent, AppleEvent& reply, RefCon refCon );
	
	template < class RefCon >
	struct RefCon_AEEventHandler
	{
		typedef void ( *type )( const Mac::AppleEvent& appleEvent, Mac::AppleEvent& reply, RefCon refCon );
	};
	
	template <>
	struct RefCon_AEEventHandler< void >
	{
		typedef void ( *type )( const Mac::AppleEvent& appleEvent, Mac::AppleEvent& reply );
	};
	
	template < class RefCon,
	           typename RefCon_AEEventHandler< RefCon >::type handler >
	struct AEEventHandler_Callback
	{
		static pascal OSErr Adapter( ::AppleEvent const*  appleEvent,
		                             ::AppleEvent      *  reply,
		                             long                 refCon )
		{
			try
			{
				handler( static_cast< Mac::AppleEvent const& >( *appleEvent ),
				         static_cast< Mac::AppleEvent      & >( *reply      ),
				         reinterpret_cast< RefCon >           ( refCon      ) );
			}
			catch ( ... )
			{
				return ConvertTheExceptionToOSStatus( errAEEventFailed );
			}
			
			return noErr;
		}
	};
	
	template < RefCon_AEEventHandler< void >::type handler >
	struct AEEventHandler_Callback< void, handler >
	{
		static pascal OSErr Adapter( ::AppleEvent const*  appleEvent,
		                             ::AppleEvent      *  reply,
		                             long )
		{
			try
			{
				handler( static_cast< Mac::AppleEvent const& >( *appleEvent ),
				         static_cast< Mac::AppleEvent      & >( *reply      ) );
			}
			catch ( ... )
			{
				return ConvertTheExceptionToOSStatus( errAEEventFailed );
			}
			
			return noErr;
		}
	};
	
	// Level 0
	
	nucleus::owned< AEEventHandler >
	//
	AEInstallEventHandler( const AEEventHandler& );
	
	inline nucleus::owned< AEEventHandler >
	//
	AEInstallEventHandler( AEEventClass       theAEEventClass,
	                       AEEventID          theAEEventID,
	                       AEEventHandlerUPP  handler,
	                       RefCon             handlerRefCon = RefCon(),
	                       bool               isSysHandler  = false )
	{
		return AEInstallEventHandler( AEEventHandler( theAEEventClass,
		                                              theAEEventID,
		                                              handler,
		                                              handlerRefCon,
		                                              isSysHandler ) );
	}
	
	// Level 1
	
	template < typename AEEventHandlerUPP::ProcPtr handler >
	inline nucleus::owned< AEEventHandler >
	//
	AEInstallEventHandler( AEEventClass       theAEEventClass,
	                       AEEventID          theAEEventID,
	                       RefCon             handlerRefCon = RefCon(),
	                       bool               isSysHandler  = false )
	{
		return AEInstallEventHandler( AEEventHandler( theAEEventClass,
		                                              theAEEventID,
		                                              StaticUPP< AEEventHandlerUPP, handler >(),
		                                              handlerRefCon,
		                                              isSysHandler ) );
	}
	
	// Level 2, refcon type specified
	
	template < class Object,
	           typename RefCon_AEEventHandler< Object >::type handler >
	inline nucleus::owned< AEEventHandler >
	//
	AEInstallEventHandler( AEEventClass                                               theAEEventClass,
	                       AEEventID                                                  theAEEventID,
	                       typename nucleus::object_parameter_traits< Object >::type  handlerRefCon   = typename nucleus::object_parameter_traits< Object >::type(),
	                       bool                                                       isSysHandler    = false )
	{
		return AEInstallEventHandler< AEEventHandler_Callback< Object, handler >::Adapter >
		(
			theAEEventClass, 
			theAEEventID, 
			nucleus::object_parameter_traits< Object >::convert_to_pointer( handlerRefCon ), 
			isSysHandler
		);
	}
	
	// With default handlerRefCon but supplied isSysHandler
	template < class Object, typename RefCon_AEEventHandler< Object >::type handler >
	inline nucleus::owned< AEEventHandler >
	//
	AEInstallEventHandler( AEEventClass  theAEEventClass,
	                       AEEventID     theAEEventID,
	                       bool          isSysHandler )
	{
		typedef typename nucleus::object_parameter_traits< Object >::type ObjectType;
		
		return AEInstallEventHandler< AEEventHandler_Callback< void, handler >::Adapter >
		(
			theAEEventClass,
			theAEEventID,
			nucleus::object_parameter_traits< Object >::convert_to_pointer( ObjectType() ),
			isSysHandler
		);
	}
	
	// Same as above, but void parameter is omitted.
	template < typename RefCon_AEEventHandler< void >::type handler >
	inline nucleus::owned< AEEventHandler >
	//
	AEInstallEventHandler( AEEventClass  theAEEventClass,
	                       AEEventID     theAEEventID,
	                       bool          isSysHandler    = false )
	{
		return AEInstallEventHandler< void, handler >( theAEEventClass,
		                                               theAEEventID,
		                                               isSysHandler );
	}
	
	void AERemoveEventHandler( nucleus::owned< AEEventHandler > );
	
	typedef AEEventHandler AEGetEventHandler_Result;
	
	AEEventHandler AEGetEventHandler( AEEventClass  theAEEventClass,
	                                  AEEventID     theAEEventID,
	                                  bool          isSysHandler = false );
}

#endif

