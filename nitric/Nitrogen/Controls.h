// Controls.h

#ifndef NITROGEN_CONTROLS_H
#define NITROGEN_CONTROLS_H

#ifndef __CONTROLS__
#include <Controls.h>
#endif
#ifndef NITROGEN_MACWINDOWS_H
#include "Nitrogen/MacWindows.h"
#endif
#ifndef NITROGEN_AEDATAMODEL_H
#include "Nitrogen/AEDataModel.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_MAKE_H
#include "Nitrogen/Make.h"
#endif

#ifdef DrawOneControl
   #undef DrawOneControl
   inline void DrawOneControl( ControlRef theControl )
     {
      Draw1Control( theControl );
     }
#endif

namespace Nitrogen
  {
   using ::ControlRef;
   
   template <> struct Disposer< ControlRef >: public std::unary_function< ControlRef, void >
     {
      void operator()( ControlRef c ) const
        {
         ::DisposeControl( c );
        }
     };

   using ::DrawOneControl;

   using ::ControlID;
   
   template <>
   struct Maker< ControlID >
     {
      ControlID operator()( OSType signature, SInt32 id ) const
        {
         ControlID result;
         result.signature = signature;
         result.id = id;
         return result;
        }
     };   
   
   ControlRef GetControlByID( WindowRef inWindow, const ControlID& id );
   ControlRef GetControlByID( WindowRef inWindow, OSType signature, SInt32 id );  // To be removed; use Make.

   template < ::ResType inTagName > struct ControlData_Traits;

   typedef UPP< ::ControlKeyFilterUPP,
                ::ControlKeyFilterProcPtr,
                ::NewControlKeyFilterUPP,
                ::DisposeControlKeyFilterUPP,
                ::InvokeControlKeyFilterUPP > ControlKeyFilterUPP;
   
   template<> struct ControlData_Traits< kControlFontStyleTag >: POD_DescType_Traits< ControlFontStyleRec > {};
   template<> struct ControlData_Traits< kControlKeyFilterTag >: POD_DescType_Traits< ControlKeyFilterUPP > {};
   template<> struct ControlData_Traits< kControlKindTag      >: POD_DescType_Traits< ControlKind         > {};
   template<> struct ControlData_Traits< kControlSizeTag      >: POD_DescType_Traits< ControlSize         > {};


   template < ::ResType inTagName >
   struct SetControlData_Traits: public ControlData_Traits<inTagName>
     {
      typedef typename ControlData_Traits<inTagName>::Parameter InData_Type;
     };

   template < ::ResType inTagName >
   struct GetControlData_Traits: public ControlData_Traits<inTagName>
     {
     };
   
   
   void SetControlData( ControlRef        inControl,
                        ControlPartCode   inPart,
                        ResType           inTagName,
                        Size              inSize,
                        const void *      inData );

   template < ::ResType inTagName >
   void SetControlData( ControlRef                                               inControl,
                        ControlPartCode                                          inPart,
                        typename SetControlData_Traits< inTagName >::InData_Type inData )
     {
      typedef SetControlData_Traits< inTagName > Traits;
      
      typename Traits::OutputBuffer buffer = Traits::PrepareOutputBuffer( inData );
      
      Nitrogen::SetControlData( inControl,
                              inPart,
                              inTagName,
                              Traits::OutputBufferLength( buffer ),
                              Traits::OutputBufferStart( buffer ) );
      
      Traits::ReleaseOutputBuffer( buffer );
     }

   template < ::ResType inTagName >
   void SetControlData( ControlRef                                               inControl,
                        typename SetControlData_Traits< inTagName >::InData_Type inData )
     {
      return SetControlData< inTagName >( inControl, kControlEntireControl, inData );
     }

   Size GetControlData( ControlRef        inControl,
                        ControlPartCode   inPart,
                        ResType           inTagName,
                        Size              inBufferSize,
                        void *            inBuffer );

   Size GetControlData( ControlRef        inControl,
                        ControlPartCode   inPart,
                        ResType           inTagName );
   
   template < ::ResType inTagName >
   typename GetControlData_Traits<inTagName>::Result
   GetControlData( ControlRef        inControl,
                   ControlPartCode   inPart = kControlEntireControl )
     {
      typedef GetControlData_Traits< inTagName > Traits;
      
      typename Traits::InputBuffer buffer;
      
      if ( Traits::inputHasVariableLength )
         Traits::SetInputBufferLength( buffer, GetControlData( inControl, inPart, inTagName ) );
      
      GetControlData( inControl,
                      inPart,
                      inTagName,
                      Traits::InputBufferLength( buffer ),
                      Traits::InputBufferStart( buffer ) );
      
      return Traits::ProcessInputBuffer( buffer );
     }
   
   /* ... */
   
   bool HandleControlDragTracking( ControlRef          inControl,
                                   DragTrackingMessage inMessage,
                                   DragReference       inDrag );

   void HandleControlDragReceive( ControlRef    inControl,
                                  DragReference inDrag );

   void SetControlDragTrackingEnabled( ControlRef theControl,
                                       bool       tracks );

   bool IsControlDragTrackingEnabled( ControlRef theControl );

   void SetAutomaticControlDragTrackingEnabledForWindow( WindowRef theWindow,
                                                         bool      tracks );

   bool IsAutomaticControlDragTrackingEnabledForWindow( WindowRef theWindow );

   /* ... */
   
   void RegisterControlManagerErrors();
  }

#endif
