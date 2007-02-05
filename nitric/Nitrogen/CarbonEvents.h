// CarbonEvents.h

#ifndef NITROGEN_CARBONEVENTS_H
#define NITROGEN_CARBONEVENTS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CARBONEVENTS__
#include FRAMEWORK_HEADER(HIToolbox,CarbonEvents.h)
#endif

#ifndef NUCLEUS_SELECTOR_H
#include "Nucleus/Selector.h"
#endif
#ifndef NUCLEUS_OBJECTPARAMETERTRAITS_H
#include "Nucleus/ObjectParameterTraits.h"
#endif
#ifndef NUCLEUS_ARRAYSINGLETON_H
#include "Nucleus/ArraySingleton.h"
#endif
#ifndef NUCLEUS_PSEUDOREFERENCE_H
#include "Nucleus/Pseudoreference.h"
#endif

#ifndef NITROGEN_UPP_H
#include "Nitrogen/UPP.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif
#ifndef NITROGEN_REFCON_H
#include "Nitrogen/RefCon.h"
#endif
#ifndef NITROGEN_APPEARANCE_H
#include "Nitrogen/Appearance.h"
#endif
#ifndef NITROGEN_SCRAP_H
#include "Nitrogen/Scrap.h"
#endif
#ifndef NITROGEN_AEREGISTRY_H
#include "Nitrogen/AERegistry.h"
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif
#ifndef NITROGEN_MENUS_H
#include "Nitrogen/Menus.h"
#endif

#include <vector>

/* CarbonEvents.h has some nasty macros which get in our way; replace them
   with the equivalent inline functions. */

#ifdef InstallApplicationEventHandler
   #undef InstallApplicationEventHandler
   inline OSStatus InstallApplicationEventHandler( EventHandlerUPP        inHandler,
                                                   UInt32                 inNumTypes,
                                                   const EventTypeSpec *  inList,
                                                   void *                 inUserData,
                                                   EventHandlerRef *      outRef )
     {
      return InstallEventHandler( GetApplicationEventTarget(),
                                  inHandler,
                                  inNumTypes,
                                  inList,
                                  inUserData,
                                  outRef );
     }
#endif

#ifdef InstallWindowEventHandler
   #undef InstallWindowEventHandler
   inline OSStatus InstallWindowEventHandler( WindowRef              inTarget,
                                              EventHandlerUPP        inHandler,
                                              UInt32                 inNumTypes,
                                              const EventTypeSpec *  inList,
                                              void *                 inUserData,
                                              EventHandlerRef *      outRef )
     {
      return InstallEventHandler( GetWindowEventTarget( inTarget ),
                                  inHandler,
                                  inNumTypes,
                                  inList,
                                  inUserData,
                                  outRef );
     }
#endif

#ifdef InstallControlEventHandler
   #undef InstallControlEventHandler
   inline OSStatus InstallControlEventHandler( ControlRef             inTarget,
                                               EventHandlerUPP        inHandler,
                                               UInt32                 inNumTypes,
                                               const EventTypeSpec *  inList,
                                               void *                 inUserData,
                                               EventHandlerRef *      outRef )
     {
      return InstallEventHandler( GetControlEventTarget( inTarget ),
                                  inHandler,
                                  inNumTypes,
                                  inList,
                                  inUserData,
                                  outRef );
     }
#endif

#ifdef InstallMenuEventHandler
   #undef InstallMenuEventHandler
   inline OSStatus InstallMenuEventHandler( MenuRef                inTarget,
                                            EventHandlerUPP        inHandler,
                                            UInt32                 inNumTypes,
                                            const EventTypeSpec *  inList,
                                            void *                 inUserData,
                                            EventHandlerRef *      outRef )
     {
      return InstallEventHandler( GetMenuEventTarget( inTarget ),
                                  inHandler,
                                  inNumTypes,
                                  inList,
                                  inUserData,
                                  outRef );
     }
#endif

#ifdef HIViewInstallEventHandler
   #undef HIViewInstallEventHandler
   inline OSStatus HIViewInstallEventHandler( HIViewRef              inTarget,
                                            EventHandlerUPP        inHandler,
                                            UInt32                 inNumTypes,
                                            const EventTypeSpec *  inList,
                                            void *                 inUserData,
                                            EventHandlerRef *      outRef )
     {
      return InstallEventHandler( HIObjectGetEventTarget((HIObjectRef) inTarget ),
                                  inHandler,
                                  inNumTypes,
                                  inList,
                                  inUserData,
                                  outRef );
     }
#endif

#ifdef SendEventToApplication
   #undef SendEventToApplication
   inline OSStatus SendEventToApplication( EventRef inEvent )
     {
      return SendEventToEventTarget( inEvent, GetApplicationEventTarget() );
     }
#endif

#ifdef SendEventToWindow
   #undef SendEventToWindow
   inline OSStatus SendEventToWindow( EventRef inEvent, WindowRef inTarget )
     {
      return SendEventToEventTarget( inEvent, GetWindowEventTarget( inTarget ) );
     }
#endif

#ifdef SendEventToControl
   #undef SendEventToControl
   inline OSStatus SendEventToControl( EventRef inEvent, ControlRef inTarget )
     {
      return SendEventToEventTarget( inEvent, GetControlEventTarget( inTarget ) );
     }
#endif

#ifdef SendEventToMenu
   #undef SendEventToMenu
   inline OSStatus SendEventToMenu( EventRef inEvent, MenuRef inTarget )
     {
      return SendEventToEventTarget( inEvent, GetMenuEventTarget( inTarget ) );
     }
#endif

#ifdef SendEventToUserFocus
   #undef SendEventToUserFocus
   inline OSStatus SendEventToUserFocus( EventRef inEvent )
     {
      return SendEventToEventTarget( inEvent, GetUserFocusEventTarget() );
     }
#endif

namespace Nitrogen
  {
	class CarbonEventManagerErrorsRegistrationDependency
	{
		public:
			CarbonEventManagerErrorsRegistrationDependency();
	};
	
   typedef Nucleus::ErrorCode< OSStatus, eventAlreadyPostedErr           > EventAlreadyPostedErr;
   typedef Nucleus::ErrorCode< OSStatus, eventTargetBusyErr              > EventTargetBusyErr;
   typedef Nucleus::ErrorCode< OSStatus, eventClassInvalidErr            > EventClassInvalidErr;
   typedef Nucleus::ErrorCode< OSStatus, eventClassIncorrectErr          > EventClassIncorrectErr;
   typedef Nucleus::ErrorCode< OSStatus, eventHandlerAlreadyInstalledErr > EventHandlerAlreadyInstalledErr;
   typedef Nucleus::ErrorCode< OSStatus, eventInternalErr                > EventInternalErr;
   typedef Nucleus::ErrorCode< OSStatus, eventKindIncorrectErr           > EventKindIncorrectErr;
   typedef Nucleus::ErrorCode< OSStatus, eventParameterNotFoundErr       > EventParameterNotFoundErr;
   typedef Nucleus::ErrorCode< OSStatus, eventNotHandledErr              > EventNotHandledErr;
   typedef Nucleus::ErrorCode< OSStatus, eventLoopTimedOutErr            > EventLoopTimedOutErr;
   typedef Nucleus::ErrorCode< OSStatus, eventLoopQuitErr                > EventLoopQuitErr;
   typedef Nucleus::ErrorCode< OSStatus, eventNotInQueueErr              > EventNotInQueueErr;
   typedef Nucleus::ErrorCode< OSStatus, eventHotKeyExistsErr            > EventHotKeyExistsErr;
   typedef Nucleus::ErrorCode< OSStatus, eventHotKeyInvalidErr           > EventHotKeyInvalidErr;

   typedef Nucleus::Selector< class EventPriority_Tag, ::EventPriority >::Type EventPriority;
      // EventProrities should be ordered; Selector::Type isn't quite the right choice.

   /* To deal with:
   typedef double                          EventTime;
   typedef EventTime                       EventTimeout;
   typedef EventTime                       EventTimerInterval;
   #define kEventDurationSecond            ((EventTime)1.0)
   #define kEventDurationMillisecond       ((EventTime)(kEventDurationSecond/1000))
   #define kEventDurationMicrosecond       ((EventTime)(kEventDurationSecond/1000000))
   #define kEventDurationNanosecond        ((EventTime)(kEventDurationSecond/1000000000))
   #define kEventDurationMinute            ((EventTime)(kEventDurationSecond*60))
   #define kEventDurationHour              ((EventTime)(kEventDurationMinute*60))
   #define kEventDurationDay               ((EventTime)(kEventDurationHour*24))
   #define kEventDurationNoWait            ((EventTime)0.0)
   #define kEventDurationForever           ((EventTime)(-1.0))
   */

   typedef Nucleus::Selector< class EventClass_Tag, ::UInt32 >::Type EventClass;    // Doesn't exist in Carbon

   typedef Nucleus::Selector< class CarbonEventKind_Tag, ::UInt32 >::Type CarbonEventKind;
   
#ifndef JOSHUA_JURAN_EXPERIMENTAL
   
   // This conflicts with Events.h
   typedef CarbonEventKind     EventKind;
   
#endif
   
   using ::EventTypeSpec;

   typedef Nucleus::Selector< class MouseTrackingResult_Tag, ::MouseTrackingResult >::Type MouseTrackingResult;
	
	enum EventParamName
	{
		kEventParamDirectObject = ::kEventParamDirectObject,
		
		kEventParamPostTarget = ::kEventParamPostTarget,
		
		kEventParamWindowRef       = ::kEventParamWindowRef,
		kEventParamGrafPort        = ::kEventParamGrafPort,
		kEventParamDragRef         = ::kEventParamDragRef,
		kEventParamMenuRef         = ::kEventParamMenuRef,
		kEventParamEventRef        = ::kEventParamEventRef,
		kEventParamControlRef      = ::kEventParamControlRef,
		kEventParamRgnHandle       = ::kEventParamRgnHandle,
		kEventParamEnabled         = ::kEventParamEnabled,
		kEventParamDimensions      = ::kEventParamDimensions,
		kEventParamAvailableBounds = ::kEventParamAvailableBounds,
		kEventParamAEEventID       = ::kEventParamAEEventID,
		kEventParamAEEventClass    = ::kEventParamAEEventClass,
		kEventParamCGContextRef    = ::kEventParamCGContextRef,
		kEventParamDeviceDepth     = ::kEventParamDeviceDepth,
		kEventParamDeviceColor     = ::kEventParamDeviceColor,
		
		kEventParamMouseLocation   = ::kEventParamMouseLocation,
		kEventParamMouseButton     = ::kEventParamMouseButton,
		kEventParamClickCount      = ::kEventParamClickCount,
		kEventParamMouseWheelAxis  = ::kEventParamMouseWheelAxis,
		kEventParamMouseWheelDelta = ::kEventParamMouseWheelDelta,
		kEventParamMouseDelta      = ::kEventParamMouseDelta,
		kEventParamMouseChord      = ::kEventParamMouseChord,
		kEventParamTabletEventType = ::kEventParamTabletEventType,
		
	#if UNIVERSAL_INTERFACES_VERSION >= 0x0400
		
		kEventParamControlWouldAcceptDrop = ::kEventParamControlWouldAcceptDrop,
		
	#endif
		
		kEventParamName_Max = Nucleus::Enumeration_Traits< ::EventParamName >::max
	};

	//typedef Nucleus::Selector< class EventParamType_Tag, ::EventParamType >::Type EventParamType;
	
	typedef DescType EventParamType;
   
   using ::EventLoopRef;
   
   using ::GetCurrentEventLoop;
   using ::GetMainEventLoop;
   void RunCurrentEventLoop( EventTimeout inTimeout );
   void QuitEventLoop( EventLoopRef inEventLoop );
   
   /* This one is awkward: the Carbon headers don't define CFRunLoopRef, but this
      function is documented to return one. */
   using ::GetCFRunLoopFromEventLoop;
   using ::EventRef;
  }

namespace Nucleus
  {
   template <>
   struct Disposer< Nitrogen::EventRef >: public std::unary_function< Nitrogen::EventRef, void >
     {
      void operator()( Nitrogen::EventRef toDispose ) const
        {
         ::ReleaseEvent( toDispose );
        }
     };
  }

