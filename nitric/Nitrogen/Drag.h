// Drag.h

#ifndef NITROGEN_DRAG_H
#define NITROGEN_DRAG_H

#ifndef __DRAG__
#include <Drag.h>
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_EVENTS_H
#include "Nitrogen/Events.h"
#endif
#ifndef NITROGEN_FILES_H
#include "Nitrogen/Files.h"
#endif
#ifndef NITROGEN_APPLEEVENTS_H
#include "Nitrogen/AppleEvents.h"
#endif
#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/Quickdraw.h"
#endif
#ifndef NITROGEN_IDTYPE_H
#include "Nitrogen/IDType.h"
#endif
#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
#endif
#ifndef NITROGEN_SELECTORTYPE_H
#include "Nitrogen/SelectorType.h"
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

namespace Nitrogen
  {
   void RegisterDragManagerErrors();
   
   using ::DragRef;

   class DragItemRefTag {};
   typedef IDType< DragItemRefTag, ::DragItemRef, 0 > DragItemRef;
   
   class FlavorTypeTag {};
   typedef SelectorType< FlavorTypeTag, ::FlavorType, 0 > FlavorType;
   
   class DragAttributesTag {};
   typedef FlagType< DragAttributesTag, ::DragAttributes, 0 > DragAttributes;
   
   class DragBehaviorsTag {};
   typedef FlagType< DragBehaviorsTag, ::DragBehaviors, 0 > DragBehaviors;
   
   class DragImageFlagsTag {};
   typedef FlagType< DragImageFlagsTag, ::DragImageFlags, 0 > DragImageFlags;
    
   class DragRegionMessageTag {};
   typedef SelectorType< DragRegionMessageTag, ::DragRegionMessage, 0 > DragRegionMessage;
    
   class ZoomAccelerationTag {};
   typedef SelectorType< ZoomAccelerationTag, ::ZoomAcceleration, 0 > ZoomAcceleration;
   
   class FlavorFlagsTag {};
   typedef FlagType< FlavorFlagsTag, ::FlavorFlags, 0 > FlavorFlags;
    
   class DragTrackingMessageTag {};
   typedef SelectorType< DragTrackingMessageTag, ::DragTrackingMessage, 0 > DragTrackingMessage;
   
   class DragActionsTag {};
   typedef FlagType< DragActionsTag, ::DragActions, 0 > DragActions;
   
   using ::HFSFlavor;
   using ::PromiseHFSFlavor;
   
   /* UPPs... */
   /* Handlers... */

   template <> struct Disposer< DragRef >: public std::unary_function< DragRef, void >,
                                           private DefaultDestructionOSStatusPolicy
     {
      void operator()( DragRef i ) const
        {
         OnlyOnce<RegisterDragManagerErrors>();
         DefaultDestructionOSStatusPolicy::HandleDestructionOSStatus( ::DisposeDrag( i ) );
        }
     };
   
   Owned<DragRef> NewDrag();
   
   void DisposeDrag( Owned<DragRef> );

   void AddDragItemFlavor( DragRef     theDrag,
                           DragItemRef theItemRef,
                           FlavorType  theType,
                           const void *dataPtr,
                           Size        dataSize,
                           FlavorFlags theFlags = FlavorFlags() );

   inline void AddDragItemFlavor( DragRef     theDrag,
                                  DragItemRef theItemRef,
                                  FlavorType  theType,
                                  FlavorFlags theFlags = FlavorFlags() )
     {
      Nitrogen::AddDragItemFlavor( theDrag, theItemRef, theType, 0, 0, theFlags );
     }

   /* AddDragItemFlavor type-safe interface goes here... */

   void SetDragItemFlavorData( DragRef      theDrag,
                               DragItemRef  theItemRef,
                               FlavorType   theType,
                               const void  *dataPtr,
                               Size         dataSize,
                               UInt32       dataOffset = 0 );
   
   /* SetDragItemFlavorData type-safe interface goes here... */
   
   /*  SetDragSendProc... */
   /*  SetDragInputProc... */
   /*  SetDragDrawingProc... */
   
   void SetDragImage( DragRef        theDrag,
                      PixMapHandle   imagePixMap,
                      RgnHandle      imageRgn,
                      Point          imageOffsetPt,
                      DragImageFlags theImageFlags );

   void ChangeDragBehaviors( DragRef       theDrag,
                             DragBehaviors inBehaviorsToSet,
                             DragBehaviors inBehaviorsToClear );

   void TrackDrag( DragRef            theDrag,
                   const EventRecord& theEvent,
                   RgnHandle          theRegion );
   
   UInt16 CountDragItems( DragRef theDrag );
   
   DragItemRef GetDragItemReferenceNumber( DragRef theDrag, UInt16 index );

   UInt16 CountDragItemFlavors( DragRef theDrag, DragItemRef theItemRef );
   
   FlavorType GetFlavorType( DragRef theDrag, DragItemRef theItemRef, UInt16 index );

   FlavorFlags GetFlavorFlags( DragRef theDrag, DragItemRef theItemRef, FlavorType theType );

   Size GetFlavorDataSize( DragRef theDrag, DragItemRef theItemRef, FlavorType theType );

   Size GetFlavorData( DragRef      theDrag,
                       DragItemRef  theItemRef,
                       FlavorType   theType,
                       void        *dataPtr,
                       Size         dataSize,
                       UInt32       dataOffset = 0 );

   /* GetFlavorData type-safe interface goes here... */
   
   Rect GetDragItemBounds( DragRef theDrag, DragItemRef theItemRef );

   void SetDragItemBounds( DragRef theDrag, DragItemRef theItemRef, const Rect& itemBounds );
   
   AEDesc GetDropLocation( DragRef theDrag );

   void SetDropLocation( DragRef theDrag, const AEDesc& dropLocation );

   DragAttributes GetDragAttributes( DragRef theDrag );

   struct GetDragMouse_Result
     {
      Point mouse;
      Point globalPinnedMouse;
     };
   
   GetDragMouse_Result GetDragMouse( DragRef theDrag );
   
   void SetDragMouse( DragRef theDrag, Point globalPinnedMouse );
   
   Point GetDragOrigin( DragRef theDrag );
   
   struct GetDragModifiers_Result
     {
      EventModifiers modifiers;
      EventModifiers mouseDownModifiers;
      EventModifiers mouseUpModifiers;
     };
   
   GetDragModifiers_Result GetDragModifiers( DragRef theDrag );
   
   DragActions GetDragAllowableActions( DragRef theDrag );

   void SetDragAllowableActions( DragRef theDrag, DragActions inActions, bool isLocal );

   DragActions GetDragDropAction( DragRef theDrag );

   void SetDragDropAction( DragRef theDrag, DragActions inAction );

   void ShowDragHilite( DragRef theDrag, RgnHandle hiliteFrame, bool inside );

   void HideDragHilite( DragRef theDrag );

   void DragPreScroll( DragRef theDrag, SInt16 dH, SInt16 dV );
   inline void DragPreScroll( DragRef theDrag, Point d )          { Nitrogen::DragPreScroll( theDrag, d.h, d.v ); }

   void DragPostScroll( DragRef theDrag );

   void UpdateDragHilite( DragRef theDrag, RgnHandle updateRgn );

   RGBColor GetDragHiliteColor( WindowRef window );
   
   inline bool WaitMouseMoved( Point initialMouse )               { return ::WaitMouseMoved( initialMouse ); }

   void ZoomRects( const Rect&      fromRect,
                   const Rect&      toRect,
                   SInt16           zoomSteps,
                   ZoomAcceleration acceleration );

   void ZoomRegion( RgnHandle        region,
                    Point            zoomDistance,
                    SInt16           zoomSteps,
                    ZoomAcceleration acceleration );
  }
 
#endif
