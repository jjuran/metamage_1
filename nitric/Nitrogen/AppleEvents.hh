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
	
	static const AEEventClass kCoreEventClass = AEEventClass( ::kCoreEventClass );
	
	static const AEEventID kAEOpenApplication = AEEventID( ::kAEOpenApplication );
	static const AEEventID kAEOpenDocuments   = AEEventID( ::kAEOpenDocuments   );
	static const AEEventID kAEPrintDocuments  = AEEventID( ::kAEPrintDocuments  );
	static const AEEventID kAEQuitApplication = AEEventID( ::kAEQuitApplication );
	static const AEEventID kAEAnswer          = AEEventID( ::kAEAnswer          );
	static const AEEventID kAEApplicationDied = AEEventID( ::kAEApplicationDied );
	static const AEEventID kAEShowPreferences = AEEventID( ::kAEShowPreferences );
	
	
	static const AEEventID kAEStartRecording       = AEEventID( ::kAEStartRecording       );
	static const AEEventID kAEStopRecording        = AEEventID( ::kAEStopRecording        );
	static const AEEventID kAENotifyStartRecording = AEEventID( ::kAENotifyStartRecording );
	static const AEEventID kAENotifyStopRecording  = AEEventID( ::kAENotifyStopRecording  );
	static const AEEventID kAENotifyRecording      = AEEventID( ::kAENotifyRecording      );
	
	enum AEEventSource
	{
		kAEUnknownSource = ::kAEUnknownSource,
		kAEDirectCall    = ::kAEDirectCall,
		kAESameProcess   = ::kAESameProcess,
		kAELocalProcess  = ::kAELocalProcess,
		kAERemoteProcess = ::kAERemoteProcess,
		
		kAEEventSource_Max = nucleus::enumeration_traits< ::AEEventSource >::max
	};
	
	// In AppleEvents.h due to dependency on enum AEEventSource.
	template <> struct AEKeyword_Traits< keyEventSourceAttr > : Integer_AEKeyword_Traits< AEEventSource, SInt16 > {};
	
	template <> struct AEKeyword_Traits< keyErrorNumber > : Integer_AEKeyword_Traits< OSStatus, ::OSErr > {};
	
	template <> struct AEKeyword_Traits< keyErrorString > : Char_AEKeyword_Traits< char > {};
	
   struct AEEventHandler
     {
      AEEventClass        theAEEventClass;
      AEEventID           theAEEventID;
      AEEventHandlerUPP   handler;
      RefCon              handlerRefCon;
      bool                isSysHandler;
      
      AEEventHandler();
      
      AEEventHandler( AEEventClass      theAEEventClass,
                      AEEventID         theAEEventID,
                      AEEventHandlerUPP handler,
                      RefCon            handlerRefCon = RefCon(),
                      bool              isSysHandler  = false )
        : theAEEventClass( theAEEventClass ),
          theAEEventID( theAEEventID ),
          handler( handler ),
          handlerRefCon( handlerRefCon ),
          isSysHandler( isSysHandler )
        {}
     };
   
   bool operator==( const AEEventHandler&, const AEEventHandler& );
   inline bool operator!=( const AEEventHandler& a, const AEEventHandler& b )    { return !( a == b ); }
   
  }

namespace nucleus
  {
   template <>
   struct disposer< Nitrogen::AEEventHandler >: public std::unary_function< Nitrogen::AEEventHandler, void >
     {
      void operator()( const Nitrogen::AEEventHandler& installation ) const
        {
         ::Nitrogen::HandleDestructionOSStatus( ::AERemoveEventHandler( installation.theAEEventClass,
                                                                        installation.theAEEventID,
                                                                        installation.handler,
                                                                        installation.isSysHandler ) );
        }
     };
  }