namespace Nitrogen
  {
   
   template < bool inPullEvent > struct ReceiveNextEvent_Traits;
   
   template <> struct ReceiveNextEvent_Traits< false >   { typedef EventRef        Result; };
   template <> struct ReceiveNextEvent_Traits< true  >   { typedef Nucleus::Owned<EventRef> Result; };
   
   template < bool inPullEvent >
   typename ReceiveNextEvent_Traits< inPullEvent >::Result
   ReceiveNextEvent( UInt32                inNumTypes  = 0,
                     const EventTypeSpec * inList      = 0,
                     EventTimeout          inTimeout   = kEventDurationForever );
   
   template <>
   EventRef ReceiveNextEvent< false >( UInt32                inNumTypes,
                                       const EventTypeSpec * inList,
                                       EventTimeout          inTimeout );

   template <>
   Nucleus::Owned<EventRef>ReceiveNextEvent< true >( UInt32                inNumTypes,
                                            const EventTypeSpec * inList,
                                            EventTimeout          inTimeout );
     
   template < bool inPullEvent, UInt32 inNumTypes >
   typename ReceiveNextEvent_Traits< inPullEvent >::Result
   ReceiveNextEvent( const EventTypeSpec (&inList)[ inNumTypes ],
                     EventTimeout        inTimeout = kEventDurationForever )
     {
      return ReceiveNextEvent<inPullEvent>( inNumTypes, inList, inTimeout );
     }

   template < bool inPullEvent >
   typename ReceiveNextEvent_Traits< inPullEvent >::Result
   ReceiveNextEvent( const std::vector<EventTypeSpec>& inList,
                     EventTimeout                      inTimeout = kEventDurationForever )
     {
      return ReceiveNextEvent<inPullEvent>( inList.size(), inList.empty() ? 0 : &inList.front(), inTimeout );
     }

   Nucleus::Owned<EventRef> CreateEvent( CFAllocatorRef    inAllocator,
                                EventClass        inClassID,
                                CarbonEventKind   kind,
                                EventTime         when,
                                EventAttributes   flags );

   inline Nucleus::Owned<EventRef> CreateEvent( EventClass        inClassID,
                                       CarbonEventKind   kind,
                                       EventTime         when,
                                       EventAttributes   flags )
     {
      return CreateEvent( 0, inClassID, kind, when, flags );
     }
   
   class CopyEvent_Failed {};
   Nucleus::Owned<EventRef> CopyEvent( EventRef inOther );
   
   inline Nucleus::Owned<EventRef> RetainEvent( EventRef inEvent )
     {
      return Nucleus::Owned<EventRef>::Seize( ::RetainEvent( inEvent ) );
     }
   
   using ::GetEventRetainCount;
   
   inline void ReleaseEvent( Nucleus::Owned<EventRef> )
     {}
   
   
   void SetEventParameter( EventRef         inEvent,
                           EventParamName   inName,
                           EventParamType   inType,
                           UInt32           inSize,
                           const void *     inDataPtr );

	class SetEventParameter_Putter
	{
		private:
			EventRef        myEvent;
			EventParamName  myName;
			EventParamType  myType;
		
		public:
			SetEventParameter_Putter( EventRef        event,
			                          EventParamName  name,
			                          EventParamType  type ) : myEvent( event ),
			                                                   myName ( name  ),
			                                                   myType ( type  )  {}
			
			void operator()( const void *begin, const void *end ) const
			{
				Nitrogen::SetEventParameter( myEvent,
				                             myName,
				                             myType,
				                             Detail::Distance( begin, end ),
				                             begin );
			}
	};
	
   template < EventParamType inType >
   struct SetEventParameter_Traits
     {
      typedef typename DescType_Traits<inType>::Parameter InData_Type;
     };
   
   template < EventParamType inType >
   void SetEventParameter( EventRef inEvent,
                           EventParamName inName,
                           typename SetEventParameter_Traits<inType>::InData_Type inData )
     {
      typedef DescType_Traits< inType > Traits;
      
      Traits().Put( inData,
		            SetEventParameter_Putter( inEvent,
		                                      inName,
		                                      inType ) );
      }

   struct GetEventParameter_Result
     {
      EventParamType outActualType;
      UInt32 outActualSize;
     };
   
   GetEventParameter_Result GetEventParameter( EventRef          inEvent,
                                               EventParamName    inName,
                                               EventParamType    inDesiredType,
                                               UInt32            inBufferSize,
                                               void *            outData );

   inline GetEventParameter_Result GetEventParameter( EventRef          inEvent,
                                                      EventParamName    inName,
                                                      EventParamType    inDesiredType )
     {
      return GetEventParameter( inEvent, inName, inDesiredType, 0, 0 );
     }
   
	template < EventParamType desiredType >
	class GetEventParameter_Getter
	{
		private:
			EventRef        myEvent;
			EventParamName  myName;
		
		public:
			GetEventParameter_Getter( EventRef        event,
			                          EventParamName  name ) : myEvent( event ),
			                                                   myName ( name  )  {}
			
			std::size_t size() const
			{
				if ( DescType_Traits< desiredType >::hasStaticSize )
				{
					return sizeof (typename DescType_Traits< desiredType >::Buffer);
				}
				
				return GetEventParameter( myEvent, myName, desiredType ).outActualSize;
			}
			
			void operator()( void *begin, void *end ) const
			{
				GetEventParameter( myEvent,
				                   myName,
				                   desiredType,
				                   Detail::Distance( begin, end ),
				                   begin );
			}
	};
	
   template < EventParamType inDesiredType >
   struct GetEventParameter_Traits
     {
      typedef typename DescType_Traits<inDesiredType>::Result Result;
     };
   
   template < EventParamType inDesiredType >
   typename GetEventParameter_Traits<inDesiredType>::Result GetEventParameter( EventRef inEvent, EventParamName inName )
     {
      typedef DescType_Traits< inDesiredType > Traits;
      
      return Traits().Get( GetEventParameter_Getter< inDesiredType >( inEvent, inName ) );
     }

   template < EventParamType inDesiredType >
   class EventParameter_Details
     {
      private:
         EventRef event;
         EventParamName name;
      
      public:
         typedef typename GetEventParameter_Traits<inDesiredType>::Result      Value;
         typedef typename GetEventParameter_Traits<inDesiredType>::Result      GetResult;
         typedef typename SetEventParameter_Traits<inDesiredType>::InData_Type SetParameter;
         
         EventParameter_Details( EventRef theEvent, EventParamName theName )
           : event( theEvent ),
             name( theName )
           {}
         
         GetResult Get() const                   { return GetEventParameter<inDesiredType>( event, name ); }
         void Set( SetParameter value ) const    { SetEventParameter<inDesiredType>( event, name, value ); }
     };

   // These will become template-typedefs when those become available.
      template < EventParamType inDesiredType >
      class ConstEventParameter: public Nucleus::ConstPseudoreference< EventParameter_Details< inDesiredType > >
        {
         private:
            typedef Nucleus::ConstPseudoreference< EventParameter_Details< inDesiredType > > Base;

         public:
            typedef typename Base::Details               Details;
            
            typedef ConstEventParameter< inDesiredType > Reference;
            typedef ConstEventParameter< inDesiredType > ConstReference;

            typedef Nucleus::Pseudopointer< Reference >           Pointer;
            typedef Nucleus::Pseudopointer< ConstReference >      ConstPointer;

            explicit ConstEventParameter( Details theDetails )                             : Base( theDetails ) {}
            ConstEventParameter( EventRef theEvent, EventParamName theName )               : Base( Details( theEvent, theName ) ) {}
            
            Pointer operator&() const                                                      { return Pointer( Base::operator&().Get() ); }
            
            using Base::Get;
        };

      template < EventParamType inDesiredType >
      class EventParameter: public Nucleus::Pseudoreference< EventParameter_Details< inDesiredType > >
        {
         private:
            typedef Nucleus::Pseudoreference< EventParameter_Details< inDesiredType > > Base;

         public:
            typedef typename Base::Details               Details;
            typedef typename Details::SetParameter       SetParameter;
            
            typedef EventParameter     < inDesiredType > Reference;
            typedef ConstEventParameter< inDesiredType > ConstReference;

            typedef Nucleus::Pseudopointer< Reference >           Pointer;
            typedef Nucleus::Pseudopointer< ConstReference >      ConstPointer;

            explicit EventParameter( Details theDetails )                                  : Base( theDetails ) {}
            EventParameter( EventRef theEvent, EventParamName theName )                    : Base( Details( theEvent, theName ) ) {}
                        
            Pointer operator&() const                                                      { return Pointer( Base::operator&().Get() ); }
    
            operator ConstReference() const                                                { return ConstReference( Base::operator&().Get() ); }
            
            using Base::Get;
            using Base::Set;
            
            const EventParameter& operator=( SetParameter value ) const                   { Set( value ); return *this; }
            const EventParameter& operator=( const EventParameter& rhs ) const            { Set( rhs.Get() ); return *this; }
            template < class T > const EventParameter& operator+=( const T& rhs ) const   { Set( Get() + rhs ); return *this; }
            template < class T > const EventParameter& operator-=( const T& rhs ) const   { Set( Get() - rhs ); return *this; }
            template < class T > const EventParameter& operator*=( const T& rhs ) const   { Set( Get() * rhs ); return *this; }
            template < class T > const EventParameter& operator/=( const T& rhs ) const   { Set( Get() / rhs ); return *this; }
            template < class T > const EventParameter& operator%=( const T& rhs ) const   { Set( Get() % rhs ); return *this; }
            template < class T > const EventParameter& operator&=( const T& rhs ) const   { Set( Get() & rhs ); return *this; }
            template < class T > const EventParameter& operator|=( const T& rhs ) const   { Set( Get() | rhs ); return *this; }
            template < class T > const EventParameter& operator^=( const T& rhs ) const   { Set( Get() ^ rhs ); return *this; }
            template < class T > const EventParameter& operator<<=( const T& rhs ) const  { Set( Get() << rhs ); return *this; }
            template < class T > const EventParameter& operator>>=( const T& rhs ) const  { Set( Get() >> rhs ); return *this; }
        };

   inline EventClass       GetEventClass( EventRef inEvent )     { return EventClass     ( ::GetEventClass( inEvent ) ); }
   inline CarbonEventKind  GetEventKind ( EventRef inEvent )     { return CarbonEventKind( ::GetEventKind ( inEvent ) ); }
   inline EventTime        GetEventTime ( EventRef inEvent )     { return EventTime      ( ::GetEventTime ( inEvent ) ); }
 
   void SetEventTime( EventRef inEvent, EventTime inTime );

   using ::GetCurrentEventQueue;
   using ::GetMainEventQueue;
   
   /* ... */
   
   template < UInt32 eventClass, UInt32 eventKind, ::EventParamName paramater >
   struct EventParameter_Traits;
   
   template < EventParamType theType, bool in, bool out >
   struct Basic_EventParameter_Traits
     {
      static const EventParamType type = theType;
      static const bool forInput = in;
      static const bool forOutput = out;
      typedef typename DescType_Traits< type >::Result Type;
     };
 
   static const ::EventParamName noEventHandlerResult = 0;
   
   template < UInt32 eventClass, UInt32 eventKind >
   struct EventParameter_Traits< eventClass, eventKind, noEventHandlerResult >
     {
      static const EventParamType type = typeNull;
      typedef typename DescType_Traits< type >::Result Type;
     };
   
   using ::HICommand;

  }

namespace Nucleus
  {
   template <>
   struct Maker< Nitrogen::HICommand >
     {
      Nitrogen::HICommand operator()( UInt32 attributes, UInt32 commandID, Nitrogen::MenuRef menuRef, Nitrogen::MenuItemIndex menuItemIndex ) const
        {
         Nitrogen::HICommand result;
         result.attributes = attributes;
         result.commandID = commandID;
         result.menu.menuRef = menuRef;
         result.menu.menuItemIndex = menuItemIndex;
         return result;
        }

      HICommand operator()( UInt32 commandID, MenuRef menuRef, MenuItemIndex menuItemIndex ) const
        {
         return operator()( kHICommandFromMenu, commandID, menuRef, menuItemIndex );
        }
      
      HICommand operator()( UInt32 attributes, UInt32 commandID ) const
        {
         HICommand result;
         result.attributes = attributes;
         result.commandID = commandID;
         return result;
        }
      
      HICommand operator()( UInt32 commandID ) const
        {
         return operator()( 0, commandID );
        }
     };
  }

namespace Nitrogen
  {
   using ::HICommandExtended;
  }

namespace Nucleus
  {
   template <>
   struct Maker< Nitrogen::HICommandExtended >
     {
      Nitrogen::HICommandExtended operator()( UInt32 attributes, UInt32 commandID, Nitrogen::MenuRef menuRef, Nitrogen::MenuItemIndex menuItemIndex ) const
        {
         Nitrogen::HICommandExtended result;
         result.attributes = attributes;
         result.commandID = commandID;
         result.source.menu.menuRef = menuRef;
         result.source.menu.menuItemIndex = menuItemIndex;
         return result;
        }

      HICommandExtended operator()( UInt32 commandID, MenuRef menuRef, MenuItemIndex menuItemIndex ) const
        {
         return operator()( kHICommandFromMenu, commandID, menuRef, menuItemIndex );
        }
 
      HICommandExtended operator()( UInt32 attributes, UInt32 commandID, ControlRef control ) const
        {
         HICommandExtended result;
         result.attributes = attributes;
         result.commandID = commandID;
         result.source.control = control;
         return result;
        }

      HICommandExtended operator()( UInt32 commandID, ControlRef control ) const
        {
         return operator()( kHICommandFromControl, commandID, control );
        }
 
      HICommandExtended operator()( UInt32 attributes, UInt32 commandID, WindowRef window ) const
        {
         HICommandExtended result;
         result.attributes = attributes;
         result.commandID = commandID;
         result.source.window = window;
         return result;
        }

      HICommandExtended operator()( UInt32 commandID, WindowRef window ) const
        {
         return operator()( kHICommandFromWindow, commandID, window );
        }
     };   
  }

