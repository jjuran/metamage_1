// AppleEvents.h

#ifndef NITROGEN_APPLEEVENTS_H
#define NITROGEN_APPLEEVENTS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __APPLEEVENTS__
#include FRAMEWORK_HEADER(AE,AppleEvents.h)
#endif
#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

namespace Nitrogen
  {
	
	// Keywords for Apple event parameters
	static const AEKeyword keyDirectObject        = AEKeyword::Make( ::keyDirectObject        );
	static const AEKeyword keyErrorNumber         = AEKeyword::Make( ::keyErrorNumber         );
	static const AEKeyword keyErrorString         = AEKeyword::Make( ::keyErrorString         );
	static const AEKeyword keyProcessSerialNumber = AEKeyword::Make( ::keyProcessSerialNumber );
	static const AEKeyword keyPreDispatch         = AEKeyword::Make( ::keyPreDispatch         );
	static const AEKeyword keySelectProc          = AEKeyword::Make( ::keySelectProc          );
	static const AEKeyword keyAERecorderCount     = AEKeyword::Make( ::keyAERecorderCount     );
	static const AEKeyword keyAEVersion           = AEKeyword::Make( ::keyAEVersion           );
	
	// Event Class
	static const AEEventClass kCoreEventClass = AEEventClass::Make( ::kCoreEventClass );
	
	// Event ID's
	static const AEEventID kAEOpenApplication = AEEventID::Make( ::kAEOpenApplication );
	static const AEEventID kAEOpenDocuments   = AEEventID::Make( ::kAEOpenDocuments   );
	static const AEEventID kAEPrintDocuments  = AEEventID::Make( ::kAEPrintDocuments  );
	static const AEEventID kAEQuitApplication = AEEventID::Make( ::kAEQuitApplication );
	static const AEEventID kAEAnswer          = AEEventID::Make( ::kAEAnswer          );
	static const AEEventID kAEApplicationDied = AEEventID::Make( ::kAEApplicationDied );
	static const AEEventID kAEShowPreferences = AEEventID::Make( ::kAEShowPreferences );
	
	// Constants for recording
	// What type are these?
	
	class AEEventSource_Tag {};
	typedef SelectorType< AEEventSource_Tag, ::AEEventSource > AEEventSource;
	
	static const AEEventSource kAEUnknownSource = AEEventSource::Make( ::kAEUnknownSource );
	static const AEEventSource kAEDirectCall    = AEEventSource::Make( ::kAEDirectCall    );
	static const AEEventSource kAESameProcess   = AEEventSource::Make( ::kAESameProcess   );
	static const AEEventSource kAELocalProcess  = AEEventSource::Make( ::kAELocalProcess  );
	static const AEEventSource kAERemoteProcess = AEEventSource::Make( ::kAERemoteProcess );
	
	
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
                      RefCon            handlerRefCon,
                      bool              isSysHandler )
        : theAEEventClass( theAEEventClass ),
          theAEEventID( theAEEventID ),
          handler( handler ),
          handlerRefCon( handlerRefCon ),
          isSysHandler( isSysHandler )
        {}
     };
   
   bool operator==( const AEEventHandler&, const AEEventHandler& );
   inline bool operator!=( const AEEventHandler& a, const AEEventHandler& b )    { return !( a == b ); }
   
   template <>
   struct Disposer< AEEventHandler >: public std::unary_function< AEEventHandler, void >,
                                      private DefaultDestructionOSStatusPolicy
     {
      void operator()( const AEEventHandler& installation ) const
        {
         DefaultDestructionOSStatusPolicy::HandleDestructionOSStatus( ::AERemoveEventHandler( installation.theAEEventClass,
                                                                                              installation.theAEEventID,
                                                                                              installation.handler,
                                                                                              installation.isSysHandler ) );
        }
     };

   Owned<AEEventHandler>
   AEInstallEventHandler( const AEEventHandler& );
   
   inline Owned<AEEventHandler>
   AEInstallEventHandler( AEEventClass       theAEEventClass,
                          AEEventID          theAEEventID,
                          AEEventHandlerUPP  handler,
                          RefCon             handlerRefCon,
                          Boolean            isSysHandler )
     {
      return AEInstallEventHandler( AEEventHandler( theAEEventClass,
                                                    theAEEventID,
                                                    handler,
                                                    handlerRefCon,
                                                    isSysHandler ) );
     }

   template < typename AEEventHandlerUPP::ProcPtr handler >
   inline  Owned<AEEventHandler>
   AEInstallEventHandler( AEEventClass       theAEEventClass,
                          AEEventID          theAEEventID,
                          RefCon             handlerRefCon,
                          Boolean            isSysHandler )
     {
      return AEInstallEventHandler( AEEventHandler( theAEEventClass,
                                                    theAEEventID,
                                                    StaticUPP< AEEventHandlerUPP, handler >(),
                                                    handlerRefCon,
                                                    isSysHandler ) );
     }
   
   void AERemoveEventHandler( Owned<AEEventHandler> );

   typedef AEEventHandler AEGetEventHandler_Result;
   AEEventHandler AEGetEventHandler( AEEventClass theAEEventClass,
                                     AEEventID    theAEEventID,
                                     bool         isSysHandler );
  }

#endif