namespace Nitrogen
  {
	
	//typedef void ( *AEEventHandlerProcPtr )( const AppleEvent& appleEvent, AppleEvent& reply, RefCon refCon );
	
	template < class RefConType >
	struct AEEventHandler_RefCon_Traits
	{
		typedef RefConType RefCon;
		typedef void ( *ProcPtr )( const AppleEvent& appleEvent, AppleEvent& reply, RefConType refCon );
	};
	
	template <>
	struct AEEventHandler_RefCon_Traits< void >
	{
		typedef void RefCon;
		typedef void ( *ProcPtr )( const AppleEvent& appleEvent, AppleEvent& reply );
	};
	
	template < class RefConType,
	           typename AEEventHandler_RefCon_Traits< RefConType >::ProcPtr handler >
	struct AEEventHandler_Callback
	{
		static pascal OSErr Adapter( ::AppleEvent const*  appleEvent,
		                             ::AppleEvent      *  reply,
		                             long               refCon )
		{
			try
			{
				handler( static_cast< AppleEvent const& >( *appleEvent ),
				         static_cast< AppleEvent      & >( *reply      ),
				         reinterpret_cast< RefConType >( refCon ) );
			}
			catch ( ... )
			{
				return ConvertTheExceptionToOSStatus( errAEEventFailed );
			}
			
			return noErr;
		}
	};
	
	template < AEEventHandler_RefCon_Traits< void >::ProcPtr handler >
	struct AEEventHandler_Callback< void, handler >
	{
		static pascal OSErr Adapter( ::AppleEvent const*  appleEvent,
		                             ::AppleEvent      *  reply,
		                             long )
		{
			try
			{
				handler( static_cast< AppleEvent const& >( *appleEvent ),
				         static_cast< AppleEvent      & >( *reply      ) );
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
   AEInstallEventHandler( const AEEventHandler& );
   
   inline nucleus::owned< AEEventHandler >
   AEInstallEventHandler( AEEventClass       theAEEventClass,
                          AEEventID          theAEEventID,
                          AEEventHandlerUPP  handler,
                          RefCon             handlerRefCon = RefCon(),
                          Boolean            isSysHandler  = false )
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
   AEInstallEventHandler( AEEventClass       theAEEventClass,
                          AEEventID          theAEEventID,
                          RefCon             handlerRefCon = RefCon(),
                          Boolean            isSysHandler  = false )
     {
      return AEInstallEventHandler( AEEventHandler( theAEEventClass,
                                                    theAEEventID,
                                                    StaticUPP< AEEventHandlerUPP, handler >(),
                                                    handlerRefCon,
                                                    isSysHandler ) );
     }
	
	// Level 2, refcon type specified
	
	template < class Object,
	           typename AEEventHandler_RefCon_Traits< Object >::ProcPtr handler >
	inline nucleus::owned< AEEventHandler >
	AEInstallEventHandler( AEEventClass                                               theAEEventClass,
	                       AEEventID                                                  theAEEventID,
	                       typename nucleus::object_parameter_traits< Object >::type  handlerRefCon   = typename nucleus::object_parameter_traits< Object >::type(),
	                       Boolean                                                    isSysHandler    = false )
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
	template < class Object, typename AEEventHandler_RefCon_Traits< Object >::ProcPtr handler >
	inline nucleus::owned< AEEventHandler >
	AEInstallEventHandler( AEEventClass  theAEEventClass,
	                       AEEventID     theAEEventID,
	                       Boolean       isSysHandler )
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
	template < typename AEEventHandler_RefCon_Traits< void >::ProcPtr handler >
	inline nucleus::owned< AEEventHandler >
	AEInstallEventHandler( AEEventClass  theAEEventClass,
	                       AEEventID     theAEEventID,
	                       Boolean       isSysHandler    = false )
	{
		return AEInstallEventHandler< void, handler >( theAEEventClass,
		                                               theAEEventID,
		                                               isSysHandler );
	}
	
   void AERemoveEventHandler( nucleus::owned<AEEventHandler> );

   typedef AEEventHandler AEGetEventHandler_Result;
   
   AEEventHandler AEGetEventHandler( AEEventClass  theAEEventClass,
                                     AEEventID     theAEEventID,
                                     bool          isSysHandler = false );
}

#endif