namespace Nitrogen
  {
	
	static const EventClass kEventClassCommand = EventClass( ::kEventClassCommand );
	
	static const CarbonEventKind kEventProcessCommand      = CarbonEventKind( ::kEventProcessCommand      );
	static const CarbonEventKind kEventCommandUpdateStatus = CarbonEventKind( ::kEventCommandUpdateStatus );
	
	
/* ... */

   template<> struct DescType_Traits< typeWindowRef               >: Nucleus::PodFlattener< WindowRef               > {};
   template<> struct DescType_Traits< typeGrafPtr                 >: Nucleus::PodFlattener< CGrafPtr                > {};
   template<> struct DescType_Traits< typeGWorldPtr               >: Nucleus::PodFlattener< GWorldPtr               > {};
   template<> struct DescType_Traits< typeDragRef                 >: Nucleus::PodFlattener< DragRef                 > {};
   template<> struct DescType_Traits< typeMenuRef                 >: Nucleus::PodFlattener< MenuRef                 > {};
   template<> struct DescType_Traits< typeControlRef              >: Nucleus::PodFlattener< ControlRef              > {};
   template<> struct DescType_Traits< typeCollection              >: Nucleus::PodFlattener< Collection              > {};
   template<> struct DescType_Traits< typeQDRgnHandle             >: Nucleus::PodFlattener< RgnHandle               > {};
   template<> struct DescType_Traits< typeOSStatus                >: Nucleus::PodFlattener< OSStatus                > {};
   template<> struct DescType_Traits< typeCFStringRef             >: Nucleus::PodFlattener< CFStringRef             > {};
   template<> struct DescType_Traits< typeCGContextRef            >: Nucleus::PodFlattener< CGContextRef            > {};
   template<> struct DescType_Traits< typeHIPoint                 >: Nucleus::PodFlattener< HIPoint                 > {};

   template<> struct DescType_Traits< typeMouseButton             >: Nucleus::PodFlattener< EventMouseButton        > {};
   template<> struct DescType_Traits< typeMouseWheelAxis          >: Nucleus::PodFlattener< EventMouseWheelAxis     > {};

   template<> struct DescType_Traits< typeEventHotKeyID           >: Nucleus::PodFlattener< EventHotKeyID           > {};

   template<> struct DescType_Traits< typeHICommand               >: Nucleus::PodFlattener< HICommand               > {};

   template<> struct DescType_Traits< typeWindowRegionCode        >: Nucleus::PodFlattener< WindowRegionCode        > {};
   template<> struct DescType_Traits< typeWindowDefPartCode       >: Nucleus::PodFlattener< WindowDefPartCode       > {};
   template<> struct DescType_Traits< typeClickActivationResult   >: Nucleus::PodFlattener< ClickActivationResult   > {};

   template<> struct DescType_Traits< typeControlActionUPP        >: Nucleus::PodFlattener< ControlActionUPP        > {};
   template<> struct DescType_Traits< typeIndicatorDragConstraint >: Nucleus::PodFlattener< IndicatorDragConstraint > {};
   template<> struct DescType_Traits< typeControlPartCode         >: Nucleus::PodFlattener< ControlPartCode         > {};

   template<> struct DescType_Traits< typeMenuItemIndex           >: Nucleus::PodFlattener< MenuItemIndex           > {};
   template<> struct DescType_Traits< typeMenuCommand             >: Nucleus::PodFlattener< MenuCommand             > {};
   template<> struct DescType_Traits< typeMenuTrackingMode        >: Nucleus::PodFlattener< MenuTrackingMode        > {};
   template<> struct DescType_Traits< typeMenuEventOptions        >: Nucleus::PodFlattener< MenuEventOptions        > {};
   template<> struct DescType_Traits< typeThemeMenuState          >: Nucleus::PodFlattener< ThemeMenuState          > {};
   template<> struct DescType_Traits< typeThemeMenuItemType       >: Nucleus::PodFlattener< ThemeMenuItemType       > {};

   template<> struct DescType_Traits< typeTabletPointRec          >: Nucleus::PodFlattener< TabletPointRec          > {};
   template<> struct DescType_Traits< typeTabletProximityRec      >: Nucleus::PodFlattener< TabletProximityRec      > {};

   template<> struct DescType_Traits< typeScrapRef                >: Nucleus::PodFlattener< ScrapRef                > {};
   template<> struct DescType_Traits< typeCFMutableArrayRef       >: Nucleus::PodFlattener< CFMutableArrayRef       > {};

   template<> struct DescType_Traits< typeFSVolumeRefNum          >: Nucleus::PodFlattener< FSVolumeRefNum          > {};

/*	This one is different; N::CFTypeRef is a type defined in Nitrogen; so we use ConvertingPODFlattener */
   template<> struct DescType_Traits< typeCFTypeRef               >: Nucleus::ConvertingPODFlattener< CFTypeRef, ::CFTypeRef > {};

/* ... */

   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseDown,       kEventParamMouseLocation      >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseDown,       kEventParamKeyModifiers       >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseDown,       kEventParamMouseButton        >: Basic_EventParameter_Traits< typeMouseButton,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseDown,       kEventParamClickCount         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseDown,       kEventParamMouseChord         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseDown,       kEventParamTabletEventType    >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseDown,       kEventParamTabletPointRec     >: Basic_EventParameter_Traits< typeTabletPointRec,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseDown,       kEventParamTabletProximityRec >: Basic_EventParameter_Traits< typeTabletProximityRec,      true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseUp,         kEventParamMouseLocation      >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseUp,         kEventParamKeyModifiers       >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseUp,         kEventParamMouseButton        >: Basic_EventParameter_Traits< typeMouseButton,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseUp,         kEventParamClickCount         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseUp,         kEventParamMouseChord         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseUp,         kEventParamTabletEventType    >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseUp,         kEventParamTabletPointRec     >: Basic_EventParameter_Traits< typeTabletPointRec,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseUp,         kEventParamTabletProximityRec >: Basic_EventParameter_Traits< typeTabletProximityRec,      true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseMoved,      kEventParamMouseLocation      >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseMoved,      kEventParamMouseDelta         >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseMoved,      kEventParamKeyModifiers       >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseMoved,      kEventParamTabletEventType    >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseMoved,      kEventParamTabletPointRec     >: Basic_EventParameter_Traits< typeTabletPointRec,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseMoved,      kEventParamTabletProximityRec >: Basic_EventParameter_Traits< typeTabletProximityRec,      true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseDragged,    kEventParamMouseLocation      >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseDragged,    kEventParamMouseDelta         >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseDragged,    kEventParamKeyModifiers       >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseDragged,    kEventParamMouseButton        >: Basic_EventParameter_Traits< typeMouseButton,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseDragged,    kEventParamMouseChord         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseDragged,    kEventParamTabletEventType    >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseDragged,    kEventParamTabletPointRec     >: Basic_EventParameter_Traits< typeTabletPointRec,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseDragged,    kEventParamTabletProximityRec >: Basic_EventParameter_Traits< typeTabletProximityRec,      true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseWheelMoved, kEventParamMouseLocation      >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseWheelMoved, kEventParamKeyModifiers       >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseWheelMoved, kEventParamMouseWheelAxis     >: Basic_EventParameter_Traits< typeMouseWheelAxis,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMouse, kEventMouseWheelMoved, kEventParamMouseWheelDelta    >: Basic_EventParameter_Traits< typeSInt32,                  true,  false >{};

   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputUpdateActiveInputArea, kEventParamTextInputSendComponentInstance      >: Basic_EventParameter_Traits< typeComponentInstance,       true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputUpdateActiveInputArea, kEventParamTextInputSendRefCon                 >: Basic_EventParameter_Traits< typeSInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputUpdateActiveInputArea, kEventParamTextInputSendSLRec                  >: Basic_EventParameter_Traits< typeIntlWritingCode,         true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputUpdateActiveInputArea, kEventParamTextInputSendFixLen                 >: Basic_EventParameter_Traits< typeSInt32,                  true,  false >{};
 //template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputUpdateActiveInputArea, kEventParamTextInputSendText                   >: Basic_EventParameter_Traits< typeUnicodeText,             true,  false >{};
 //The data type depends on the TSMDocument.
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputUpdateActiveInputArea, kEventParamTextInputSendUpdateRng              >: Basic_EventParameter_Traits< typeTextRangeArray,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputUpdateActiveInputArea, kEventParamTextInputSendHiliteRng              >: Basic_EventParameter_Traits< typeTextRangeArray,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputUpdateActiveInputArea, kEventParamTextInputSendClauseRng              >: Basic_EventParameter_Traits< typeOffsetArray,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputUpdateActiveInputArea, kEventParamTextInputSendPinRng                 >: Basic_EventParameter_Traits< typeTextRange,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputUpdateActiveInputArea, kEventParamTextInputSendTextServiceEncoding    >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputUpdateActiveInputArea, kEventParamTextInputSendTextServiceMacEncoding >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputUnicodeForKeyEvent,    kEventParamTextInputSendComponentInstance      >: Basic_EventParameter_Traits< typeComponentInstance,       true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputUnicodeForKeyEvent,    kEventParamTextInputSendRefCon                 >: Basic_EventParameter_Traits< typeSInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputUnicodeForKeyEvent,    kEventParamTextInputSendSLRec                  >: Basic_EventParameter_Traits< typeIntlWritingCode,         true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputUnicodeForKeyEvent,    kEventParamTextInputSendText                   >: Basic_EventParameter_Traits< typeUnicodeText,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputUnicodeForKeyEvent,    kEventParamTextInputSendKeyboardEvent          >: Basic_EventParameter_Traits< typeEventRef,                true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputOffsetToPos,           kEventParamTextInputSendComponentInstance      >: Basic_EventParameter_Traits< typeComponentInstance,       true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputOffsetToPos,           kEventParamTextInputSendRefCon                 >: Basic_EventParameter_Traits< typeSInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputOffsetToPos,           kEventParamTextInputSendTextOffset             >: Basic_EventParameter_Traits< typeSInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputOffsetToPos,           kEventParamTextInputReplyPoint                 >: Basic_EventParameter_Traits< typeQDPoint,                 false, true  >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputOffsetToPos,           kEventParamTextInputSendSLRec                  >: Basic_EventParameter_Traits< typeIntlWritingCode,         true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputOffsetToPos,           kEventParamTextInputSendLeadingEdge            >: Basic_EventParameter_Traits< typeBoolean,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputOffsetToPos,           kEventParamTextInputReplySLRec                 >: Basic_EventParameter_Traits< typeIntlWritingCode,         false, true  >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputOffsetToPos,           kEventParamTextInputReplyFont                  >: Basic_EventParameter_Traits< typeSInt32,                  false, true  >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputOffsetToPos,           kEventParamTextInputReplyFMFont                >: Basic_EventParameter_Traits< typeUInt32,                  false, true  >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputOffsetToPos,           kEventParamTextInputReplyPointSize             >: Basic_EventParameter_Traits< typeFixed,                   false, true  >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputOffsetToPos,           kEventParamTextInputReplyLineHeight            >: Basic_EventParameter_Traits< typeSInt16,                  false, true  >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputOffsetToPos,           kEventParamTextInputReplyLineAscent            >: Basic_EventParameter_Traits< typeSInt16,                  false, true  >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputOffsetToPos,           kEventParamTextInputReplyTextAngle             >: Basic_EventParameter_Traits< typeFixed,                   false, true  >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputPosToOffset,           kEventParamTextInputSendComponentInstance      >: Basic_EventParameter_Traits< typeComponentInstance,       true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputPosToOffset,           kEventParamTextInputSendRefCon                 >: Basic_EventParameter_Traits< typeSInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputPosToOffset,           kEventParamTextInputSendCurrentPoint           >: Basic_EventParameter_Traits< typeQDPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputPosToOffset,           kEventParamTextInputReplyRegionClass           >: Basic_EventParameter_Traits< typeSInt32,                  false, true  >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputPosToOffset,           kEventParamTextInputReplyTextOffset            >: Basic_EventParameter_Traits< typeSInt32,                  false, true  >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputPosToOffset,           kEventParamTextInputSendDraggingMode           >: Basic_EventParameter_Traits< typeBoolean,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputPosToOffset,           kEventParamTextInputReplyLeadingEdge           >: Basic_EventParameter_Traits< typeBoolean,                 false, true  >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputPosToOffset,           kEventParamTextInputReplySLRec                 >: Basic_EventParameter_Traits< typeIntlWritingCode,         false, true  >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputShowHideBottomWindow,  kEventParamTextInputSendComponentInstance      >: Basic_EventParameter_Traits< typeComponentInstance,       true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputShowHideBottomWindow,  kEventParamTextInputSendRefCon                 >: Basic_EventParameter_Traits< typeSInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputShowHideBottomWindow,  kEventParamTextInputSendShowHide               >: Basic_EventParameter_Traits< typeBoolean,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputShowHideBottomWindow,  kEventParamTextInputReplyShowHide              >: Basic_EventParameter_Traits< typeBoolean,                 false, true  >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputGetSelectedText,       kEventParamTextInputSendComponentInstance      >: Basic_EventParameter_Traits< typeComponentInstance,       true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputGetSelectedText,       kEventParamTextInputSendRefCon                 >: Basic_EventParameter_Traits< typeSInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputGetSelectedText,       kEventParamTextInputSendLeadingEdge            >: Basic_EventParameter_Traits< typeBoolean,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputGetSelectedText,       kEventParamTextInputSendTextServiceEncoding    >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputGetSelectedText,       kEventParamTextInputSendTextServiceMacEncoding >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
 //template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputGetSelectedText,       kEventParamTextInputReplyText                  >: Basic_EventParameter_Traits< typeUnicodeText,             false, true  >{};
 //The data type depends on the TSMDocument.
   template <> struct EventParameter_Traits< kEventClassTextInput, kEventTextInputGetSelectedText,       kEventParamTextInputReplySLRec                 >: Basic_EventParameter_Traits< typeIntlWritingCode,         false, true  >{};

   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventRawKeyDown,             kEventParamKeyMacCharCodes >: Basic_EventParameter_Traits< typeChar,                    true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventRawKeyDown,             kEventParamKeyUnicodes     >: Basic_EventParameter_Traits< typeUnicodeText,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventRawKeyDown,             kEventParamKeyCode         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventRawKeyDown,             kEventParamKeyModifiers    >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventRawKeyDown,             kEventParamKeyboardType    >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventRawKeyRepeat,           kEventParamKeyMacCharCodes >: Basic_EventParameter_Traits< typeChar,                    true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventRawKeyRepeat,           kEventParamKeyUnicodes     >: Basic_EventParameter_Traits< typeUnicodeText,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventRawKeyRepeat,           kEventParamKeyCode         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventRawKeyRepeat,           kEventParamKeyModifiers    >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventRawKeyRepeat,           kEventParamKeyboardType    >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventRawKeyUp,               kEventParamKeyMacCharCodes >: Basic_EventParameter_Traits< typeChar,                    true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventRawKeyUp,               kEventParamKeyUnicodes     >: Basic_EventParameter_Traits< typeUnicodeText,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventRawKeyUp,               kEventParamKeyCode         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventRawKeyUp,               kEventParamKeyModifiers    >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventRawKeyUp,               kEventParamKeyboardType    >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventRawKeyModifiersChanged, kEventParamKeyModifiers    >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventHotKeyPressed,          kEventParamDirectObject    >: Basic_EventParameter_Traits< typeEventHotKeyID,           true,  false >{};
   template <> struct EventParameter_Traits< kEventClassKeyboard, kEventHotKeyReleased,         kEventParamDirectObject    >: Basic_EventParameter_Traits< typeEventHotKeyID,           true,  false >{};

   template <> struct EventParameter_Traits< kEventClassApplication, kEventAppActivated,          kEventParamWindowRef    >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassApplication, kEventAppLaunchNotification, kEventParamProcessID    >: Basic_EventParameter_Traits< typeProcessSerialNumber,     true,  false >{};
   template <> struct EventParameter_Traits< kEventClassApplication, kEventAppLaunchNotification, kEventParamLaunchRefCon >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassApplication, kEventAppLaunchNotification, kEventParamLaunchErr    >: Basic_EventParameter_Traits< typeOSStatus,                true,  false >{};
   template <> struct EventParameter_Traits< kEventClassApplication, kEventAppLaunched,           kEventParamProcessID    >: Basic_EventParameter_Traits< typeProcessSerialNumber,     true,  false >{};
   template <> struct EventParameter_Traits< kEventClassApplication, kEventAppTerminated,         kEventParamProcessID    >: Basic_EventParameter_Traits< typeProcessSerialNumber,     true,  false >{};
   template <> struct EventParameter_Traits< kEventClassApplication, kEventAppFrontSwitched,      kEventParamProcessID    >: Basic_EventParameter_Traits< typeProcessSerialNumber,     true,  false >{};
   template <> struct EventParameter_Traits< kEventClassApplication, kEventAppGetDockTileMenu,    kEventParamMenuRef      >: Basic_EventParameter_Traits< typeMenuRef,                 false, true  >{};

   template <> struct EventParameter_Traits< kEventClassAppleEvent, kEventAppleEvent,             kEventParamAEEventClass >: Basic_EventParameter_Traits< typeType,                    true,  false >{};
   template <> struct EventParameter_Traits< kEventClassAppleEvent, kEventAppleEvent,             kEventParamAEEventID    >: Basic_EventParameter_Traits< typeType,                    true,  false >{};

   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowUpdate,                kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowDrawContent,           kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowActivated,             kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowDeactivated,           kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetClickActivation,    kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetClickActivation,    kEventParamMouseLocation           >: Basic_EventParameter_Traits< typeQDPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetClickActivation,    kEventParamKeyModifiers            >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetClickActivation,    kEventParamWindowDefPart           >: Basic_EventParameter_Traits< typeWindowDefPartCode,       true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetClickActivation,    kEventParamControlRef              >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetClickActivation,    kEventParamClickActivation         >: Basic_EventParameter_Traits< typeClickActivationResult,   false, true  >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowShowing,               kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowHiding,                kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowShown,                 kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowHidden,                kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowBoundsChanging,        kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowBoundsChanging,        kEventParamAttributes              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowBoundsChanging,        kEventParamOriginalBounds          >: Basic_EventParameter_Traits< typeQDRectangle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowBoundsChanging,        kEventParamPreviousBounds          >: Basic_EventParameter_Traits< typeQDRectangle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowBoundsChanging,        kEventParamCurrentBounds           >: Basic_EventParameter_Traits< typeQDRectangle,             true,  true  >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowBoundsChanged,         kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowBoundsChanged,         kEventParamAttributes              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowBoundsChanged,         kEventParamOriginalBounds          >: Basic_EventParameter_Traits< typeQDRectangle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowBoundsChanged,         kEventParamPreviousBounds          >: Basic_EventParameter_Traits< typeQDRectangle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowBoundsChanged,         kEventParamCurrentBounds           >: Basic_EventParameter_Traits< typeQDRectangle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowResizeStarted,         kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowResizeCompleted,       kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowDragStarted,           kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowDragCompleted,         kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickDragRgn,          kEventParamMouseLocation           >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickDragRgn,          kEventParamKeyModifiers            >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickDragRgn,          kEventParamMouseButton             >: Basic_EventParameter_Traits< typeMouseButton,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickDragRgn,          kEventParamClickCount              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickDragRgn,          kEventParamMouseChord              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickDragRgn,          kEventParamTabletEventType         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickDragRgn,          kEventParamTabletPointRec          >: Basic_EventParameter_Traits< typeTabletPointRec,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickDragRgn,          kEventParamTabletProximityRec      >: Basic_EventParameter_Traits< typeTabletProximityRec,      true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickResizeRgn,        kEventParamMouseLocation           >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickResizeRgn,        kEventParamKeyModifiers            >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickResizeRgn,        kEventParamMouseButton             >: Basic_EventParameter_Traits< typeMouseButton,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickResizeRgn,        kEventParamClickCount              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickResizeRgn,        kEventParamMouseChord              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickResizeRgn,        kEventParamTabletEventType         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickResizeRgn,        kEventParamTabletPointRec          >: Basic_EventParameter_Traits< typeTabletPointRec,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickResizeRgn,        kEventParamTabletProximityRec      >: Basic_EventParameter_Traits< typeTabletProximityRec,      true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickCollapseRgn,      kEventParamMouseLocation           >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickCollapseRgn,      kEventParamKeyModifiers            >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickCollapseRgn,      kEventParamMouseButton             >: Basic_EventParameter_Traits< typeMouseButton,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickCollapseRgn,      kEventParamClickCount              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickCollapseRgn,      kEventParamMouseChord              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickCollapseRgn,      kEventParamTabletEventType         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickCollapseRgn,      kEventParamTabletPointRec          >: Basic_EventParameter_Traits< typeTabletPointRec,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickCollapseRgn,      kEventParamTabletProximityRec      >: Basic_EventParameter_Traits< typeTabletProximityRec,      true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickCloseRgn,         kEventParamMouseLocation           >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickCloseRgn,         kEventParamKeyModifiers            >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickCloseRgn,         kEventParamMouseButton             >: Basic_EventParameter_Traits< typeMouseButton,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickCloseRgn,         kEventParamClickCount              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickCloseRgn,         kEventParamMouseChord              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickCloseRgn,         kEventParamTabletEventType         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickCloseRgn,         kEventParamTabletPointRec          >: Basic_EventParameter_Traits< typeTabletPointRec,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickCloseRgn,         kEventParamTabletProximityRec      >: Basic_EventParameter_Traits< typeTabletProximityRec,      true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickZoomRgn,          kEventParamMouseLocation           >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickZoomRgn,          kEventParamKeyModifiers            >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickZoomRgn,          kEventParamMouseButton             >: Basic_EventParameter_Traits< typeMouseButton,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickZoomRgn,          kEventParamClickCount              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickZoomRgn,          kEventParamMouseChord              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickZoomRgn,          kEventParamTabletEventType         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickZoomRgn,          kEventParamTabletPointRec          >: Basic_EventParameter_Traits< typeTabletPointRec,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickZoomRgn,          kEventParamTabletProximityRec      >: Basic_EventParameter_Traits< typeTabletProximityRec,      true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickContentRgn,       kEventParamMouseLocation           >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickContentRgn,       kEventParamKeyModifiers            >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickContentRgn,       kEventParamMouseButton             >: Basic_EventParameter_Traits< typeMouseButton,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickContentRgn,       kEventParamClickCount              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickContentRgn,       kEventParamMouseChord              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickContentRgn,       kEventParamTabletEventType         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickContentRgn,       kEventParamTabletPointRec          >: Basic_EventParameter_Traits< typeTabletPointRec,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickContentRgn,       kEventParamTabletProximityRec      >: Basic_EventParameter_Traits< typeTabletProximityRec,      true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickProxyIconRgn,     kEventParamMouseLocation           >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickProxyIconRgn,     kEventParamKeyModifiers            >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickProxyIconRgn,     kEventParamMouseButton             >: Basic_EventParameter_Traits< typeMouseButton,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickProxyIconRgn,     kEventParamClickCount              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickProxyIconRgn,     kEventParamMouseChord              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickProxyIconRgn,     kEventParamTabletEventType         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickProxyIconRgn,     kEventParamTabletPointRec          >: Basic_EventParameter_Traits< typeTabletPointRec,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickProxyIconRgn,     kEventParamTabletProximityRec      >: Basic_EventParameter_Traits< typeTabletProximityRec,      true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickToolbarButtonRgn, kEventParamMouseLocation           >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickToolbarButtonRgn, kEventParamKeyModifiers            >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickToolbarButtonRgn, kEventParamMouseButton             >: Basic_EventParameter_Traits< typeMouseButton,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickToolbarButtonRgn, kEventParamClickCount              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickToolbarButtonRgn, kEventParamMouseChord              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickToolbarButtonRgn, kEventParamTabletEventType         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickToolbarButtonRgn, kEventParamTabletPointRec          >: Basic_EventParameter_Traits< typeTabletPointRec,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickToolbarButtonRgn, kEventParamTabletProximityRec      >: Basic_EventParameter_Traits< typeTabletProximityRec,      true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickStructureRgn,     kEventParamMouseLocation           >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickStructureRgn,     kEventParamKeyModifiers            >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickStructureRgn,     kEventParamMouseButton             >: Basic_EventParameter_Traits< typeMouseButton,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickStructureRgn,     kEventParamClickCount              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickStructureRgn,     kEventParamMouseChord              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickStructureRgn,     kEventParamTabletEventType         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickStructureRgn,     kEventParamTabletPointRec          >: Basic_EventParameter_Traits< typeTabletPointRec,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClickStructureRgn,     kEventParamTabletProximityRec      >: Basic_EventParameter_Traits< typeTabletProximityRec,      true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowCursorChange,          kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowCursorChange,          kEventParamMouseLocation           >: Basic_EventParameter_Traits< typeQDPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowCursorChange,          kEventParamKeyModifiers            >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowCollapse,              kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowCollapsed,             kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowCollapseAll,           kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowExpand,                kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowExpanded,              kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowExpandAll,             kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClose,                 kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowClosed,                kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowCloseAll,              kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowZoom,                  kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowZoomed,                kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowZoomAll,               kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowContextualMenuSelect,  kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowPathSelect,            kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetIdealSize,          kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetIdealSize,          kEventParamDimensions              >: Basic_EventParameter_Traits< typeQDPoint,                 false, true  >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetMinimumSize,        kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetMinimumSize,        kEventParamDimensions              >: Basic_EventParameter_Traits< typeQDPoint,                 false, true  >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetMaximumSize,        kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetMaximumSize,        kEventParamDimensions              >: Basic_EventParameter_Traits< typeQDPoint,                 false, true  >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowConstrain,             kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowConstrain,             kEventParamAvailableBounds         >: Basic_EventParameter_Traits< typeQDRectangle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowConstrain,             kEventParamAttributes              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowConstrain,             kEventParamWindowRegionCode        >: Basic_EventParameter_Traits< typeWindowRegionCode,        true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowConstrain,             kEventParamRgnHandle               >: Basic_EventParameter_Traits< typeQDRgnHandle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowHandleContentClick,    kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowHandleContentClick,    kEventParamMouseLocation           >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowHandleContentClick,    kEventParamKeyModifiers            >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowHandleContentClick,    kEventParamMouseButton             >: Basic_EventParameter_Traits< typeMouseButton,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowHandleContentClick,    kEventParamClickCount              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowHandleContentClick,    kEventParamMouseChord              >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowHandleContentClick,    kEventParamTabletEventType         >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowHandleContentClick,    kEventParamTabletPointRec          >: Basic_EventParameter_Traits< typeTabletPointRec,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowHandleContentClick,    kEventParamTabletProximityRec      >: Basic_EventParameter_Traits< typeTabletProximityRec,      true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowProxyBeginDrag,        kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowProxyEndDrag,          kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowFocusAcquired,         kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowFocusRelinquish,       kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowDrawFrame,             kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowDrawPart,              kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowDrawPart,              kEventParamWindowDefPart           >: Basic_EventParameter_Traits< typeWindowDefPartCode,       true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetRegion,             kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetRegion,             kEventParamWindowRegionCode        >: Basic_EventParameter_Traits< typeWindowRegionCode,        true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetRegion,             kEventParamRgnHandle               >: Basic_EventParameter_Traits< typeQDRgnHandle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowHitTest,               kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowHitTest,               kEventParamMouseLocation           >: Basic_EventParameter_Traits< typeQDPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowHitTest,               kEventParamWindowDefPart           >: Basic_EventParameter_Traits< typeWindowDefPartCode,       false, true  >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowInit,                  kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowInit,                  kEventParamWindowFeatures          >: Basic_EventParameter_Traits< typeUInt32,                  false, true  >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowDispose,               kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowDragHilite,            kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowDragHilite,            kEventParamWindowDragHiliteFlag    >: Basic_EventParameter_Traits< typeBoolean,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowModified,              kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowModified,              kEventParamWindowModifiedFlag      >: Basic_EventParameter_Traits< typeBoolean,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowSetupProxyDragImage,   kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowSetupProxyDragImage,   kEventParamWindowProxyImageRgn     >: Basic_EventParameter_Traits< typeQDRgnHandle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowSetupProxyDragImage,   kEventParamWindowProxyOutlineRgn   >: Basic_EventParameter_Traits< typeQDRgnHandle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowSetupProxyDragImage,   kEventParamWindowProxyGWorldPtr    >: Basic_EventParameter_Traits< typeGWorldPtr,               false, true  >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowStateChanged,          kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowStateChanged,          kEventParamWindowStateChangedFlags >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowMeasureTitle,          kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowMeasureTitle,          kEventParamWindowTitleFullWidth    >: Basic_EventParameter_Traits< typeSInt16,                  false, true  >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowMeasureTitle,          kEventParamWindowTitleTextWidth    >: Basic_EventParameter_Traits< typeSInt16,                  false, true  >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowDrawGrowBox,           kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetGrowImageRegion,    kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetGrowImageRegion,    kEventParamWindowGrowRect          >: Basic_EventParameter_Traits< typeQDRectangle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowGetGrowImageRegion,    kEventParamRgnHandle               >: Basic_EventParameter_Traits< typeQDRgnHandle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassWindow, kEventWindowPaint,                 kEventParamDirectObject            >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};

   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuBeginTracking,      kEventParamDirectObject              >: Basic_EventParameter_Traits< typeMenuRef,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuBeginTracking,      kEventParamCurrentMenuTrackingMode   >: Basic_EventParameter_Traits< typeMenuTrackingMode,        true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuBeginTracking,      kEventParamMenuContext               >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuEndTracking,        kEventParamDirectObject              >: Basic_EventParameter_Traits< typeMenuRef,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuEndTracking,        kEventParamMenuContext               >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuChangeTrackingMode, kEventParamCurrentMenuTrackingMode   >: Basic_EventParameter_Traits< typeMenuTrackingMode,        true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuChangeTrackingMode, kEventParamNewMenuTrackingMode       >: Basic_EventParameter_Traits< typeMenuTrackingMode,        true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuChangeTrackingMode, kEventParamMenuContext               >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuOpening,            kEventParamDirectObject              >: Basic_EventParameter_Traits< typeMenuRef,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuOpening,            kEventParamMenuFirstOpen             >: Basic_EventParameter_Traits< typeBoolean,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuOpening,            kEventParamMenuContext               >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuClosed,             kEventParamDirectObject              >: Basic_EventParameter_Traits< typeMenuRef,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuClosed,             kEventParamMenuContext               >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuTargetItem,         kEventParamDirectObject              >: Basic_EventParameter_Traits< typeMenuRef,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuTargetItem,         kEventParamMenuItemIndex             >: Basic_EventParameter_Traits< typeMenuItemIndex,           true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuTargetItem,         kEventParamMenuCommand               >: Basic_EventParameter_Traits< typeMenuCommand,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuTargetItem,         kEventParamMenuContext               >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuMatchKey,           kEventParamDirectObject              >: Basic_EventParameter_Traits< typeMenuRef,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuMatchKey,           kEventParamEventRef                  >: Basic_EventParameter_Traits< typeEventRef,                true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuMatchKey,           kEventParamMenuEventOptions          >: Basic_EventParameter_Traits< typeMenuEventOptions,        true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuMatchKey,           kEventParamMenuContext               >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuMatchKey,           kEventParamMenuItemIndex             >: Basic_EventParameter_Traits< typeMenuItemIndex,           false, true  >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuEnableItems,        kEventParamDirectObject              >: Basic_EventParameter_Traits< typeMenuRef,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuEnableItems,        kEventParamEnableMenuForKeyEvent     >: Basic_EventParameter_Traits< typeBoolean,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuEnableItems,        kEventParamMenuContext               >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuPopulate,           kEventParamDirectObject              >: Basic_EventParameter_Traits< typeMenuRef,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuPopulate,           kEventParamMenuContext               >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuMeasureItemWidth,   kEventParamDirectObject              >: Basic_EventParameter_Traits< typeMenuRef,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuMeasureItemWidth,   kEventParamMenuItemIndex             >: Basic_EventParameter_Traits< typeMenuItemIndex,           true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuMeasureItemWidth,   kEventParamMenuItemWidth             >: Basic_EventParameter_Traits< typeSInt16,                  false, true  >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuMeasureItemHeight,  kEventParamDirectObject              >: Basic_EventParameter_Traits< typeMenuRef,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuMeasureItemHeight,  kEventParamMenuItemIndex             >: Basic_EventParameter_Traits< typeMenuItemIndex,           true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuMeasureItemHeight,  kEventParamMenuItemHeight            >: Basic_EventParameter_Traits< typeSInt16,                  false, true  >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItem,           kEventParamDirectObject              >: Basic_EventParameter_Traits< typeMenuRef,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItem,           kEventParamCurrentBounds             >: Basic_EventParameter_Traits< typeQDRectangle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItem,           kEventParamMenuItemIndex             >: Basic_EventParameter_Traits< typeMenuItemIndex,           true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItem,           kEventParamMenuItemBounds            >: Basic_EventParameter_Traits< typeQDRectangle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItem,           kEventParamMenuVirtualTop            >: Basic_EventParameter_Traits< typeSInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItem,           kEventParamMenuVirtualBottom         >: Basic_EventParameter_Traits< typeSInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItem,           kEventParamMenuDrawState             >: Basic_EventParameter_Traits< typeThemeMenuState,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItem,           kEventParamMenuItemType              >: Basic_EventParameter_Traits< typeThemeMenuItemType,       true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItem,           kEventParamCGContextRef              >: Basic_EventParameter_Traits< typeCGContextRef,            true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItem,           kEventParamMenuMarkBounds            >: Basic_EventParameter_Traits< typeQDRectangle,             false, true  >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItem,           kEventParamMenuIconBounds            >: Basic_EventParameter_Traits< typeQDRectangle,             false, true  >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItem,           kEventParamMenuTextBounds            >: Basic_EventParameter_Traits< typeQDRectangle,             false, true  >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItem,           kEventParamMenuTextBaseline          >: Basic_EventParameter_Traits< typeSInt16,                  false, true  >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItem,           kEventParamMenuCommandKeyBounds      >: Basic_EventParameter_Traits< typeQDRectangle,             false, true  >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItemContent,    kEventParamDirectObject              >: Basic_EventParameter_Traits< typeMenuRef,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItemContent,    kEventParamMenuItemIndex             >: Basic_EventParameter_Traits< typeMenuItemIndex,           true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItemContent,    kEventParamMenuItemBounds            >: Basic_EventParameter_Traits< typeQDRectangle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItemContent,    kEventParamDeviceDepth               >: Basic_EventParameter_Traits< typeSInt16,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItemContent,    kEventParamDeviceColor               >: Basic_EventParameter_Traits< typeBoolean,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItemContent,    kEventParamCGContextRef              >: Basic_EventParameter_Traits< typeCGContextRef,            true,  false >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItemContent,    kEventParamMenuMarkBounds            >: Basic_EventParameter_Traits< typeQDRectangle,             false, true  >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItemContent,    kEventParamMenuIconBounds            >: Basic_EventParameter_Traits< typeQDRectangle,             false, true  >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItemContent,    kEventParamMenuTextBounds            >: Basic_EventParameter_Traits< typeQDRectangle,             false, true  >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItemContent,    kEventParamMenuTextBaseline          >: Basic_EventParameter_Traits< typeSInt16,                  false, true  >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDrawItemContent,    kEventParamMenuCommandKeyBounds      >: Basic_EventParameter_Traits< typeQDRectangle,             false, true  >{};
   template <> struct EventParameter_Traits< kEventClassMenu, kEventMenuDispose,            kEventParamDirectObject              >: Basic_EventParameter_Traits< typeMenuRef,                 true,  false >{};

   template <> struct EventParameter_Traits< kEventClassCommand, kEventCommandProcess,      kEventParamDirectObject >: Basic_EventParameter_Traits< typeHICommand,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassCommand, kEventCommandProcess,      kEventParamKeyModifiers >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassCommand, kEventCommandUpdateStatus, kEventParamDirectObject >: Basic_EventParameter_Traits< typeHICommand,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassCommand, kEventCommandUpdateStatus, kEventParamMenuContext  >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};

   template <> struct EventParameter_Traits< kEventClassControl, kEventControlInitialize,                 kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlInitialize,                 kEventParamInitCollection                 >: Basic_EventParameter_Traits< typeCollection,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlInitialize,                 kEventParamControlFeatures                >: Basic_EventParameter_Traits< typeUInt32,                  false, true  >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlDispose,                    kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetOptimalBounds,           kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetOptimalBounds,           kEventParamControlOptimalBounds           >: Basic_EventParameter_Traits< typeQDRectangle,             false, true  >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetOptimalBounds,           kEventParamControlOptimalBaselineOffset   >: Basic_EventParameter_Traits< typeSInt16,                  false, true  >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlHit,                        kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlHit,                        kEventParamControlPart                    >: Basic_EventParameter_Traits< typeControlPartCode,         true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlHit,                        kEventParamKeyModifiers                   >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlSimulateHit,                kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlHitTest,                    kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlHitTest,                    kEventParamMouseLocation                  >: Basic_EventParameter_Traits< typeQDPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlHitTest,                    kEventParamControlPart                    >: Basic_EventParameter_Traits< typeControlPartCode,         false, true  >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlDraw,                       kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlDraw,                       kEventParamControlPart                    >: Basic_EventParameter_Traits< typeControlPartCode,         true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlDraw,                       kEventParamGrafPort                       >: Basic_EventParameter_Traits< typeGrafPtr,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlApplyBackground,            kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlApplyBackground,            kEventParamControlSubControl              >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlApplyBackground,            kEventParamControlDrawDepth               >: Basic_EventParameter_Traits< typeSInt16,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlApplyBackground,            kEventParamControlDrawInColor             >: Basic_EventParameter_Traits< typeBoolean,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlApplyBackground,            kEventParamGrafPort                       >: Basic_EventParameter_Traits< typeGrafPtr,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlApplyTextColor,             kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlApplyTextColor,             kEventParamControlSubControl              >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlApplyTextColor,             kEventParamControlDrawDepth               >: Basic_EventParameter_Traits< typeSInt16,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlApplyTextColor,             kEventParamControlDrawInColor             >: Basic_EventParameter_Traits< typeBoolean,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlApplyTextColor,             kEventParamCGContextRef                   >: Basic_EventParameter_Traits< typeCGContextRef,            true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlApplyTextColor,             kEventParamGrafPort                       >: Basic_EventParameter_Traits< typeGrafPtr,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlSetFocusPart,               kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlSetFocusPart,               kEventParamControlPart                    >: Basic_EventParameter_Traits< typeControlPartCode,         true,  true  >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetFocusPart,               kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetFocusPart,               kEventParamControlPart                    >: Basic_EventParameter_Traits< typeControlPartCode,         false, true  >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlActivate,                   kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlDeactivate,                 kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlSetCursor,                  kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlSetCursor,                  kEventParamMouseLocation                  >: Basic_EventParameter_Traits< typeQDPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlSetCursor,                  kEventParamKeyModifiers                   >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlContextualMenuClick,        kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlContextualMenuClick,        kEventParamMouseLocation                  >: Basic_EventParameter_Traits< typeQDPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlClick,                      kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlClick,                      kEventParamMouseLocation                  >: Basic_EventParameter_Traits< typeHIPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlClick,                      kEventParamKeyModifiers                   >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlClick,                      kEventParamMouseButton                    >: Basic_EventParameter_Traits< typeMouseButton,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlClick,                      kEventParamClickCount                     >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlClick,                      kEventParamMouseChord                     >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlClick,                      kEventParamTabletEventType                >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlClick,                      kEventParamTabletPointRec                 >: Basic_EventParameter_Traits< typeTabletPointRec,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlClick,                      kEventParamTabletProximityRec             >: Basic_EventParameter_Traits< typeTabletProximityRec,      true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlTrack,                      kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlTrack,                      kEventParamMouseLocation                  >: Basic_EventParameter_Traits< typeQDPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlTrack,                      kEventParamKeyModifiers                   >: Basic_EventParameter_Traits< typeUInt32,                  true,  true  >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlTrack,                      kEventParamControlAction                  >: Basic_EventParameter_Traits< typeControlActionUPP,        true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlTrack,                      kEventParamControlPart                    >: Basic_EventParameter_Traits< typeControlPartCode,         false, true  >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetScrollToHereStartPoint,  kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetScrollToHereStartPoint,  kEventParamMouseLocation                  >: Basic_EventParameter_Traits< typeQDPoint,                 true,  true  >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetScrollToHereStartPoint,  kEventParamKeyModifiers                   >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetIndicatorDragConstraint, kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetIndicatorDragConstraint, kEventParamMouseLocation                  >: Basic_EventParameter_Traits< typeQDPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetIndicatorDragConstraint, kEventParamKeyModifiers                   >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetIndicatorDragConstraint, kEventParamControlIndicatorDragConstraint >: Basic_EventParameter_Traits< typeIndicatorDragConstraint, false, true  >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlIndicatorMoved,             kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlIndicatorMoved,             kEventParamControlIndicatorRegion         >: Basic_EventParameter_Traits< typeQDRgnHandle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlIndicatorMoved,             kEventParamControlIsGhosting              >: Basic_EventParameter_Traits< typeBoolean,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGhostingFinished,           kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGhostingFinished,           kEventParamControlIndicatorOffset         >: Basic_EventParameter_Traits< typeQDPoint,                 true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetActionProcPart,          kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetActionProcPart,          kEventParamKeyModifiers                   >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetActionProcPart,          kEventParamControlPart                    >: Basic_EventParameter_Traits< typeControlPartCode,         true,  true  >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetPartRegion,              kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetPartRegion,              kEventParamControlPart                    >: Basic_EventParameter_Traits< typeControlPartCode,         true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetPartRegion,              kEventParamControlRegion                  >: Basic_EventParameter_Traits< typeQDRgnHandle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetPartBounds,              kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetPartBounds,              kEventParamControlPart                    >: Basic_EventParameter_Traits< typeControlPartCode,         true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetPartBounds,              kEventParamControlPartBounds              >: Basic_EventParameter_Traits< typeQDRectangle,             false, true  >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlSetData,                    kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlSetData,                    kEventParamControlPart                    >: Basic_EventParameter_Traits< typeControlPartCode,         true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlSetData,                    kEventParamControlDataTag                 >: Basic_EventParameter_Traits< typeEnumeration,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlSetData,                    kEventParamControlDataBuffer              >: Basic_EventParameter_Traits< typePtr,                     true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlSetData,                    kEventParamControlDataBufferSize          >: Basic_EventParameter_Traits< typeSInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetData,                    kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetData,                    kEventParamControlPart                    >: Basic_EventParameter_Traits< typeControlPartCode,         true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetData,                    kEventParamControlDataTag                 >: Basic_EventParameter_Traits< typeEnumeration,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetData,                    kEventParamControlDataBuffer              >: Basic_EventParameter_Traits< typePtr,                     true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlGetData,                    kEventParamControlDataBufferSize          >: Basic_EventParameter_Traits< typeSInt32,                  true,  true  >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlValueFieldChanged,          kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlAddedSubControl,            kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlAddedSubControl,            kEventParamControlSubControl              >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlRemovingSubControl,         kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlRemovingSubControl,         kEventParamControlSubControl              >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlBoundsChanged,              kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlBoundsChanged,              kEventParamAttributes                     >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlBoundsChanged,              kEventParamOriginalBounds                 >: Basic_EventParameter_Traits< typeQDRectangle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlBoundsChanged,              kEventParamPreviousBounds                 >: Basic_EventParameter_Traits< typeQDRectangle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlBoundsChanged,              kEventParamCurrentBounds                  >: Basic_EventParameter_Traits< typeQDRectangle,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlOwningWindowChanged,        kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlOwningWindowChanged,        kEventParamAttributes                     >: Basic_EventParameter_Traits< typeUInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlOwningWindowChanged,        kEventParamControlOriginalOwningWindow    >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlOwningWindowChanged,        kEventParamControlCurrentOwningWindow     >: Basic_EventParameter_Traits< typeWindowRef,               true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlArbitraryMessage,           kEventParamDirectObject                   >: Basic_EventParameter_Traits< typeControlRef,              true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlArbitraryMessage,           kEventParamControlMessage                 >: Basic_EventParameter_Traits< typeSInt16,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlArbitraryMessage,           kEventParamControlParam                   >: Basic_EventParameter_Traits< typeSInt32,                  true,  false >{};
   template <> struct EventParameter_Traits< kEventClassControl, kEventControlArbitraryMessage,           kEventParamControlResult                  >: Basic_EventParameter_Traits< typeSInt32,                  false, true  >{};

   template <> struct EventParameter_Traits< kEventClassVolume, kEventVolumeMounted,   kEventParamDirectObject >: Basic_EventParameter_Traits< typeFSVolumeRefNum,          true,  false >{};
   template <> struct EventParameter_Traits< kEventClassVolume, kEventVolumeUnmounted, kEventParamDirectObject >: Basic_EventParameter_Traits< typeFSVolumeRefNum,          true,  false >{};

   template <> struct EventParameter_Traits< kEventClassAppearance, kEventAppearanceScrollBarVariantChanged, kEventParamNewScrollBarVariant >: Basic_EventParameter_Traits< typeSInt16,                  true,  false >{};

   template <> struct EventParameter_Traits< kEventClassService, kEventServiceCopy,      kEventParamScrapRef           >: Basic_EventParameter_Traits< typeScrapRef,                true,  false >{};
   template <> struct EventParameter_Traits< kEventClassService, kEventServicePaste,     kEventParamScrapRef           >: Basic_EventParameter_Traits< typeScrapRef,                true,  false >{};
   template <> struct EventParameter_Traits< kEventClassService, kEventServiceGetTypes,  kEventParamServiceCopyTypes   >: Basic_EventParameter_Traits< typeCFMutableArrayRef,       true,  false >{};
   template <> struct EventParameter_Traits< kEventClassService, kEventServiceGetTypes,  kEventParamServicePasteTypes  >: Basic_EventParameter_Traits< typeCFMutableArrayRef,       true,  false >{};
   template <> struct EventParameter_Traits< kEventClassService, kEventServicePerform,   kEventParamScrapRef           >: Basic_EventParameter_Traits< typeScrapRef,                true,  false >{};
   template <> struct EventParameter_Traits< kEventClassService, kEventServicePerform,   kEventParamServiceMessageName >: Basic_EventParameter_Traits< typeCFStringRef,             true,  false >{};
   template <> struct EventParameter_Traits< kEventClassService, kEventServicePerform,   kEventParamServiceUserData    >: Basic_EventParameter_Traits< typeCFStringRef,             true,  false >{};

