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
#ifndef NITROGEN_AEINTERACTION_H
#include "Nitrogen/AEInteraction.h"
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif
#ifndef NITROGEN_OBJECTPARAMETERTRAITS_H
#include "Nitrogen/ObjectParameterTraits.h"
#endif

namespace Nitrogen
  {
	
	// Keywords for Apple event parameters
	inline AEKeyword KeyDirectObject       ()  { return AEKeyword::Make( keyDirectObject        ); }
	inline AEKeyword KeyErrorNumber        ()  { return AEKeyword::Make( keyErrorNumber         ); }
	inline AEKeyword KeyErrorString        ()  { return AEKeyword::Make( keyErrorString         ); }
	inline AEKeyword KeyProcessSerialNumber()  { return AEKeyword::Make( keyProcessSerialNumber ); }
	inline AEKeyword KeyPreDispatch        ()  { return AEKeyword::Make( keyPreDispatch         ); }
	inline AEKeyword KeySelectProc         ()  { return AEKeyword::Make( keySelectProc          ); }
	inline AEKeyword KeyAERecorderCount    ()  { return AEKeyword::Make( keyAERecorderCount     ); }
	inline AEKeyword KeyAEVersion          ()  { return AEKeyword::Make( keyAEVersion           ); }
	
	// Event Class
	inline AEEventClass CoreEventClass()  { return AEEventClass::Make( kCoreEventClass ); }
	
	// Event ID's
	inline AEEventID AEOpenApplication()  { return AEEventID::Make( kAEOpenApplication ); }
	inline AEEventID AEOpenDocuments  ()  { return AEEventID::Make( kAEOpenDocuments   ); }
	inline AEEventID AEPrintDocuments ()  { return AEEventID::Make( kAEPrintDocuments  ); }
	inline AEEventID AEQuitApplication()  { return AEEventID::Make( kAEQuitApplication ); }
	inline AEEventID AEAnswer         ()  { return AEEventID::Make( kAEAnswer          ); }
	inline AEEventID AEApplicationDied()  { return AEEventID::Make( kAEApplicationDied ); }
	inline AEEventID AEShowPreferences()  { return AEEventID::Make( kAEShowPreferences ); }
	
	// Constants for recording
	inline AEEventID AEStartRecording      ()  { return AEEventID::Make( kAEStartRecording       ); }
	inline AEEventID AEStopRecording       ()  { return AEEventID::Make( kAEStopRecording        ); }
	inline AEEventID AENotifyStartRecording()  { return AEEventID::Make( kAENotifyStartRecording ); }
	inline AEEventID AENotifyStopRecording ()  { return AEEventID::Make( kAENotifyStopRecording  ); }
	inline AEEventID AENotifyRecording     ()  { return AEEventID::Make( kAENotifyRecording      ); }
	
	class AEEventSource_Tag {};
	typedef SelectorType< AEEventSource_Tag, ::AEEventSource > AEEventSource;
	
	inline AEEventSource AEUnknownSource()  { return AEEventSource::Make( kAEUnknownSource ); }
	inline AEEventSource AEDirectCall   ()  { return AEEventSource::Make( kAEDirectCall    ); }
	inline AEEventSource AESameProcess  ()  { return AEEventSource::Make( kAESameProcess   ); }
	inline AEEventSource AELocalProcess ()  { return AEEventSource::Make( kAELocalProcess  ); }
	inline AEEventSource AERemoteProcess()  { return AEEventSource::Make( kAERemoteProcess ); }
	
	/*
	class AEObjectClass_Tag {};
	typedef SelectorType< AEObjectClass_Tag, ::DescType > AEObjectClass;
	
	class AEPropertyID_Tag {};
	typedef SelectorType< AEPropertyID_Tag, ::DescType > AEPropertyID;
	*/
	
	typedef DescType AEObjectClass, AEPropertyID;
	
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
                      bool              isSysHandler = false )
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
         HandleDestructionOSStatus( ::AERemoveEventHandler( installation.theAEEventClass,
                                                            installation.theAEEventID,
                                                            installation.handler,
                                                            installation.isSysHandler ) );
        }
     };
	
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
	
	template < class RefConType, typename AEEventHandler_RefCon_Traits< RefConType >::ProcPtr handler >
	struct Adapt_AEEventHandler
	{
		static pascal OSErr ToCallback( const AppleEvent* appleEvent, AppleEvent* reply, long refCon )
		{
			try
			{
				handler( *appleEvent, *reply, reinterpret_cast< RefConType >( refCon ) );
			}
			catch ( OSStatus err )
			{
				return err.Get();
			}
			return noErr;
		}
	};
	
	template < AEEventHandler_RefCon_Traits< void >::ProcPtr handler >
	struct Adapt_AEEventHandler< void, handler >
	{
		static pascal OSErr ToCallback( const AppleEvent* appleEvent, AppleEvent* reply, long )
		{
			try
			{
				handler( *appleEvent, *reply );
			}
			catch ( OSStatus err )
			{
				return err.Get();
			}
			return noErr;
		}
	};
	
	// Level 0
	
   Owned<AEEventHandler>
   AEInstallEventHandler( const AEEventHandler& );
   
   inline Owned<AEEventHandler>
   AEInstallEventHandler( AEEventClass       theAEEventClass,
                          AEEventID          theAEEventID,
                          AEEventHandlerUPP  handler,
                          RefCon             handlerRefCon = RefCon(),
                          Boolean            isSysHandler = false )
     {
      return AEInstallEventHandler( AEEventHandler( theAEEventClass,
                                                    theAEEventID,
                                                    handler,
                                                    handlerRefCon,
                                                    isSysHandler ) );
     }
	
	// Level 1
	
   template < typename AEEventHandlerUPP::ProcPtr handler >
   inline  Owned<AEEventHandler>
   AEInstallEventHandler( AEEventClass       theAEEventClass,
                          AEEventID          theAEEventID,
                          RefCon             handlerRefCon = RefCon(),
                          Boolean            isSysHandler = false )
     {
      return AEInstallEventHandler( AEEventHandler( theAEEventClass,
                                                    theAEEventID,
                                                    StaticUPP< AEEventHandlerUPP, handler >(),
                                                    handlerRefCon,
                                                    isSysHandler ) );
     }
	
	// Level 2, refcon type specified
	
	template < class Object, typename AEEventHandler_RefCon_Traits< Object >::ProcPtr handler >
	inline Owned< AEEventHandler >
	AEInstallEventHandler( AEEventClass                                  theAEEventClass, 
	                       AEEventID                                     theAEEventID, 
	                       typename ObjectParameterTraits<Object>::Type  handlerRefCon   = typename ObjectParameterTraits<Object>::Type(), 
	                       Boolean                                       isSysHandler    = false )
	{
		return AEInstallEventHandler< Adapt_AEEventHandler< Object, handler >::ToCallback >
		(
			theAEEventClass, 
			theAEEventID, 
			ObjectParameterTraits<Object>::ConvertToPointer( handlerRefCon ), 
			isSysHandler
		);
	}
	
	// With default handlerRefCon but supplied isSysHandler
	template < class Object, typename AEEventHandler_RefCon_Traits< Object >::ProcPtr handler >
	inline Owned< AEEventHandler >
	AEInstallEventHandler( AEEventClass  theAEEventClass, 
	                       AEEventID     theAEEventID, 
	                       Boolean       isSysHandler )
	{
		typedef typename ObjectParameterTraits<Object>::Type ObjectType;
		
		return AEInstallEventHandler< Adapt_AEEventHandler< void, handler >::ToCallback >
		(
			theAEEventClass, 
			theAEEventID, 
			ObjectParameterTraits<Object>::ConvertToPointer( ObjectType() ), 
			isSysHandler
		);
	}
	
	// Same as above, but void parameter is omitted.
	template < typename AEEventHandler_RefCon_Traits< void >::ProcPtr handler >
	inline Owned< AEEventHandler >
	AEInstallEventHandler( AEEventClass  theAEEventClass, 
	                       AEEventID     theAEEventID, 
	                       Boolean       isSysHandler    = false )
	{
		return AEInstallEventHandler< void, handler >
		(
			theAEEventClass, 
			theAEEventID, 
			isSysHandler
		);
	}
	
   void AERemoveEventHandler( Owned<AEEventHandler> );

   typedef AEEventHandler AEGetEventHandler_Result;
   AEEventHandler AEGetEventHandler( AEEventClass theAEEventClass,
                                     AEEventID    theAEEventID,
                                     bool         isSysHandler = false );
  }

#endif
