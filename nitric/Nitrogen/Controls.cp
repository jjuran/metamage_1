// Controls.cp

#ifndef NITROGEN_CONTROLS_H
#include "Nitrogen/Controls.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif

namespace Nitrogen
  {
   ControlRef GetControlByID( WindowRef inWindow, const ControlID& id )
     {
      OnlyOnce< RegisterControlManagerErrors >();
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

   void SetControlData( ControlRef        inControl,
                        ControlPartCode   inPart,
                        ResType           inTagName,
                        Size              inSize,
                        const void *      inData )
     {
      OnlyOnce< RegisterControlManagerErrors >();
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
      OnlyOnce< RegisterControlManagerErrors >();
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
