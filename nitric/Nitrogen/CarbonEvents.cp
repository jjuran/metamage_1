// CarbonEvents.cp

#ifndef NITROGEN_CARBONEVENTS_H
#include "Nitrogen/CarbonEvents.h"
#endif
#ifndef NUCLEUS_ONLYONCE_H
#include "Nucleus/OnlyOnce.h"
#endif

namespace Nitrogen
  {
   void RunCurrentEventLoop( EventTimeout inTimeout )
     {
      Nucleus::OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( ::RunCurrentEventLoop( inTimeout ) );
     }

   void QuitEventLoop( EventLoopRef inEventLoop )
     {
      Nucleus::OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( ::QuitEventLoop( inEventLoop ) );
     }
   
   template <>
   EventRef ReceiveNextEvent< false >( UInt32                inNumTypes,
                                       const EventTypeSpec * inList,
                                       EventTimeout          inTimeout )
     {
      Nucleus::OnlyOnce< RegisterCarbonEventErrors >();
      EventRef result;
      ThrowOSStatus( ::ReceiveNextEvent( inNumTypes, inList, inTimeout, false, &result ) );
      return result;
     }

   template <>
   Nucleus::Owned<EventRef>ReceiveNextEvent< true >( UInt32                inNumTypes,
                                            const EventTypeSpec * inList,
                                            EventTimeout          inTimeout )
     {
      Nucleus::OnlyOnce< RegisterCarbonEventErrors >();
      EventRef result;
      ThrowOSStatus( ::ReceiveNextEvent( inNumTypes, inList, inTimeout, true, &result ) );
      return Nucleus::Owned<EventRef>::Seize( result );
     }

   Nucleus::Owned<EventRef> CreateEvent( CFAllocatorRef    inAllocator,
                                EventClass        inClassID,
                                CarbonEventKind   kind,
                                EventTime         when,
                                EventAttributes   flags )
     {
      Nucleus::OnlyOnce< RegisterCarbonEventErrors >();

      EventRef result;
      
      ThrowOSStatus( CreateEvent( inAllocator,
                                  inClassID,
                                  kind,
                                  when,
                                  flags,
                                  &result ) );
      
      return Nucleus::Owned<EventRef>::Seize( result );
     }

   Nucleus::Owned<EventRef> CopyEvent( EventRef inOther )
     {
      EventRef result = ::CopyEvent( inOther );
      
      if ( result == 0 )
         throw CopyEvent_Failed();
      
      return Nucleus::Owned<EventRef>::Seize( result );
     }

   void SetEventParameter( EventRef         inEvent,
                           EventParamName   inName,
                           EventParamType   inType,
                           UInt32           inSize,
                           const void *     inDataPtr )
     {
      Nucleus::OnlyOnce< RegisterCarbonEventErrors >();

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

      Nucleus::OnlyOnce< RegisterCarbonEventErrors >();

      ThrowOSStatus( ::GetEventParameter( inEvent,
                                          inName,
                                          inDesiredType,
                                          &outActualType,
                                          inBufferSize,
                                          &outActualSize,
                                          outData ) );

      GetEventParameter_Result result;
         result.outActualType = EventParamType( outActualType );
         result.outActualSize = outActualSize;
      
      return result;
     }

   void SetEventTime( EventRef inEvent, EventTime inTime )
     {
      Nucleus::OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( ::SetEventTime( inEvent, inTime ) );
     }

   
   void InvokeEventHandlerUPP( EventHandlerCallRef inHandlerCallRef,
                               EventRef            inEvent,
                               RefCon              inUserData,
                               EventHandlerUPP     userUPP )
     {
      Nucleus::OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( userUPP( inHandlerCallRef, inEvent, inUserData ) );
     }

   Nucleus::Owned<EventHandlerRef> InstallEventHandler( EventTargetRef         inTarget,
                                               EventHandlerUPP        inHandler,
                                               UInt32                 inNumTypes,
                                               const EventTypeSpec *  inList,
                                               const void *           inUserData )
     {
      EventHandlerRef result;
      Nucleus::OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( ::InstallEventHandler( inTarget,
                                            inHandler,
                                            inNumTypes,
                                            inList,
                                            const_cast<void*>( inUserData ),
                                            &result ) );
      return Nucleus::Owned<EventHandlerRef>::Seize( result );
     }

   Nucleus::Owned<EventHandlerRef> InstallEventHandler( EventTargetRef         inTarget,
                                               EventHandlerUPP        inHandler,
                                               EventClass             eventClass,
                                               CarbonEventKind        eventKind,
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

   ::OSStatus EventHandler_ExceptionGlue::CatchExceptions( EventHandlerCallRef inHandlerCallRef,
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
         return Nucleus::Convert< OSStatus >( Nucleus::TheExceptionBeingHandled(), EventInternalErr() );
        }
     }

   void InstallStandardEventHandler( EventTargetRef inTarget )
     {
      Nucleus::OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( ::InstallStandardEventHandler( inTarget ) );
     }

   void RemoveEventHandler( Nucleus::Owned<EventHandlerRef> inHandlerRef )
     {
      Nucleus::OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( ::RemoveEventHandler( inHandlerRef.release() ) );
     }

   void SendEventToEventTarget( EventRef inEvent, EventTargetRef inTarget )
     {
      Nucleus::OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( ::SendEventToEventTarget( inEvent, inTarget ) );
     }

   void ProcessHICommand( const HICommand& inCommand )
     {
      Nucleus::OnlyOnce< RegisterCarbonEventErrors >();
      ThrowOSStatus( ::ProcessHICommand( &inCommand ) );
     }

   Nucleus::Owned< EventHotKeyRef > RegisterEventHotKey( UInt32            inHotKeyCode,
                                                UInt32            inHotKeyModifiers,
                                                EventHotKeyID     inHotKeyID,
                                                EventTargetRef    inTarget,
                                                ::OptionBits      inOptions )
     {
      Nucleus::OnlyOnce< RegisterCarbonEventErrors >();
      EventHotKeyRef result;
      ThrowOSStatus( ::RegisterEventHotKey( inHotKeyCode,
                                            inHotKeyModifiers,
                                            inHotKeyID,
                                            inTarget,
                                            inOptions,
                                            &result ) );
      return Nucleus::Owned< EventHotKeyRef >::Seize( result );
     }

   void UnregisterEventHotKey( Nucleus::Owned< EventHotKeyRef > hotKey )
     {
      Nucleus::OnlyOnce< RegisterCarbonEventErrors >();
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