/* ... */

   
   using ::EventHandlerRef;
   using ::EventHandlerCallRef;
   
   struct EventHandlerUPP_Details: Basic_UPP_Details< ::EventHandlerUPP,
                                                      ::EventHandlerProcPtr,
                                                      ::NewEventHandlerUPP,
                                                      ::DisposeEventHandlerUPP,
                                                      ::InvokeEventHandlerUPP >
     {};

   typedef UPP< EventHandlerUPP_Details > EventHandlerUPP;
   
   using ::EventHandlerProcPtr;
   
   inline Nucleus::Owned<EventHandlerUPP> NewEventHandlerUPP( EventHandlerProcPtr p )
     {
      return NewUPP<EventHandlerUPP>( p );
     }

   inline void DisposeEventHandlerUPP( Nucleus::Owned<EventHandlerUPP> )
     {
     }
   
   void InvokeEventHandlerUPP( EventHandlerCallRef inHandlerCallRef,
                               EventRef            inEvent,
                               RefCon              inUserData,
                               EventHandlerUPP     userUPP );

   using ::EventTargetRef;
   
   using ::GetWindowEventTarget;
   using ::GetControlEventTarget;
   using ::GetMenuEventTarget;
   using ::GetApplicationEventTarget;
   using ::GetUserFocusEventTarget;
   using ::GetEventDispatcherTarget;

  }

