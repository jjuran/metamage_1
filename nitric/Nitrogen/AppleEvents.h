//	=============
//	AppleEvents.h
//	=============

#ifndef NITROGEN_APPLEEVENTS_H
#define NITROGEN_APPLEEVENTS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __APPLEEVENTS__
#include FRAMEWORK_HEADER(AE,AppleEvents.h)
#endif

// Nitrogen core
#ifndef NUCLEUS_OBJECTPARAMETERTRAITS_H
#include "Nucleus/ObjectParameterTraits.h"
#endif
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

// Nitrogen Carbon support
#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif
#ifndef NITROGEN_AEINTERACTION_H
#include "Nitrogen/AEInteraction.h"
#endif


namespace Nitrogen
{
	
	static const AEKeyword keyDirectObject        = AEKeyword( ::keyDirectObject        );
	static const AEKeyword keyErrorNumber         = AEKeyword( ::keyErrorNumber         );
	static const AEKeyword keyErrorString         = AEKeyword( ::keyErrorString         );
	static const AEKeyword keyProcessSerialNumber = AEKeyword( ::keyProcessSerialNumber );
	static const AEKeyword keyPreDispatch         = AEKeyword( ::keyPreDispatch         );
	static const AEKeyword keySelectProc          = AEKeyword( ::keySelectProc          );
                      
	static const AEKeyword keyAERecorderCount     = AEKeyword( ::keyAERecorderCount     );
                      
	static const AEKeyword keyAEVersion           = AEKeyword( ::keyAEVersion           );
	
	
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
	
	typedef Nucleus::Selector< class AEEventSource_Tag, ::AEEventSource >::Type AEEventSource;
	
	static const AEEventSource kAEUnknownSource = AEEventSource( ::kAEUnknownSource );
	static const AEEventSource kAEDirectCall    = AEEventSource( ::kAEDirectCall    );
	static const AEEventSource kAESameProcess   = AEEventSource( ::kAESameProcess   );
	static const AEEventSource kAELocalProcess  = AEEventSource( ::kAELocalProcess  );
	static const AEEventSource kAERemoteProcess = AEEventSource( ::kAERemoteProcess );
	
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

namespace Nucleus
  {
   template <>
   struct Disposer< Nitrogen::AEEventHandler >: public std::unary_function< Nitrogen::AEEventHandler, void >,
                                                private Nitrogen::DefaultDestructionOSStatusPolicy
     {
      void operator()( const Nitrogen::AEEventHandler& installation ) const
        {
         HandleDestructionOSStatus( ::AERemoveEventHandler( installation.theAEEventClass,
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
		static pascal OSErr Adapter( AppleEvent const*  appleEvent,
		                             AppleEvent      *  reply,
		                             long               refCon )
		{
			try
			{
				handler( *appleEvent,
				         *reply,
				         reinterpret_cast< RefConType >( refCon ) );
			}
			catch ( OSStatus err )
			{
				return err.Get();
			}
			
			return noErr;
		}
	};
	
	template < AEEventHandler_RefCon_Traits< void >::ProcPtr handler >
	struct AEEventHandler_Callback< void, handler >
	{
		static pascal OSErr Adapter( AppleEvent const*  appleEvent,
		                             AppleEvent      *  reply,
		                             long )
		{
			try
			{
				handler( *appleEvent,
				         *reply );
			}
			catch ( OSStatus err )
			{
				return err.Get();
			}
			
			return noErr;
		}
	};
	
	// Level 0
	
   Nucleus::Owned< AEEventHandler >
   AEInstallEventHandler( const AEEventHandler& );
   
   inline Nucleus::Owned< AEEventHandler >
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
   inline Nucleus::Owned< AEEventHandler >
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
	inline Nucleus::Owned< AEEventHandler >
	AEInstallEventHandler( AEEventClass                                             theAEEventClass,
	                       AEEventID                                                theAEEventID,
	                       typename Nucleus::ObjectParameterTraits< Object >::Type  handlerRefCon   = typename Nucleus::ObjectParameterTraits< Object >::Type(),
	                       Boolean                                                  isSysHandler    = false )
	{
		return AEInstallEventHandler< AEEventHandler_Callback< Object, handler >::Adapter >
		(
			theAEEventClass, 
			theAEEventID, 
			Nucleus::ObjectParameterTraits< Object >::ConvertToPointer( handlerRefCon ), 
			isSysHandler
		);
	}
	
	// With default handlerRefCon but supplied isSysHandler
	template < class Object, typename AEEventHandler_RefCon_Traits< Object >::ProcPtr handler >
	inline Nucleus::Owned< AEEventHandler >
	AEInstallEventHandler( AEEventClass  theAEEventClass,
	                       AEEventID     theAEEventID,
	                       Boolean       isSysHandler )
	{
		typedef typename Nucleus::ObjectParameterTraits< Object >::Type ObjectType;
		
		return AEInstallEventHandler< AEEventHandler_Callback< void, handler >::Adapter >
		(
			theAEEventClass,
			theAEEventID,
			Nucleus::ObjectParameterTraits< Object >::ConvertToPointer( ObjectType() ),
			isSysHandler
		);
	}
	
	// Same as above, but void parameter is omitted.
	template < typename AEEventHandler_RefCon_Traits< void >::ProcPtr handler >
	inline Nucleus::Owned< AEEventHandler >
	AEInstallEventHandler( AEEventClass  theAEEventClass,
	                       AEEventID     theAEEventID,
	                       Boolean       isSysHandler    = false )
	{
		return AEInstallEventHandler< void, handler >( theAEEventClass,
		                                               theAEEventID,
		                                               isSysHandler );
	}
	
   void AERemoveEventHandler( Nucleus::Owned<AEEventHandler> );

   typedef AEEventHandler AEGetEventHandler_Result;
   
   AEEventHandler AEGetEventHandler( AEEventClass  theAEEventClass,
                                     AEEventID     theAEEventID,
                                     bool          isSysHandler = false );
}

#endif

