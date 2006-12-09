// Controls.cp

#ifndef NITROGEN_CONTROLS_H
#include "Nitrogen/Controls.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NUCLEUS_ONLYONCE_H
#include "Nucleus/OnlyOnce.h"
#endif

namespace Nitrogen
  {
  
	ControlRef NewControl( WindowRef         owningWindow,
	                       const Rect&       boundsRect, 
	                       ConstStr255Param  controlTitle,
	                       bool              initiallyVisible,
	                       short             initialValue,
	                       short             minimumValue,
	                       short             maximumValue,
	                       ControlProcID     procID,
	                       RefCon            refCon )
	{
		return ::NewControl( owningWindow,
		                     &boundsRect,
		                     controlTitle,
		                     initiallyVisible,
		                     initialValue,
		                     minimumValue,
		                     maximumValue,
		                     procID,
		                     refCon );
	}
	
	void HiliteControl( ControlRef control, ControlPartCode hiliteState )
	{
		::HiliteControl( control, hiliteState );
	}
	
	ControlPartCode TrackControl( ControlRef        theControl,
	                              Point             startPoint,
	                              ControlActionUPP  actionProc )
	{
		return ControlPartCode( ::TrackControl( theControl,
		                                        startPoint,
		                                        actionProc ) );
	}
	
	FindControl_Result FindControl( Point testPoint, WindowRef theWindow )
	{
		FindControl_Result result;
		result.part = ControlPartCode( ::FindControl( testPoint,
		                                              theWindow,
		                                              &result.control ) );
		
		return result;
	}
	
	void SetControlTitle( ControlRef control, std::string title )
	{
		::SetControlTitle( control, Str255( title ) );
	}
	
   ControlRef GetControlByID( WindowRef inWindow, const ControlID& id )
     {
      Nucleus::OnlyOnce< RegisterControlManagerErrors >();
      ControlRef result;
      ThrowOSStatus( ::GetControlByID( inWindow, &id, &result ) );
      return result;
     }

   ControlRef GetControlByID( WindowRef inWindow, OSType signature, SInt32 id )
     {
      ControlID controlID;
      controlID.signature = signature;
      controlID.id = id;
      return GetControlByID( inWindow, controlID );
     }

	void SetControlAction( ControlRef control, ControlActionUPP actionProc )
	{
		::SetControlAction( control, actionProc );
	}
	
	void SetControlReference( ControlRef control, RefCon data )
	{
		::SetControlReference( control, data );
	}
	
	RefCon GetControlReference( ControlRef control )
	{
		return ::GetControlReference( control );
	}
	
   void SetControlData( ControlRef        inControl,
                        ControlPartCode   inPart,
                        ResType           inTagName,
                        Size              inSize,
                        const void *      inData )
     {
      Nucleus::OnlyOnce< RegisterControlManagerErrors >();
      ThrowOSStatus( ::SetControlData( inControl,
                                       inPart,
                                       inTagName,
                                       static_cast< ::Size >( inSize ),
                                       inData ) );
     }

   Size GetControlData( ControlRef        inControl,
                        ControlPartCode   inPart,
                        ResType           inTagName,
                        Size              inBufferSize,
                        void *            inBuffer )
     {
      Nucleus::OnlyOnce< RegisterControlManagerErrors >();
      ::Size outActualSize;
      ThrowOSStatus( ::GetControlData( inControl,
                                       inPart,
                                       inTagName,
                                       static_cast< ::Size >( inBufferSize ),
                                       inBuffer,
                                       &outActualSize ) );
      return static_cast<Size>( outActualSize );
     }

   Size GetControlData( ControlRef        inControl,
                        ControlPartCode   inPart,
                        ResType           inTagName )
     {
      return GetControlData( inControl, inPart, inTagName, 0, 0 );
     }
   
   bool HandleControlDragTracking( ControlRef          inControl,
                                   DragTrackingMessage inMessage,
                                   DragReference       inDrag )
     {
      Nucleus::OnlyOnce< RegisterControlManagerErrors >();
      ::Boolean outLikesDrag;
      ThrowOSStatus( ::HandleControlDragTracking( inControl, inMessage, inDrag, &outLikesDrag ) );
      return outLikesDrag;
     }

   void HandleControlDragReceive( ControlRef    inControl,
                                  DragReference inDrag )
     {
      Nucleus::OnlyOnce< RegisterControlManagerErrors >();
      ThrowOSStatus( ::HandleControlDragReceive( inControl, inDrag ) );
     }

   void SetControlDragTrackingEnabled( ControlRef theControl,
                                       bool       tracks )
     {
      Nucleus::OnlyOnce< RegisterControlManagerErrors >();
      ThrowOSStatus( ::SetControlDragTrackingEnabled( theControl, tracks ) );
     }

   bool IsControlDragTrackingEnabled( ControlRef theControl )
     {
      Nucleus::OnlyOnce< RegisterControlManagerErrors >();
      ::Boolean tracks;
      ThrowOSStatus( ::IsControlDragTrackingEnabled( theControl, &tracks ) );
      return tracks;
     }
   
   void SetAutomaticControlDragTrackingEnabledForWindow( WindowRef theWindow,
                                                         bool      tracks )
     {
      Nucleus::OnlyOnce< RegisterControlManagerErrors >();
      ThrowOSStatus( ::SetAutomaticControlDragTrackingEnabledForWindow( theWindow, tracks ) );
     }
  
   bool IsAutomaticControlDragTrackingEnabledForWindow( WindowRef theWindow )
     {
      Nucleus::OnlyOnce< RegisterControlManagerErrors >();
      ::Boolean tracks;
      ThrowOSStatus( ::IsAutomaticControlDragTrackingEnabledForWindow( theWindow, &tracks ) );
      return tracks;
     }

	Rect GetControlBounds( ControlRef control )
	{
	#if OPAQUE_TOOLBOX_STRUCTS
		
		Rect bounds;
		return *( ::GetControlBounds( control, &bounds ) );
		
	#else
		
		return (**control).contrlRect;
		
	#endif
	}
	
	void SetControlBounds( ControlRef control, const Rect& bounds )
	{
	#if OPAQUE_TOOLBOX_STRUCTS
		
		::SetControlBounds( control, &bounds );
		
	#else
		
		(**control).contrlRect = bounds;
		
	#endif
	}
	
   void RegisterControlManagerErrors()
     {
      RegisterOSStatus< memFullErr                   >();
      RegisterOSStatus< errMessageNotSupported       >();
      RegisterOSStatus< errDataNotSupported          >();
      RegisterOSStatus< errControlDoesntSupportFocus >();
      RegisterOSStatus< errUnknownControl            >();
      RegisterOSStatus< errCouldntSetFocus           >();
      RegisterOSStatus< errNoRootControl             >();
      RegisterOSStatus< errRootAlreadyExists         >();
      RegisterOSStatus< errInvalidPartCode           >();
      RegisterOSStatus< errControlsAlreadyExist      >();
      RegisterOSStatus< errControlIsNotEmbedder      >();
      RegisterOSStatus< errDataSizeMismatch          >();
      RegisterOSStatus< errControlHiddenOrDisabled   >();
      RegisterOSStatus< errCantEmbedIntoSelf         >();
      RegisterOSStatus< errCantEmbedRoot             >();
      RegisterOSStatus< errItemNotControl            >();
      RegisterOSStatus< controlInvalidDataVersionErr >();
      RegisterOSStatus< controlPropertyInvalid       >();
      RegisterOSStatus< controlPropertyNotFoundErr   >();
      RegisterOSStatus< controlHandleInvalidErr      >();
     }
  }