namespace Nucleus
  {
   template <>
   struct Disposer< Nitrogen::EventHandlerRef >: public std::unary_function< Nitrogen::EventHandlerRef, void >,
                                                 private Nitrogen::DefaultDestructionOSStatusPolicy
     {
      void operator()( EventHandlerRef toDispose ) const
        {
         (void) Nitrogen::CarbonEventManagerErrorsRegistrationDependency();
         
         HandleDestructionOSStatus( ::RemoveEventHandler( toDispose ) );
        }
     };
  }

namespace Nitrogen
  {

   /* Level 0 (UPP) InstallEventHandler, for multiple or single events */
      Nucleus::Owned<EventHandlerRef> InstallEventHandler( EventTargetRef         inTarget,
                                                  EventHandlerUPP        inHandler,
                                                  UInt32                 inNumTypes,
                                                  const EventTypeSpec *  inList,
                                                  const void *           inUserData = 0 );

      Nucleus::Owned<EventHandlerRef> InstallEventHandler( EventTargetRef         inTarget,
                                                  EventHandlerUPP        inHandler,
                                                  EventClass             eventClass,
                                                  CarbonEventKind        eventKind,
                                                  const void *           inUserData = 0 );
   
   /* Level 1 (UPP-creating) InstallEventHandler, for multiple or single events */

      template < EventHandlerProcPtr handler >
      Nucleus::Owned<EventHandlerRef> InstallEventHandler( EventTargetRef         inTarget,
                                                  UInt32                 inNumTypes,
                                                  const EventTypeSpec *  inList,
                                                  const void *           inUserData = 0 )
        {
         return InstallEventHandler( inTarget,
                                     StaticUPP< EventHandlerUPP, handler >(),
                                     inNumTypes,
                                     inList,
                                     inUserData );
        }


      template < EventHandlerProcPtr handler >
      Nucleus::Owned<EventHandlerRef> InstallEventHandler( EventTargetRef         inTarget,
                                                  EventClass             eventClass,
                                                  CarbonEventKind        eventKind,
                                                  const void *           inUserData = 0 )
        {
         return InstallEventHandler( inTarget,
                                     StaticUPP< EventHandlerUPP, handler >(),
                                     eventClass,
                                     eventKind,
                                     inUserData );
        }
   
   /* Exception catching EventHandler glue, used in levels 2 and 3 */

      struct EventHandler_ExceptionGlue
        {
         typedef void (*Handler)( EventHandlerCallRef, EventRef, void * );
                  
         static ::OSStatus CatchExceptions( EventHandlerCallRef inHandlerCallRef,
                                            EventRef inEvent,
                                            void *inUserData,
                                            Handler handler );
        };
         
      template < EventHandler_ExceptionGlue::Handler handler >
      struct EventHandler_Bound_ExceptionGlue
        {
         // In CodeWarrior 7.2, all of the specializations of a pascal template function
         // get the same linkage name.  The linker strips all but one, and chaos ensues.
         // But this problem does not affect pascal non-template member functions of template classes.
         static pascal ::OSStatus ProvideHandler( EventHandlerCallRef inHandlerCallRef,
                                                  EventRef inEvent,
                                                  void *inUserData )
           {
            return EventHandler_ExceptionGlue::CatchExceptions( inHandlerCallRef, inEvent, inUserData, handler );
           }
         
         static EventHandlerUPP UPP()
           {
            return StaticUPPWorkaround< EventHandlerUPP, EventHandler_Bound_ExceptionGlue::ProvideHandler >::StaticUPP();
           }
        };
      
   /* Glue for adjusting the object parameters of level 2 EventHandlers */
   
      template < class Object >
      struct EventHandler_ObjectGlue
        {
         typedef void (*Handler)( Object, EventHandlerCallRef, EventRef );
         
         static void ExtractObjectParameter( EventHandlerCallRef inHandlerCallRef, EventRef inEvent, void *inUserData, Handler handler )
           {
            return handler( Nucleus::ObjectParameterTraits<Object>::ConvertFromPointer( inUserData ), inHandlerCallRef, inEvent );
           }
        };
   
      template <>
      struct EventHandler_ObjectGlue< void >
        {
         typedef void (*Handler)( EventHandlerCallRef, EventRef );
         
         static void ExtractObjectParameter( EventHandlerCallRef inHandlerCallRef, EventRef inEvent, void *, Handler handler )
           {
            return handler( inHandlerCallRef, inEvent );
           }
        };
      
      template < class Object,
                 typename EventHandler_ObjectGlue<Object>::Handler handler >
      struct EventHandler_Bound_ObjectGlue
        {
         static void ProvideHandler( EventHandlerCallRef inHandlerCallRef,
                                     EventRef inEvent,
                                     void *inUserData )
           {
            return EventHandler_ObjectGlue<Object>::ExtractObjectParameter( inHandlerCallRef, inEvent, inUserData, handler );
           }
         
         static EventHandlerUPP UPP()
           {
            return EventHandler_Bound_ExceptionGlue< EventHandler_Bound_ObjectGlue::ProvideHandler >::UPP();
           }
        };
      
   /* Level 2 (basic Nitrogen signature) InstallEventHandler */

      template < class Object, typename EventHandler_ObjectGlue<Object>::Handler handler >
      Nucleus::Owned<EventHandlerRef> InstallEventHandler( EventTargetRef                                 inTarget,
                                                  UInt32                                         inNumTypes,
                                                  const EventTypeSpec *                          inList,
                                                  typename Nucleus::ObjectParameterTraits<Object>::Type   inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler( inTarget,
                                     EventHandler_Bound_ObjectGlue< Object, handler >::UPP(),
                                     inNumTypes,
                                     inList,
                                     Nucleus::ObjectParameterTraits<Object>::ConvertToPointer( inUserData ) );
        }

      template < class Object, typename EventHandler_ObjectGlue<Object>::Handler handler >
      Nucleus::Owned<EventHandlerRef> InstallEventHandler( EventTargetRef                                 inTarget,
                                                  EventClass                                     eventClass,
                                                  CarbonEventKind                                eventKind,
                                                  typename Nucleus::ObjectParameterTraits<Object>::Type   inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler( inTarget,
                                     EventHandler_Bound_ObjectGlue< Object, handler >::UPP(),
                                     eventClass,
                                     eventKind,
                                     Nucleus::ObjectParameterTraits<Object>::ConvertToPointer( inUserData ) );
        }

   /* Glue for storing an EventHandler's result into the event parameter */
   
      template < EventParamType resultType >
      struct EventHandler_ResultGlue
        {
         typedef typename DescType_Traits< resultType >::Result Result;
         
         typedef Result (*Handler)( EventHandlerCallRef, EventRef, void * );
         
         static void StoreResult( EventHandlerCallRef call,
                                  EventRef event,
                                  void *userData,
                                  EventParamName resultParameter,
                                  Handler handler )
           {
            SetEventParameter< resultType >( event,
                                             resultParameter, 
                                             handler( call, event, userData ) );
           }
        };      
 
      template < EventParamType resultType,
                 ::EventParamName resultParameter,
                 typename EventHandler_ResultGlue< resultType >::Handler handler >
      struct EventHandler_Bound_ResultGlue
        {
         static void ProvideResultNameAndHandler( EventHandlerCallRef call,
                                                  EventRef event,
                                                  void *userData )
           {
            EventHandler_ResultGlue< resultType >::StoreResult( call, event, userData, resultParameter, handler );
           }
         
         static EventHandlerUPP UPP()
           {
            return EventHandler_Bound_ExceptionGlue< EventHandler_Bound_ResultGlue::ProvideResultNameAndHandler >::UPP();
           }
        };

      template < ::EventParamName resultParameter, EventHandler_ExceptionGlue::Handler handler >
      struct EventHandler_Bound_ResultGlue< typeNull, resultParameter, handler >
        {
         static EventHandlerUPP UPP()
           {
            return EventHandler_Bound_ExceptionGlue< handler >::UPP();
           }
        };
      
   /* Types which represent event parameter lists and parameter type lists */
	
	static const EventParamType typeUndef = EventParamType( 0 );
	
      template < ::EventParamName name0 = 0, ::EventParamName name1 = 0, ::EventParamName name2 = 0, ::EventParamName name3 = 0,
                 ::EventParamName name4 = 0, ::EventParamName name5 = 0, ::EventParamName name6 = 0, ::EventParamName name7 = 0 >
      struct EventParamName_List
         : Nucleus::ArraySingleton< ::EventParamName >::ArrayType< name0, name1, name2, name3, name4, name5, name6, name7 >
         {};

      template < EventParamType type0 = typeUndef, EventParamType type1 = typeUndef, EventParamType type2 = typeUndef, EventParamType type3 = typeUndef,
                 EventParamType type4 = typeUndef, EventParamType type5 = typeUndef, EventParamType type6 = typeUndef, EventParamType type7 = typeUndef >
      struct EventParamType_List
         : Nucleus::ArraySingleton< EventParamType >::ArrayType< type0, type1, type2, type3, type4, type5, type6, type7 >
         {};

      // These specializations take the load off a construct in ArraySingleton.h that CodeWarrior 8.3 can't handle.
         template <>
         struct EventParamName_List< 0, 0, 0, 0, 0, 0, 0, 0 >
            : Nucleus::ArraySingleton0< ::EventParamName >
            {};
      
         template <>
         struct EventParamType_List< typeUndef, typeUndef, typeUndef, typeUndef, typeUndef, typeUndef, typeUndef, typeUndef >
            : Nucleus::ArraySingleton0< EventParamType >
            {};
      
   /* Glue for extracting an EventHandler's parameters */
      
      template < class Result, class Object, class ParameterTypes, int parameterCount = ParameterTypes::size >
      struct EventHandler_ParameterGlue;
      
      template < class Result, class Object, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, Object, ParameterTypes, 8 >
        {
         typedef Result (*Handler)( Object,
                                    typename DescType_Traits< ParameterTypes::item0 >::Result,
                                    typename DescType_Traits< ParameterTypes::item1 >::Result,
                                    typename DescType_Traits< ParameterTypes::item2 >::Result,
                                    typename DescType_Traits< ParameterTypes::item3 >::Result,
                                    typename DescType_Traits< ParameterTypes::item4 >::Result,
                                    typename DescType_Traits< ParameterTypes::item5 >::Result,
                                    typename DescType_Traits< ParameterTypes::item6 >::Result,
                                    typename DescType_Traits< ParameterTypes::item7 >::Result );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef event,
                                      void *userData,
                                      const ::EventParamName parameterNames[],
                                      Handler handler )
           {
            return handler( Nucleus::ObjectParameterTraits<Object>::ConvertFromPointer( userData ),
                            EventParameter< ParameterTypes::item0 >( event, parameterNames[0] ),
                            EventParameter< ParameterTypes::item1 >( event, parameterNames[1] ),
                            EventParameter< ParameterTypes::item2 >( event, parameterNames[2] ),
                            EventParameter< ParameterTypes::item3 >( event, parameterNames[3] ),
                            EventParameter< ParameterTypes::item4 >( event, parameterNames[4] ),
                            EventParameter< ParameterTypes::item5 >( event, parameterNames[5] ),
                            EventParameter< ParameterTypes::item6 >( event, parameterNames[6] ),
                            EventParameter< ParameterTypes::item7 >( event, parameterNames[7] ) );
           }
        };
      
      template < class Result, class Object, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, Object, ParameterTypes, 7 >
        {
         typedef Result (*Handler)( Object,
                                    typename DescType_Traits< ParameterTypes::item0 >::Result,
                                    typename DescType_Traits< ParameterTypes::item1 >::Result,
                                    typename DescType_Traits< ParameterTypes::item2 >::Result,
                                    typename DescType_Traits< ParameterTypes::item3 >::Result,
                                    typename DescType_Traits< ParameterTypes::item4 >::Result,
                                    typename DescType_Traits< ParameterTypes::item5 >::Result,
                                    typename DescType_Traits< ParameterTypes::item6 >::Result );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef event,
                                      void *userData,
                                      const ::EventParamName parameterNames[],
                                      Handler handler )
           {
            return handler( Nucleus::ObjectParameterTraits<Object>::ConvertFromPointer( userData ),
                            EventParameter< ParameterTypes::item0 >( event, parameterNames[0] ),
                            EventParameter< ParameterTypes::item1 >( event, parameterNames[1] ),
                            EventParameter< ParameterTypes::item2 >( event, parameterNames[2] ),
                            EventParameter< ParameterTypes::item3 >( event, parameterNames[3] ),
                            EventParameter< ParameterTypes::item4 >( event, parameterNames[4] ),
                            EventParameter< ParameterTypes::item5 >( event, parameterNames[5] ),
                            EventParameter< ParameterTypes::item6 >( event, parameterNames[6] ) );
           }
        };
      
      template < class Result, class Object, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, Object, ParameterTypes, 6 >
        {
         typedef Result (*Handler)( Object,
                                    typename DescType_Traits< ParameterTypes::item0 >::Result,
                                    typename DescType_Traits< ParameterTypes::item1 >::Result,
                                    typename DescType_Traits< ParameterTypes::item2 >::Result,
                                    typename DescType_Traits< ParameterTypes::item3 >::Result,
                                    typename DescType_Traits< ParameterTypes::item4 >::Result,
                                    typename DescType_Traits< ParameterTypes::item5 >::Result );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef event,
                                      void *userData,
                                      const ::EventParamName parameterNames[],
                                      Handler handler )
           {
            return handler( Nucleus::ObjectParameterTraits<Object>::ConvertFromPointer( userData ),
                            EventParameter< ParameterTypes::item0 >( event, parameterNames[0] ),
                            EventParameter< ParameterTypes::item1 >( event, parameterNames[1] ),
                            EventParameter< ParameterTypes::item2 >( event, parameterNames[2] ),
                            EventParameter< ParameterTypes::item3 >( event, parameterNames[3] ),
                            EventParameter< ParameterTypes::item4 >( event, parameterNames[4] ),
                            EventParameter< ParameterTypes::item5 >( event, parameterNames[5] ) );
           }
        };
      
      template < class Result, class Object, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, Object, ParameterTypes, 5 >
        {
         typedef Result (*Handler)( Object,
                                    typename DescType_Traits< ParameterTypes::item0 >::Result,
                                    typename DescType_Traits< ParameterTypes::item1 >::Result,
                                    typename DescType_Traits< ParameterTypes::item2 >::Result,
                                    typename DescType_Traits< ParameterTypes::item3 >::Result,
                                    typename DescType_Traits< ParameterTypes::item4 >::Result );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef event,
                                      void *userData,
                                      const ::EventParamName parameterNames[],
                                      Handler handler )
           {
            return handler( Nucleus::ObjectParameterTraits<Object>::ConvertFromPointer( userData ),
                            EventParameter< ParameterTypes::item0 >( event, parameterNames[0] ),
                            EventParameter< ParameterTypes::item1 >( event, parameterNames[1] ),
                            EventParameter< ParameterTypes::item2 >( event, parameterNames[2] ),
                            EventParameter< ParameterTypes::item3 >( event, parameterNames[3] ),
                            EventParameter< ParameterTypes::item4 >( event, parameterNames[4] ) );
           }
        };
      
      template < class Result, class Object, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, Object, ParameterTypes, 4 >
        {
         typedef Result (*Handler)( Object,
                                    typename DescType_Traits< ParameterTypes::item0 >::Result,
                                    typename DescType_Traits< ParameterTypes::item1 >::Result,
                                    typename DescType_Traits< ParameterTypes::item2 >::Result,
                                    typename DescType_Traits< ParameterTypes::item3 >::Result );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef event,
                                      void *userData,
                                      const ::EventParamName parameterNames[],
                                      Handler handler )
           {
            return handler( Nucleus::ObjectParameterTraits<Object>::ConvertFromPointer( userData ),
                            EventParameter< ParameterTypes::item0 >( event, parameterNames[0] ),
                            EventParameter< ParameterTypes::item1 >( event, parameterNames[1] ),
                            EventParameter< ParameterTypes::item2 >( event, parameterNames[2] ),
                            EventParameter< ParameterTypes::item3 >( event, parameterNames[3] ) );
           }
        };
      
      template < class Result, class Object, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, Object, ParameterTypes, 3 >
        {
         typedef Result (*Handler)( Object,
                                    typename DescType_Traits< ParameterTypes::item0 >::Result,
                                    typename DescType_Traits< ParameterTypes::item1 >::Result,
                                    typename DescType_Traits< ParameterTypes::item2 >::Result );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef event,
                                      void *userData,
                                      const ::EventParamName parameterNames[],
                                      Handler handler )
           {
            return handler( Nucleus::ObjectParameterTraits<Object>::ConvertFromPointer( userData ),
                            EventParameter< ParameterTypes::item0 >( event, parameterNames[0] ),
                            EventParameter< ParameterTypes::item1 >( event, parameterNames[1] ),
                            EventParameter< ParameterTypes::item2 >( event, parameterNames[2] ) );
           }
        };
      
      template < class Result, class Object, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, Object, ParameterTypes, 2 >
        {
         typedef Result (*Handler)( Object,
                                    typename DescType_Traits< ParameterTypes::item0 >::Result,
                                    typename DescType_Traits< ParameterTypes::item1 >::Result );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef event,
                                      void *userData,
                                      const ::EventParamName parameterNames[],
                                      Handler handler )
           {
            return handler( Nucleus::ObjectParameterTraits<Object>::ConvertFromPointer( userData ),
                            EventParameter< ParameterTypes::item0 >( event, parameterNames[0] ),
                            EventParameter< ParameterTypes::item1 >( event, parameterNames[1] ) );
           }
        };
      
      template < class Result, class Object, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, Object, ParameterTypes, 1 >
        {
         typedef Result (*Handler)( Object,
                                    typename DescType_Traits< ParameterTypes::item0 >::Result );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef event,
                                      void *userData,
                                      const ::EventParamName parameterNames[],
                                      Handler handler )
           {
            return handler( Nucleus::ObjectParameterTraits<Object>::ConvertFromPointer( userData ),
                            EventParameter< ParameterTypes::item0 >( event, parameterNames[0] ) );
           }
        };
      
      template < class Result, class Object, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, Object, ParameterTypes, 0 >
        {
         typedef Result (*Handler)( Object );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef,
                                      void *userData,
                                      const ::EventParamName[],
                                      Handler handler )
           {
            return handler( Nucleus::ObjectParameterTraits<Object>::ConvertFromPointer( userData ) );
           }
        };

      template < class Result, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, void, ParameterTypes, 8 >
        {
         typedef Result (*Handler)( typename DescType_Traits< ParameterTypes::item0 >::Result,
                                    typename DescType_Traits< ParameterTypes::item1 >::Result,
                                    typename DescType_Traits< ParameterTypes::item2 >::Result,
                                    typename DescType_Traits< ParameterTypes::item3 >::Result,
                                    typename DescType_Traits< ParameterTypes::item4 >::Result,
                                    typename DescType_Traits< ParameterTypes::item5 >::Result,
                                    typename DescType_Traits< ParameterTypes::item6 >::Result,
                                    typename DescType_Traits< ParameterTypes::item7 >::Result );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef event,
                                      void *,
                                      const ::EventParamName parameterNames[],
                                      Handler handler )
           {
            return handler( EventParameter< ParameterTypes::item0 >( event, parameterNames[0] ),
                            EventParameter< ParameterTypes::item1 >( event, parameterNames[1] ),
                            EventParameter< ParameterTypes::item2 >( event, parameterNames[2] ),
                            EventParameter< ParameterTypes::item3 >( event, parameterNames[3] ),
                            EventParameter< ParameterTypes::item4 >( event, parameterNames[4] ),
                            EventParameter< ParameterTypes::item5 >( event, parameterNames[5] ),
                            EventParameter< ParameterTypes::item6 >( event, parameterNames[6] ),
                            EventParameter< ParameterTypes::item7 >( event, parameterNames[7] ) );
           }
        };

      template < class Result, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, void, ParameterTypes, 7 >
        {
         typedef Result (*Handler)( typename DescType_Traits< ParameterTypes::item0 >::Result,
                                    typename DescType_Traits< ParameterTypes::item1 >::Result,
                                    typename DescType_Traits< ParameterTypes::item2 >::Result,
                                    typename DescType_Traits< ParameterTypes::item3 >::Result,
                                    typename DescType_Traits< ParameterTypes::item4 >::Result,
                                    typename DescType_Traits< ParameterTypes::item5 >::Result,
                                    typename DescType_Traits< ParameterTypes::item6 >::Result );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef event,
                                      void *,
                                      const ::EventParamName parameterNames[],
                                      Handler handler )
           {
            return handler( EventParameter< ParameterTypes::item0 >( event, parameterNames[0] ),
                            EventParameter< ParameterTypes::item1 >( event, parameterNames[1] ),
                            EventParameter< ParameterTypes::item2 >( event, parameterNames[2] ),
                            EventParameter< ParameterTypes::item3 >( event, parameterNames[3] ),
                            EventParameter< ParameterTypes::item4 >( event, parameterNames[4] ),
                            EventParameter< ParameterTypes::item5 >( event, parameterNames[5] ),
                            EventParameter< ParameterTypes::item6 >( event, parameterNames[6] ) );
           }
        };

      template < class Result, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, void, ParameterTypes, 6 >
        {
         typedef Result (*Handler)( typename DescType_Traits< ParameterTypes::item0 >::Result,
                                    typename DescType_Traits< ParameterTypes::item1 >::Result,
                                    typename DescType_Traits< ParameterTypes::item2 >::Result,
                                    typename DescType_Traits< ParameterTypes::item3 >::Result,
                                    typename DescType_Traits< ParameterTypes::item4 >::Result,
                                    typename DescType_Traits< ParameterTypes::item5 >::Result );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef event,
                                      void *,
                                      const ::EventParamName parameterNames[],
                                      Handler handler )
           {
            return handler( EventParameter< ParameterTypes::item0 >( event, parameterNames[0] ),
                            EventParameter< ParameterTypes::item1 >( event, parameterNames[1] ),
                            EventParameter< ParameterTypes::item2 >( event, parameterNames[2] ),
                            EventParameter< ParameterTypes::item3 >( event, parameterNames[3] ),
                            EventParameter< ParameterTypes::item4 >( event, parameterNames[4] ),
                            EventParameter< ParameterTypes::item5 >( event, parameterNames[5] ) );
           }
        };

      template < class Result, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, void, ParameterTypes, 5 >
        {
         typedef Result (*Handler)( typename DescType_Traits< ParameterTypes::item0 >::Result,
                                    typename DescType_Traits< ParameterTypes::item1 >::Result,
                                    typename DescType_Traits< ParameterTypes::item2 >::Result,
                                    typename DescType_Traits< ParameterTypes::item3 >::Result,
                                    typename DescType_Traits< ParameterTypes::item4 >::Result );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef event,
                                      void *,
                                      const ::EventParamName parameterNames[],
                                      Handler handler )
           {
            return handler( EventParameter< ParameterTypes::item0 >( event, parameterNames[0] ),
                            EventParameter< ParameterTypes::item1 >( event, parameterNames[1] ),
                            EventParameter< ParameterTypes::item2 >( event, parameterNames[2] ),
                            EventParameter< ParameterTypes::item3 >( event, parameterNames[3] ),
                            EventParameter< ParameterTypes::item4 >( event, parameterNames[4] ) );
           }
        };

      template < class Result, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, void, ParameterTypes, 4 >
        {
         typedef Result (*Handler)( typename DescType_Traits< ParameterTypes::item0 >::Result,
                                    typename DescType_Traits< ParameterTypes::item1 >::Result,
                                    typename DescType_Traits< ParameterTypes::item2 >::Result,
                                    typename DescType_Traits< ParameterTypes::item3 >::Result );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef event,
                                      void *,
                                      const ::EventParamName parameterNames[],
                                      Handler handler )
           {
            return handler( EventParameter< ParameterTypes::item0 >( event, parameterNames[0] ),
                            EventParameter< ParameterTypes::item1 >( event, parameterNames[1] ),
                            EventParameter< ParameterTypes::item2 >( event, parameterNames[2] ),
                            EventParameter< ParameterTypes::item3 >( event, parameterNames[3] ) );
           }
        };

      template < class Result, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, void, ParameterTypes, 3 >
        {
         typedef Result (*Handler)( typename DescType_Traits< ParameterTypes::item0 >::Result,
                                    typename DescType_Traits< ParameterTypes::item1 >::Result,
                                    typename DescType_Traits< ParameterTypes::item2 >::Result );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef event,
                                      void *,
                                      const ::EventParamName parameterNames[],
                                      Handler handler )
           {
            return handler( EventParameter< ParameterTypes::item0 >( event, parameterNames[0] ),
                            EventParameter< ParameterTypes::item1 >( event, parameterNames[1] ),
                            EventParameter< ParameterTypes::item2 >( event, parameterNames[2] ) );
           }
        };

      template < class Result, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, void, ParameterTypes, 2 >
        {
         typedef Result (*Handler)( typename DescType_Traits< ParameterTypes::item0 >::Result,
                                    typename DescType_Traits< ParameterTypes::item1 >::Result );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef event,
                                      void *,
                                      const ::EventParamName parameterNames[],
                                      Handler handler )
           {
            return handler( EventParameter< ParameterTypes::item0 >( event, parameterNames[0] ),
                            EventParameter< ParameterTypes::item1 >( event, parameterNames[1] ) );
           }
        };

      template < class Result, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, void, ParameterTypes, 1 >
        {
         typedef Result (*Handler)( typename DescType_Traits< ParameterTypes::item0 >::Result );
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef event,
                                      void *,
                                      const ::EventParamName parameterNames[],
                                      Handler handler )
           {
            return handler( EventParameter< ParameterTypes::item0 >( event, parameterNames[0] ) );
           }
        };

      template < class Result, class ParameterTypes >
      struct EventHandler_ParameterGlue< Result, void, ParameterTypes, 0 >
        {
         typedef Result (*Handler)();
         
         static Result GetParameters( EventHandlerCallRef,
                                      EventRef,
                                      void *,
                                      const ::EventParamName[],
                                      Handler handler )
           {
            return handler();
           }
        };

      template < class Result,
                 class Object,
                 class ParameterTypes,
                 class ParameterNames,
                 typename EventHandler_ParameterGlue< Result, Object, ParameterTypes >::Handler handler >
      struct EventHandler_Bound_ParameterGlue
        {
         static Result ProvideParameterNamesAndHandler( EventHandlerCallRef call,
                                                        EventRef event,
                                                        void *userData )
           {
            return EventHandler_ParameterGlue< Result, Object, ParameterTypes >::GetParameters( call,
                                                                                                event,
                                                                                                userData,
                                                                                                ParameterNames::Array(),
                                                                                                handler );
           }
        };
      
      template < ::EventParamName resultParameter,
                 EventParamType resultType,
                 class Object,
                 class ParameterNames,
                 class ParameterTypes,
                 typename EventHandler_ParameterGlue< typename DescType_Traits< resultType >::Result, Object, ParameterTypes >::Handler handler >
      EventHandlerUPP EventHandler_ParameterGlueUPP()
        {
         typedef typename DescType_Traits< resultType >::Result Result;
         
         return EventHandler_Bound_ResultGlue<
                  resultType,
                  resultParameter,
                  EventHandler_Bound_ParameterGlue< Result,
                                                    Object,
                                                    ParameterTypes,
                                                    ParameterNames,
                                                    handler >::ProvideParameterNamesAndHandler >::UPP();
        }

   /* A traits class which looks up a list of parameter types from an event and a list of parameter names */
      
      template < UInt32 eventClass, UInt32 eventKind, class ParameterNames, int parameterCount = ParameterNames::size >
      struct EventParameter_ListType;
      
      template < UInt32 eventClass, UInt32 eventKind, class ParameterNames >
      struct EventParameter_ListType< eventClass, eventKind, ParameterNames, 8 >
        {
         typedef EventParamType_List< EventParameter_Traits< eventClass, eventKind, ParameterNames::item0 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item1 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item2 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item3 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item4 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item5 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item6 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item7 >::type > Types;
        };

      template < UInt32 eventClass, UInt32 eventKind, class ParameterNames >
      struct EventParameter_ListType< eventClass, eventKind, ParameterNames, 7 >
        {
         typedef EventParamType_List< EventParameter_Traits< eventClass, eventKind, ParameterNames::item0 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item1 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item2 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item3 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item4 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item5 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item6 >::type > Types;
        };

      template < UInt32 eventClass, UInt32 eventKind, class ParameterNames >
      struct EventParameter_ListType< eventClass, eventKind, ParameterNames, 6 >
        {
         typedef EventParamType_List< EventParameter_Traits< eventClass, eventKind, ParameterNames::item0 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item1 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item2 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item3 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item4 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item5 >::type > Types;
        };

      template < UInt32 eventClass, UInt32 eventKind, class ParameterNames >
      struct EventParameter_ListType< eventClass, eventKind, ParameterNames, 5 >
        {
         typedef EventParamType_List< EventParameter_Traits< eventClass, eventKind, ParameterNames::item0 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item1 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item2 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item3 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item4 >::type > Types;
        };

      template < UInt32 eventClass, UInt32 eventKind, class ParameterNames >
      struct EventParameter_ListType< eventClass, eventKind, ParameterNames, 4 >
        {
         typedef EventParamType_List< EventParameter_Traits< eventClass, eventKind, ParameterNames::item0 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item1 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item2 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item3 >::type > Types;
        };

      template < UInt32 eventClass, UInt32 eventKind, class ParameterNames >
      struct EventParameter_ListType< eventClass, eventKind, ParameterNames, 3 >
        {
         typedef EventParamType_List< EventParameter_Traits< eventClass, eventKind, ParameterNames::item0 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item1 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item2 >::type > Types;
        };

      template < UInt32 eventClass, UInt32 eventKind, class ParameterNames >
      struct EventParameter_ListType< eventClass, eventKind, ParameterNames, 2 >
        {
         typedef EventParamType_List< EventParameter_Traits< eventClass, eventKind, ParameterNames::item0 >::type,
                                      EventParameter_Traits< eventClass, eventKind, ParameterNames::item1 >::type > Types;
        };

      template < UInt32 eventClass, UInt32 eventKind, class ParameterNames >
      struct EventParameter_ListType< eventClass, eventKind, ParameterNames, 1 >
        {
         typedef EventParamType_List< EventParameter_Traits< eventClass, eventKind, ParameterNames::item0 >::type > Types;
        };

      template < UInt32 eventClass, UInt32 eventKind, class ParameterNames >
      struct EventParameter_ListType< eventClass, eventKind, ParameterNames, 0 >
        {
         typedef EventParamType_List<> Types;
        };

   /* All the glue, looked up according to the event we're handling */
   
      template < UInt32 eventClass,
                 UInt32 eventKind,
                 ::EventParamName resultParameter,
                 class Object,
                 class ParameterNames >
      struct EventHandler_EventSpecificGlue
        {
         static const EventParamType resultType = EventParameter_Traits< eventClass, eventKind, resultParameter >::type;
         
         typedef typename DescType_Traits< resultType >::Result Result;
         
         typedef typename EventParameter_ListType< eventClass, eventKind, ParameterNames >::Types ParameterTypes;
         
         typedef typename EventHandler_ParameterGlue< Result, Object, ParameterTypes >::Handler Handler;
         
         template < Handler handler >
         static EventHandlerUPP UPP()
           {
            return EventHandler_ParameterGlueUPP< resultParameter,
                                                  resultType,
                                                  Object,
                                                  ParameterNames,
                                                  ParameterTypes,
                                                  handler >();
           }
        };
      
   /* Level 3 (Untyped interface decipherment) InstallEventHandler */
   
      template < UInt32 eventClass,
                 UInt32 eventKind,
                 ::EventParamName resultParameter,
                 class Object,
                 class ParameterNames,
                 typename EventHandler_EventSpecificGlue< eventClass,
                                                          eventKind,
                                                          resultParameter,
                                                          Object,
                                                          ParameterNames >::Handler handler >
      Nucleus::Owned<EventHandlerRef> InstallEventHandler( EventTargetRef                                 inTarget,
                                                  typename Nucleus::ObjectParameterTraits<Object>::Type   inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         typedef EventHandler_EventSpecificGlue< eventClass, eventKind, resultParameter, Object, ParameterNames > Glue;

		 EventHandlerUPP upp = Glue::template UPP< handler >();

         const void *userData = Nucleus::ObjectParameterTraits<Object>::ConvertToPointer( inUserData );
         
         return InstallEventHandler( inTarget,
                                     upp,
                                     EventClass( eventClass ),
                                     CarbonEventKind( eventKind ),
                                     userData );
        }
      
      inline Nucleus::Owned<EventHandlerRef> InstallApplicationEventHandler( EventHandlerUPP        inHandler,
                                                                    UInt32                 inNumTypes,
                                                                    const EventTypeSpec *  inList,
                                                                    const void *           inUserData = 0 )
        {
         return InstallEventHandler( GetApplicationEventTarget(), inHandler, inNumTypes, inList, inUserData );
        }
      
      inline Nucleus::Owned<EventHandlerRef> InstallApplicationEventHandler( EventHandlerUPP        inHandler,
                                                                    EventClass             eventClass,
                                                                    CarbonEventKind        eventKind,
                                                                    const void *           inUserData = 0 )
        {
         return InstallEventHandler( GetApplicationEventTarget(), inHandler, eventClass, eventKind, inUserData );
        }

      template < EventHandlerProcPtr handler >
      Nucleus::Owned<EventHandlerRef> InstallApplicationEventHandler( UInt32                 inNumTypes,
                                                             const EventTypeSpec *  inList,
                                                             const void *           inUserData = 0 )
        {
         return InstallEventHandler< handler >( GetApplicationEventTarget(), inNumTypes, inList, inUserData );
        }

      template < EventHandlerProcPtr handler >
      Nucleus::Owned<EventHandlerRef> InstallApplicationEventHandler( EventClass             eventClass,
                                                             CarbonEventKind        eventKind,
                                                             const void *           inUserData = 0 )
        {
         return InstallEventHandler< handler >( GetApplicationEventTarget(), eventClass, eventKind, inUserData );
        }

      template < class Object, typename EventHandler_ObjectGlue<Object>::Handler handler >
      Nucleus::Owned<EventHandlerRef> InstallApplicationEventHandler( UInt32                                         inNumTypes,
                                                             const EventTypeSpec *                          inList,
                                                             typename Nucleus::ObjectParameterTraits<Object>::Type   inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler< Object, handler >( GetApplicationEventTarget(), inNumTypes, inList, inUserData );
        }

      template < class Object, typename EventHandler_ObjectGlue<Object>::Handler handler >
      Nucleus::Owned<EventHandlerRef> InstallApplicationEventHandler( EventClass                                     eventClass,
                                                             CarbonEventKind                                eventKind,
                                                             typename Nucleus::ObjectParameterTraits<Object>::Type   inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler< Object, handler >( GetApplicationEventTarget(), eventClass, eventKind, inUserData );
        }

      template < UInt32 eventClass,
                 UInt32 eventKind,
                 ::EventParamName resultParameter,
                 class Object,
                 class ParameterNames,
                 typename EventHandler_EventSpecificGlue< eventClass,
                                                          eventKind,
                                                          resultParameter,
                                                          Object,
                                                          ParameterNames >::Handler handler >
      Nucleus::Owned<EventHandlerRef> InstallApplicationEventHandler( typename Nucleus::ObjectParameterTraits<Object>::Type inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler< eventClass, eventKind, resultParameter, Object, ParameterNames, handler >( GetApplicationEventTarget(), inUserData );
        }

   /* InstallWindowEventHandler */

      inline Nucleus::Owned<EventHandlerRef> InstallWindowEventHandler( WindowRef              window,
                                                               EventHandlerUPP        inHandler,
                                                               UInt32                 inNumTypes,
                                                               const EventTypeSpec *  inList,
                                                               const void *           inUserData = 0 )
        {
         return InstallEventHandler( GetWindowEventTarget( window ), inHandler, inNumTypes, inList, inUserData );
        }
      
      inline Nucleus::Owned<EventHandlerRef> InstallWindowEventHandler( WindowRef              window,
                                                               EventHandlerUPP        inHandler,
                                                               EventClass             eventClass,
                                                               CarbonEventKind        eventKind,
                                                               const void *           inUserData = 0 )
        {
         return InstallEventHandler( GetWindowEventTarget( window ), inHandler, eventClass, eventKind, inUserData );
        }

      template < EventHandlerProcPtr handler >
      Nucleus::Owned<EventHandlerRef> InstallWindowEventHandler( WindowRef              window,
                                                        UInt32                 inNumTypes,
                                                        const EventTypeSpec *  inList,
                                                        const void *           inUserData = 0 )
        {
         return InstallEventHandler< handler >( GetWindowEventTarget( window ), inNumTypes, inList, inUserData );
        }

      template < EventHandlerProcPtr handler >
      Nucleus::Owned<EventHandlerRef> InstallWindowEventHandler( WindowRef              window,
                                                        EventClass             eventClass,
                                                        CarbonEventKind        eventKind,
                                                        const void *           inUserData = 0 )
        {
         return InstallEventHandler< handler >( GetWindowEventTarget( window ), eventClass, eventKind, inUserData );
        }

      template < class Object, typename EventHandler_ObjectGlue<Object>::Handler handler >
      Nucleus::Owned<EventHandlerRef> InstallWindowEventHandler( WindowRef                                      window,
                                                        UInt32                                         inNumTypes,
                                                        const EventTypeSpec *                          inList,
                                                        typename Nucleus::ObjectParameterTraits<Object>::Type   inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler< Object, handler >( GetWindowEventTarget( window ), inNumTypes, inList, inUserData );
        }

      template < class Object, typename EventHandler_ObjectGlue<Object>::Handler handler >
      Nucleus::Owned<EventHandlerRef> InstallWindowEventHandler( WindowRef                                      window,
                                                        EventClass                                     eventClass,
                                                        CarbonEventKind                                eventKind,
                                                        typename Nucleus::ObjectParameterTraits<Object>::Type   inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler< Object, handler >( GetWindowEventTarget( window ), eventClass, eventKind, inUserData );
        }

      template < UInt32 eventClass,
                 UInt32 eventKind,
                 ::EventParamName resultParameter,
                 class Object,
                 class ParameterNames,
                 typename EventHandler_EventSpecificGlue< eventClass,
                                                          eventKind,
                                                          resultParameter,
                                                          Object,
                                                          ParameterNames >::Handler handler >
      Nucleus::Owned<EventHandlerRef> InstallWindowEventHandler( WindowRef                                    window,
                                                        typename Nucleus::ObjectParameterTraits<Object>::Type inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler< eventClass, eventKind, resultParameter, Object, ParameterNames, handler >( GetWindowEventTarget( window ), inUserData );
        }

   /* InstallControlEventHandler */

      inline Nucleus::Owned<EventHandlerRef> InstallControlEventHandler( ControlRef             control,
                                                                EventHandlerUPP        inHandler,
                                                                UInt32                 inNumTypes,
                                                                const EventTypeSpec *  inList,
                                                                const void *           inUserData = 0 )
        {
         return InstallEventHandler( GetControlEventTarget( control ), inHandler, inNumTypes, inList, inUserData );
        }
      
      inline Nucleus::Owned<EventHandlerRef> InstallControlEventHandler( ControlRef             control,
                                                                EventHandlerUPP        inHandler,
                                                                EventClass             eventClass,
                                                                CarbonEventKind        eventKind,
                                                                const void *           inUserData = 0 )
        {
         return InstallEventHandler( GetControlEventTarget( control ), inHandler, eventClass, eventKind, inUserData );
        }

      template < EventHandlerProcPtr handler >
      Nucleus::Owned<EventHandlerRef> InstallControlEventHandler( ControlRef             control,
                                                         UInt32                 inNumTypes,
                                                         const EventTypeSpec *  inList,
                                                         const void *           inUserData = 0 )
        {
         return InstallEventHandler< handler >( GetControlEventTarget( control ), inNumTypes, inList, inUserData );
        }

      template < EventHandlerProcPtr handler >
      Nucleus::Owned<EventHandlerRef> InstallControlEventHandler( ControlRef             control,
                                                         EventClass             eventClass,
                                                         CarbonEventKind        eventKind,
                                                         const void *           inUserData = 0 )
        {
         return InstallEventHandler< handler >( GetControlEventTarget( control ), eventClass, eventKind, inUserData );
        }

      template < class Object, typename EventHandler_ObjectGlue<Object>::Handler handler >
      Nucleus::Owned<EventHandlerRef> InstallControlEventHandler( ControlRef                                     control,
                                                         UInt32                                         inNumTypes,
                                                         const EventTypeSpec *                          inList,
                                                         typename Nucleus::ObjectParameterTraits<Object>::Type   inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler< Object, handler >( GetControlEventTarget( control ), inNumTypes, inList, inUserData );
        }

      template < class Object, typename EventHandler_ObjectGlue<Object>::Handler handler >
      Nucleus::Owned<EventHandlerRef> InstallControlEventHandler( ControlRef                                     control,
                                                         EventClass                                     eventClass,
                                                         CarbonEventKind                                eventKind,
                                                         typename Nucleus::ObjectParameterTraits<Object>::Type   inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler< Object, handler >( GetControlEventTarget( control ), eventClass, eventKind, inUserData );
        }

      template < UInt32 eventClass,
                 UInt32 eventKind,
                 ::EventParamName resultParameter,
                 class Object,
                 class ParameterNames,
                 typename EventHandler_EventSpecificGlue< eventClass,
                                                          eventKind,
                                                          resultParameter,
                                                          Object,
                                                          ParameterNames >::Handler handler >
      Nucleus::Owned<EventHandlerRef> InstallControlEventHandler( ControlRef                                   control,
                                                         typename Nucleus::ObjectParameterTraits<Object>::Type inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler< eventClass, eventKind, resultParameter, Object, ParameterNames, handler >( GetControlEventTarget( control ), inUserData );
        }

   /* InstallMenuEventHandler */

      inline Nucleus::Owned<EventHandlerRef> InstallMenuEventHandler( MenuRef                menu,
                                                             EventHandlerUPP        inHandler,
                                                             UInt32                 inNumTypes,
                                                             const EventTypeSpec *  inList,
                                                             const void *           inUserData = 0 )
        {
         return InstallEventHandler( GetMenuEventTarget( menu ), inHandler, inNumTypes, inList, inUserData );
        }
      
      inline Nucleus::Owned<EventHandlerRef> InstallMenuEventHandler( MenuRef                menu,
                                                             EventHandlerUPP        inHandler,
                                                             EventClass             eventClass,
                                                             CarbonEventKind        eventKind,
                                                             const void *           inUserData = 0 )
        {
         return InstallEventHandler( GetMenuEventTarget( menu ), inHandler, eventClass, eventKind, inUserData );
        }

      template < EventHandlerProcPtr handler >
      Nucleus::Owned<EventHandlerRef> InstallMenuEventHandler( MenuRef                menu,
                                                      UInt32                 inNumTypes,
                                                      const EventTypeSpec *  inList,
                                                      const void *           inUserData = 0 )
        {
         return InstallEventHandler< handler >( GetMenuEventTarget( menu ), inNumTypes, inList, inUserData );
        }

      template < EventHandlerProcPtr handler >
      Nucleus::Owned<EventHandlerRef> InstallMenuEventHandler( MenuRef                menu,
                                                      EventClass             eventClass,
                                                      CarbonEventKind        eventKind,
                                                      const void *           inUserData = 0 )
        {
         return InstallEventHandler< handler >( GetMenuEventTarget( menu ), eventClass, eventKind, inUserData );
        }

      template < class Object, typename EventHandler_ObjectGlue<Object>::Handler handler >
      Nucleus::Owned<EventHandlerRef> InstallMenuEventHandler( MenuRef                                        menu,
                                                      UInt32                                         inNumTypes,
                                                      const EventTypeSpec *                          inList,
                                                      typename Nucleus::ObjectParameterTraits<Object>::Type   inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler< Object, handler >( GetMenuEventTarget( menu ), inNumTypes, inList, inUserData );
        }

      template < class Object, typename EventHandler_ObjectGlue<Object>::Handler handler >
      Nucleus::Owned<EventHandlerRef> InstallMenuEventHandler( MenuRef                                        menu,
                                                      EventClass                                     eventClass,
                                                      CarbonEventKind                                eventKind,
                                                      typename Nucleus::ObjectParameterTraits<Object>::Type   inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler< Object, handler >( GetMenuEventTarget( menu ), eventClass, eventKind, inUserData );
        }

      template < UInt32 eventClass,
                 UInt32 eventKind,
                 ::EventParamName resultParameter,
                 class Object,
                 class ParameterNames,
                 typename EventHandler_EventSpecificGlue< eventClass,
                                                          eventKind,
                                                          resultParameter,
                                                          Object,
                                                          ParameterNames >::Handler handler >
      Nucleus::Owned<EventHandlerRef> InstallMenuEventHandler( MenuRef                                      menu,
                                                      typename Nucleus::ObjectParameterTraits<Object>::Type inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler< eventClass, eventKind, resultParameter, Object, ParameterNames, handler >( GetMenuEventTarget( menu ), inUserData );
        }


   /* HIViewInstallEventHandler */

