// CarbonEvents.cp

#ifndef NITROGEN_CARBONEVENTS_H
#include "Nitrogen/CarbonEvents.h"
#endif
#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif

namespace Nitrogen
  {
   void RunCurrentEventLoop( EventTimeout inTimeout )
     {
      OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( ::RunCurrentEventLoop( inTimeout ) );
     }

   void QuitEventLoop( EventLoopRef inEventLoop )
     {
      OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( ::QuitEventLoop( inEventLoop ) );
     }

   Owned<EventRef> CreateEvent( CFAllocatorRef    inAllocator,
                                EventClass        inClassID,
                                EventKind         kind,
                                EventTime         when,
                                EventAttributes   flags )
     {
      OnlyOnce< RegisterCarbonEventErrors >();

      EventRef result;
      
      ThrowOSStatus( CreateEvent( inAllocator,
                                  inClassID,
                                  kind,
                                  when,
                                  flags,
                                  &result ) );
      
      return Owned<EventRef>::Seize( result );
     }

   Owned<EventRef> CopyEvent( EventRef inOther )
     {
      EventRef result = ::CopyEvent( inOther );
      
      if ( result == 0 )
         throw CopyEvent_Failed();
      
      return Owned<EventRef>::Seize( result );
     }

   void SetEventParameter( EventRef         inEvent,
                           EventParamName   inName,
                           EventParamType   inType,
                           UInt32           inSize,
                           const void *     inDataPtr )
     {
      OnlyOnce< RegisterCarbonEventErrors >();

      ThrowOSStatus( ::SetEventParameter( inEvent,
                                          inName,
                                          inType,
                                          inSize,
                                          inDataPtr ) );
     }

   GetEventParameter_Result GetEventParameter( EventRef          inEvent,
                                               EventParamName    inName,
                                               EventParamType    inDesiredType,
                                               UInt32            inBufferSize,
                                               void *            outData )
     {
      ::EventParamType outActualType;
      UInt32 outActualSize;

      OnlyOnce< RegisterCarbonEventErrors >();

      ThrowOSStatus( ::GetEventParameter( inEvent,
                                          inName,
                                          inDesiredType,
                                          &outActualType,
                                          inBufferSize,
                                          &outActualSize,
                                          outData ) );

      GetEventParameter_Result result;
         result.outActualType = outActualType;
         result.outActualSize = outActualSize;
      
      return result;
     }

   void SetEventTime( EventRef inEvent, EventTime inTime )
     {
      OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( ::SetEventTime( inEvent, inTime ) );
     }

   
   void InvokeEventHandlerUPP( EventHandlerCallRef inHandlerCallRef,
                               EventRef            inEvent,
                               RefCon              inUserData,
                               EventHandlerUPP     userUPP )
     {
      OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( userUPP( inHandlerCallRef, inEvent, inUserData ) );
     }

   Owned<EventHandlerRef> InstallEventHandler( EventTargetRef         inTarget,
                                               EventHandlerUPP        inHandler,
                                               UInt32                 inNumTypes,
                                               const EventTypeSpec *  inList,
                                               const void *           inUserData )
     {
      EventHandlerRef result;
      OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( ::InstallEventHandler( inTarget,
                                            inHandler,
                                            inNumTypes,
                                            inList,
                                            const_cast<void*>( inUserData ),
                                            &result ) );
      return Owned<EventHandlerRef>::Seize( result );
     }

   Owned<EventHandlerRef> InstallEventHandler( EventTargetRef         inTarget,
                                               EventHandlerUPP        inHandler,
                                               EventClass             eventClass,
                                               EventKind              eventKind,
                                               const void *           inUserData )
     {
      EventTypeSpec event;
      event.eventClass = eventClass;
      event.eventKind  = eventKind;

      return InstallEventHandler( inTarget,
                                  inHandler,
                                  1,
                                  &event,
                                  inUserData );
     }

   OSStatus EventHandler_ExceptionGlue::CatchExceptions( EventHandlerCallRef inHandlerCallRef,
                                                         EventRef inEvent,
                                                         void *inUserData,
                                                         Handler handler )
     {
      try
        {
         handler( inHandlerCallRef, inEvent, inUserData );
         return noErr;
        }
      catch ( ... )
        {
         return Convert< OSStatus >( TheExceptionBeingHandled(), eventInternalErr );
        }
     }

   void InstallStandardEventHandler( EventTargetRef inTarget )
     {
      OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( ::InstallStandardEventHandler( inTarget ) );
     }

   void RemoveEventHandler( Owned<EventHandlerRef> inHandlerRef )
     {
      OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( ::RemoveEventHandler( inHandlerRef.release() ) );
     }

   void SendEventToEventTarget( EventRef inEvent, EventTargetRef inTarget )
     {
      OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( ::SendEventToEventTarget( inEvent, inTarget ) );
     }

   Owned< EventHotKeyRef > RegisterEventHotKey( UInt32            inHotKeyCode,
                                                UInt32            inHotKeyModifiers,
                                                EventHotKeyID     inHotKeyID,
                                                EventTargetRef    inTarget,
                                                ::OptionBits      inOptions )
     {
      OnlyOnce< RegisterCarbonEventErrors >();
      EventHotKeyRef result;
      ThrowOSStatus( ::RegisterEventHotKey( inHotKeyCode,
                                            inHotKeyModifiers,
                                            inHotKeyID,
                                            inTarget,
                                            inOptions,
                                            &result ) );
      return Owned< EventHotKeyRef >::Seize( result );
     }

   void UnregisterEventHotKey( Owned< EventHotKeyRef > hotKey )
     {
      OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( ::UnregisterEventHotKey( hotKey.Release() ) );
     }

   void RegisterCarbonEventErrors()
     {
      RegisterOSStatus< eventAlreadyPostedErr           >();
      RegisterOSStatus< eventTargetBusyErr              >();
      RegisterOSStatus< eventClassInvalidErr            >();
      RegisterOSStatus< eventClassIncorrectErr          >();
      RegisterOSStatus< eventHandlerAlreadyInstalledErr >();
      RegisterOSStatus< eventInternalErr                >();
      RegisterOSStatus< eventKindIncorrectErr           >();
      RegisterOSStatus< eventParameterNotFoundErr       >();
      RegisterOSStatus< eventNotHandledErr              >();
      RegisterOSStatus< eventLoopTimedOutErr            >();
      RegisterOSStatus< eventLoopQuitErr                >();
      RegisterOSStatus< eventNotInQueueErr              >();
      RegisterOSStatus< eventHotKeyExistsErr            >();
      RegisterOSStatus< eventHotKeyInvalidErr           >();
     }
  }
