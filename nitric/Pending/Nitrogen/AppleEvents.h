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
#ifndef NITROGEN_OBJECTPARAMETERTRAITS_H
#include "Nitrogen/ObjectParameterTraits.h"
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
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
	
	class AEEventSource_Tag {};
	typedef SelectorType< AEEventSource_Tag, ::AEEventSource, kAEUnknownSource > AEEventSource;
	
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
	
   Owned< AEEventHandler >
   AEInstallEventHandler( const AEEventHandler& );
   
   inline Owned< AEEventHandler >
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
   inline Owned< AEEventHandler >
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
	inline Owned< AEEventHandler >
	AEInstallEventHandler( AEEventClass                                    theAEEventClass,
	                       AEEventID                                       theAEEventID,
	                       typename ObjectParameterTraits< Object >::Type  handlerRefCon   = typename ObjectParameterTraits< Object >::Type(),
	                       Boolean                                         isSysHandler    = false )
	{
		return AEInstallEventHandler< AEEventHandler_Callback< Object, handler >::Adapter >
		(
			theAEEventClass, 
			theAEEventID, 
			ObjectParameterTraits< Object >::ConvertToPointer( handlerRefCon ), 
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
		typedef typename ObjectParameterTraits< Object >::Type ObjectType;
		
		return AEInstallEventHandler< AEEventHandler_Callback< void, handler >::Adapter >
		(
			theAEEventClass,
			theAEEventID,
			ObjectParameterTraits< Object >::ConvertToPointer( ObjectType() ),
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
		return AEInstallEventHandler< void, handler >( theAEEventClass,
		                                               theAEEventID,
		                                               isSysHandler );
	}
	
   void AERemoveEventHandler( Owned<AEEventHandler> );

   typedef AEEventHandler AEGetEventHandler_Result;
   
   AEEventHandler AEGetEventHandler( AEEventClass  theAEEventClass,
                                     AEEventID     theAEEventID,
                                     bool          isSysHandler = false );
}

#endif