#if TARGET_RT_MAC_MACHO

      inline Nucleus::Owned<EventHandlerRef> HIViewInstallEventHandler( HIViewRef   view,
                                                             EventHandlerUPP        inHandler,
                                                             UInt32                 inNumTypes,
                                                             const EventTypeSpec *  inList,
                                                             const void *           inUserData = 0 )
        {
         return InstallEventHandler( HIObjectGetEventTarget((HIObjectRef) view ), inHandler, inNumTypes, inList, inUserData );
        }
      
      inline Nucleus::Owned<EventHandlerRef> HIViewInstallEventHandler( HIViewRef     view,
                                                             EventHandlerUPP        inHandler,
                                                             EventClass             eventClass,
                                                             CarbonEventKind        eventKind,
                                                             const void *           inUserData = 0 )
        {
         return InstallEventHandler( HIObjectGetEventTarget((HIObjectRef) view ), inHandler, eventClass, eventKind, inUserData );
        }

      template < EventHandlerProcPtr handler >
      Nucleus::Owned<EventHandlerRef> HIViewInstallEventHandler( HIViewRef   view,
                                                      UInt32                 inNumTypes,
                                                      const EventTypeSpec *  inList,
                                                      const void *           inUserData = 0 )
        {
         return InstallEventHandler< handler >( HIObjectGetEventTarget((HIObjectRef) view ), inNumTypes, inList, inUserData );
        }

      template < EventHandlerProcPtr handler >
      Nucleus::Owned<EventHandlerRef> HIViewInstallEventHandler( HIViewRef   view,
                                                      EventClass             eventClass,
                                                      CarbonEventKind        eventKind,
                                                      const void *           inUserData = 0 )
        {
         return InstallEventHandler< handler >( HIObjectGetEventTarget((HIObjectRef) view ), eventClass, eventKind, inUserData );
        }

      template < class Object, typename EventHandler_ObjectGlue<Object>::Handler handler >
      Nucleus::Owned<EventHandlerRef> HIViewInstallEventHandler( HIViewRef                           view,
                                                      UInt32                                         inNumTypes,
                                                      const EventTypeSpec *                          inList,
                                                      typename Nucleus::ObjectParameterTraits<Object>::Type   inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler< Object, handler >( HIObjectGetEventTarget((HIObjectRef) view ), inNumTypes, inList, inUserData );
        }

      template < class Object, typename EventHandler_ObjectGlue<Object>::Handler handler >
      Nucleus::Owned<EventHandlerRef> HIViewInstallEventHandler( HIViewRef                           view,
                                                      EventClass                                     eventClass,
                                                      CarbonEventKind                                eventKind,
                                                      typename Nucleus::ObjectParameterTraits<Object>::Type   inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler< Object, handler >( HIObjectGetEventTarget((HIObjectRef) view ), eventClass, eventKind, inUserData );
        }

      template < UInt32 eventClass,
                 UInt32 eventKind,
                 ::EventParamName resultParameter,
                 class Object,
                 class ParameterNames,
                 typename EventHandler_EventSpecificGlue< eventClass,
                                                          eventKind,
                                                          resultParameter,
                                                          Object,
                                                          ParameterNames >::Handler handler >
      Nucleus::Owned<EventHandlerRef> HIViewInstallEventHandler( HIViewRef                                  view,
                                                      typename Nucleus::ObjectParameterTraits<Object>::Type inUserData = typename Nucleus::ObjectParameterTraits<Object>::Type() )
        {
         return InstallEventHandler< eventClass, eventKind, resultParameter, Object, ParameterNames, handler >( HIObjectGetEventTarget((HIObjectRef) view ), inUserData );
        }

