// Drag.cp

#ifndef NITROGEN_DRAG_H
#include "Nitrogen/Drag.h"
#endif

namespace Nitrogen
  {
   Owned<DragRef> NewDrag()
     {
      OnlyOnce<RegisterDragManagerErrors>();
      DragRef result;
      ThrowOSStatus( ::NewDrag( &result ) );
      return Owned<DragRef>::Seize( result );
     }
   
   void DisposeDrag( Owned<DragRef> theDrag )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::DisposeDrag( theDrag.release() ) );
     }

   void AddDragItemFlavor( DragRef     theDrag,
                           DragItemRef theItemRef,
                           FlavorType  theType,
                           const void *dataPtr,
                           Size        dataSize,
                           FlavorFlags theFlags )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::AddDragItemFlavor( theDrag,
                                          theItemRef,
                                          theType,
                                          dataPtr,
                                          static_cast< ::Size >( dataSize ),
                                          theFlags ) );
     }

   void SetDragItemFlavorData( DragRef      theDrag,
                               DragItemRef  theItemRef,
                               FlavorType   theType,
                               const void  *dataPtr,
                               Size         dataSize,
                               UInt32       dataOffset )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::SetDragItemFlavorData( theDrag,
                                              theItemRef,
                                              theType,
                                              dataPtr,
                                              static_cast< ::Size >( dataSize ),
                                              dataOffset ) );
     }

   void SetDragImage( DragRef        theDrag,
                      PixMapHandle   imagePixMap,
                      RgnHandle      imageRgn,
                      Point          imageOffsetPt,
                      DragImageFlags theImageFlags )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::SetDragImage( theDrag,
                                     imagePixMap,
                                     imageRgn,
                                     imageOffsetPt,
                                     theImageFlags ) );
     }

   void ChangeDragBehaviors( DragRef       theDrag,
                             DragBehaviors inBehaviorsToSet,
                             DragBehaviors inBehaviorsToClear )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::ChangeDragBehaviors( theDrag,
                                            inBehaviorsToSet,
                                            inBehaviorsToClear ) );
     }

   void TrackDrag( DragRef            theDrag,
                   const EventRecord& theEvent,
                   RgnHandle          theRegion )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::TrackDrag( theDrag, &theEvent, theRegion ) );
     }     

   UInt16 CountDragItems( DragRef theDrag )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      UInt16 result;
      ThrowOSStatus( ::CountDragItems( theDrag, &result ) );
      return result;
     }

   DragItemRef GetDragItemReferenceNumber( DragRef theDrag, UInt16 index )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ::DragItemRef result;
      ThrowOSStatus( ::GetDragItemReferenceNumber( theDrag, index, &result ) );
      return result;
     }

   UInt16 CountDragItemFlavors( DragRef theDrag, DragItemRef theItemRef )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      UInt16 result;
      ThrowOSStatus( ::CountDragItemFlavors( theDrag, theItemRef, &result ) );
      return result;
     }

   FlavorType GetFlavorType( DragRef theDrag, DragItemRef theItemRef, UInt16 index )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ::FlavorType result;
      ThrowOSStatus( ::GetFlavorType( theDrag, theItemRef, index, &result ) );
      return result;
     }

   FlavorFlags GetFlavorFlags( DragRef theDrag, DragItemRef theItemRef, FlavorType theType )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ::FlavorFlags result;
      ThrowOSStatus( ::GetFlavorFlags( theDrag, theItemRef, theType, &result ) );
      return result;
     }

   Size GetFlavorDataSize( DragRef theDrag, DragItemRef theItemRef, FlavorType theType )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ::Size result;
      ThrowOSStatus( ::GetFlavorDataSize( theDrag, theItemRef, theType, &result ) );
      return static_cast<Size>( result );
     }

   Size GetFlavorData( DragRef      theDrag,
                       DragItemRef  theItemRef,
                       FlavorType   theType,
                       void        *dataPtr,
                       Size         dataSize,
                       UInt32       dataOffset )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ::Size result = static_cast< ::Size >( dataSize );
      ThrowOSStatus( ::GetFlavorData( theDrag, theItemRef, theType, dataPtr, &result, dataOffset ) );
      return static_cast< Size >( result );
     }

   Rect GetDragItemBounds( DragRef theDrag, DragItemRef theItemRef )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      Rect result;
      ThrowOSStatus( ::GetDragItemBounds( theDrag, theItemRef, &result ) );
      return result;
     }

   void SetDragItemBounds( DragRef theDrag, DragItemRef theItemRef, const Rect& itemBounds )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::SetDragItemBounds( theDrag, theItemRef, &itemBounds ) );
     }

   AEDesc GetDropLocation( DragRef theDrag )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      AEDesc result;
      ThrowOSStatus( ::GetDropLocation( theDrag, &result ) );
      return result;
     }

   void SetDropLocation( DragRef theDrag, const AEDesc& dropLocation )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::SetDropLocation( theDrag, &dropLocation ) );
     }

   DragAttributes GetDragAttributes( DragRef theDrag )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ::DragAttributes result;
      ThrowOSStatus( ::GetDragAttributes( theDrag, &result ) );
      return result;
     }

   GetDragMouse_Result GetDragMouse( DragRef theDrag )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      GetDragMouse_Result result;
      ThrowOSStatus( ::GetDragMouse( theDrag, &result.mouse, &result.globalPinnedMouse ) );
      return result;
     }

   void SetDragMouse( DragRef theDrag, Point globalPinnedMouse )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::SetDragMouse( theDrag, globalPinnedMouse ) );
     }

   Point GetDragOrigin( DragRef theDrag )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      Point result;
      ThrowOSStatus( ::GetDragOrigin( theDrag, &result ) );
      return result;
     }
   
   GetDragModifiers_Result GetDragModifiers( DragRef theDrag )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      
      SInt16 modifiers;
      SInt16 mouseDownModifiers;
      SInt16 mouseUpModifiers;
      ThrowOSStatus( ::GetDragModifiers( theDrag, &modifiers, &mouseDownModifiers, &mouseUpModifiers ) );
      
      GetDragModifiers_Result result;
      result.modifiers          = static_cast< ::EventModifiers >( modifiers );
      result.mouseDownModifiers = static_cast< ::EventModifiers >( mouseDownModifiers );
      result.mouseUpModifiers   = static_cast< ::EventModifiers >( mouseUpModifiers );

      return result;
     }

   DragActions GetDragAllowableActions( DragRef theDrag )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ::DragActions result;
      ThrowOSStatus( ::GetDragAllowableActions( theDrag, &result ) );
      return result;
     }

   void SetDragAllowableActions( DragRef theDrag, DragActions inActions, bool isLocal )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::SetDragAllowableActions( theDrag, inActions, isLocal ) );
     }

   DragActions GetDragDropAction( DragRef theDrag )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ::DragActions result;
      ThrowOSStatus( ::GetDragDropAction( theDrag, &result ) );
      return result;
     }

   void SetDragDropAction( DragRef theDrag, DragActions inAction )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::SetDragDropAction( theDrag, inAction ) );
     }

   void ShowDragHilite( DragRef theDrag, RgnHandle hiliteFrame, bool inside )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::ShowDragHilite( theDrag, hiliteFrame, inside ) );
     }

   void HideDragHilite( DragRef theDrag )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::HideDragHilite( theDrag ) );
     }

   void DragPreScroll( DragRef theDrag, SInt16 dH, SInt16 dV )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::DragPreScroll( theDrag, dH, dV ) );
     }

   void DragPostScroll( DragRef theDrag )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::DragPostScroll( theDrag ) );
     }

   void UpdateDragHilite( DragRef theDrag, RgnHandle updateRgn )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::UpdateDragHilite( theDrag, updateRgn ) );
     }

   RGBColor GetDragHiliteColor( WindowRef window )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      RGBColor result;
      ThrowOSStatus( ::GetDragHiliteColor( window, &result ) );
      return result;
     }

   void ZoomRects( const Rect&      fromRect,
                   const Rect&      toRect,
                   SInt16           zoomSteps,
                   ZoomAcceleration acceleration )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::ZoomRects( &fromRect, &toRect, zoomSteps, acceleration ) );
     }

   void ZoomRegion( RgnHandle        region,
                    Point            zoomDistance,
                    SInt16           zoomSteps,
                    ZoomAcceleration acceleration )
     {
      OnlyOnce<RegisterDragManagerErrors>();
      ThrowOSStatus( ::ZoomRegion( region, zoomDistance, zoomSteps, acceleration ) );
     }
   
   void RegisterDragManagerErrors()
     {
      RegisterOSStatus< badDragRefErr             >();
      RegisterOSStatus< badDragItemErr            >();
      RegisterOSStatus< badDragFlavorErr          >();
      RegisterOSStatus< duplicateFlavorErr        >();
      RegisterOSStatus< cantGetFlavorErr          >();
      RegisterOSStatus< duplicateHandlerErr       >();
      RegisterOSStatus< handlerNotFoundErr        >();
      RegisterOSStatus< dragNotAcceptedErr        >();
      RegisterOSStatus< unsupportedForPlatformErr >();
      RegisterOSStatus< noSuitableDisplaysErr     >();
      RegisterOSStatus< badImageRgnErr            >();
      RegisterOSStatus< badImageErr               >();
      RegisterOSStatus< nonDragOriginatorErr      >();
     }
  }