#endif

   void InstallStandardEventHandler( EventTargetRef inTarget );

   void RemoveEventHandler( Nucleus::Owned<EventHandlerRef> inHandlerRef );

   /* ... */
   
   void SendEventToEventTarget( EventRef inEvent, EventTargetRef inTarget );
   
   /* ... */
   
   void ProcessHICommand( const HICommand& inCommand );
   
   using ::RunApplicationEventLoop;
   
   /* ... */

   using ::EventHotKeyRef;
   using ::EventHotKeyID;
   
  }

namespace Nucleus
  {
   template <>
   struct Disposer< Nitrogen::EventHotKeyRef >: public std::unary_function< Nitrogen::EventHotKeyRef, void >
     {
      void operator()( Nitrogen::EventHotKeyRef toDispose ) const
        {
         ::UnregisterEventHotKey( toDispose );
        }
     };
  }

namespace Nitrogen
  {

   Nucleus::Owned< EventHotKeyRef > RegisterEventHotKey( UInt32            inHotKeyCode,
                                                UInt32            inHotKeyModifiers,
                                                EventHotKeyID     inHotKeyID,
                                                EventTargetRef    inTarget,
                                                ::OptionBits      inOptions = 0 );


   void UnregisterEventHotKey( Nucleus::Owned< EventHotKeyRef > );
  }

#endif
