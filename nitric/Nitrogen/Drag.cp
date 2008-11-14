// Nitrogen/Drag.cp
// ----------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2006 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_DRAG_H
#include "Nitrogen/Drag.h"
#endif

namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( DragManager )
	
	
	static void RegisterDragManagerErrors();
	
	
#pragma force_active on
	
	class DragManagerErrorsRegistration
	{
		public:
			DragManagerErrorsRegistration()  { RegisterDragManagerErrors(); }
	};
	
	static DragManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
	
	
   Nucleus::Owned<DragRef> NewDrag()
     {
      DragRef result;
      ThrowOSStatus( ::NewDrag( &result ) );
      return Nucleus::Owned<DragRef>::Seize( result );
     }
   
   void DisposeDrag( Nucleus::Owned<DragRef> theDrag )
     {
      ThrowOSStatus( ::DisposeDrag( theDrag.release() ) );
     }

   void AddDragItemFlavor( DragRef     theDrag,
                           DragItemRef theItemRef,
                           FlavorType  theType,
                           const void *dataPtr,
                           Size        dataSize,
                           FlavorFlags theFlags )
     {
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
      ThrowOSStatus( ::ChangeDragBehaviors( theDrag,
                                            inBehaviorsToSet,
                                            inBehaviorsToClear ) );
     }

   void TrackDrag( DragRef            theDrag,
                   const EventRecord& theEvent,
                   RgnHandle          theRegion )
     {
      ThrowOSStatus( ::TrackDrag( theDrag, &theEvent, theRegion ) );
     }     

   UInt16 CountDragItems( DragRef theDrag )
     {
      UInt16 result;
      ThrowOSStatus( ::CountDragItems( theDrag, &result ) );
      return result;
     }

   DragItemRef GetDragItemReferenceNumber( DragRef theDrag, UInt16 index )
     {
      ::DragItemRef result;
      ThrowOSStatus( ::GetDragItemReferenceNumber( theDrag, index, &result ) );
      return DragItemRef( result );
     }

   UInt16 CountDragItemFlavors( DragRef theDrag, DragItemRef theItemRef )
     {
      UInt16 result;
      ThrowOSStatus( ::CountDragItemFlavors( theDrag, theItemRef, &result ) );
      return result;
     }

   FlavorType GetFlavorType( DragRef theDrag, DragItemRef theItemRef, UInt16 index )
     {
      ::FlavorType result;
      ThrowOSStatus( ::GetFlavorType( theDrag, theItemRef, index, &result ) );
      return FlavorType( result );
     }

   FlavorFlags GetFlavorFlags( DragRef theDrag, DragItemRef theItemRef, FlavorType theType )
     {
      ::FlavorFlags result;
      ThrowOSStatus( ::GetFlavorFlags( theDrag, theItemRef, theType, &result ) );
      return FlavorFlags( result );
     }

   Size GetFlavorDataSize( DragRef theDrag, DragItemRef theItemRef, FlavorType theType )
     {
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
      ::Size result = static_cast< ::Size >( dataSize );
      ThrowOSStatus( ::GetFlavorData( theDrag, theItemRef, theType, dataPtr, &result, dataOffset ) );
      return static_cast< Size >( result );
     }

   Rect GetDragItemBounds( DragRef theDrag, DragItemRef theItemRef )
     {
      Rect result;
      ThrowOSStatus( ::GetDragItemBounds( theDrag, theItemRef, &result ) );
      return result;
     }

   void SetDragItemBounds( DragRef theDrag, DragItemRef theItemRef, const Rect& itemBounds )
     {
      ThrowOSStatus( ::SetDragItemBounds( theDrag, theItemRef, &itemBounds ) );
     }

   AEDesc GetDropLocation( DragRef theDrag )
     {
      AEDesc result;
      ThrowOSStatus( ::GetDropLocation( theDrag, &result ) );
      return result;
     }

   void SetDropLocation( DragRef theDrag, const AEDesc& dropLocation )
     {
      ThrowOSStatus( ::SetDropLocation( theDrag, &dropLocation ) );
     }

   DragAttributes GetDragAttributes( DragRef theDrag )
     {
      ::DragAttributes result;
      ThrowOSStatus( ::GetDragAttributes( theDrag, &result ) );
      return DragAttributes( result );
     }

   GetDragMouse_Result GetDragMouse( DragRef theDrag )
     {
      GetDragMouse_Result result;
      ThrowOSStatus( ::GetDragMouse( theDrag, &result.mouse, &result.globalPinnedMouse ) );
      return result;
     }

   void SetDragMouse( DragRef theDrag, Point globalPinnedMouse )
     {
      ThrowOSStatus( ::SetDragMouse( theDrag, globalPinnedMouse ) );
     }

   Point GetDragOrigin( DragRef theDrag )
     {
      Point result;
      ThrowOSStatus( ::GetDragOrigin( theDrag, &result ) );
      return result;
     }
   
   GetDragModifiers_Result GetDragModifiers( DragRef theDrag )
     {
      
      SInt16 modifiers;
      SInt16 mouseDownModifiers;
      SInt16 mouseUpModifiers;
      ThrowOSStatus( ::GetDragModifiers( theDrag, &modifiers, &mouseDownModifiers, &mouseUpModifiers ) );
      
      GetDragModifiers_Result result;
      result.modifiers          = EventModifiers( static_cast< ::EventModifiers >( modifiers          ) );
      result.mouseDownModifiers = EventModifiers( static_cast< ::EventModifiers >( mouseDownModifiers ) );
      result.mouseUpModifiers   = EventModifiers( static_cast< ::EventModifiers >( mouseUpModifiers   ) );

      return result;
     }

   DragActions GetDragAllowableActions( DragRef theDrag )
     {
      ::DragActions result;
      ThrowOSStatus( ::GetDragAllowableActions( theDrag, &result ) );
      return DragActions( result );
     }

   void SetDragAllowableActions( DragRef theDrag, DragActions inActions, bool isLocal )
     {
      ThrowOSStatus( ::SetDragAllowableActions( theDrag, inActions, isLocal ) );
     }

   DragActions GetDragDropAction( DragRef theDrag )
     {
      ::DragActions result;
      ThrowOSStatus( ::GetDragDropAction( theDrag, &result ) );
      return DragActions( result );
     }

   void SetDragDropAction( DragRef theDrag, DragActions inAction )
     {
      ThrowOSStatus( ::SetDragDropAction( theDrag, inAction ) );
     }

   void ShowDragHilite( DragRef theDrag, RgnHandle hiliteFrame, bool inside )
     {
      ThrowOSStatus( ::ShowDragHilite( theDrag, hiliteFrame, inside ) );
     }

   void HideDragHilite( DragRef theDrag )
     {
      ThrowOSStatus( ::HideDragHilite( theDrag ) );
     }

   void DragPreScroll( DragRef theDrag, SInt16 dH, SInt16 dV )
     {
      ThrowOSStatus( ::DragPreScroll( theDrag, dH, dV ) );
     }

   void DragPostScroll( DragRef theDrag )
     {
      ThrowOSStatus( ::DragPostScroll( theDrag ) );
     }

   void UpdateDragHilite( DragRef theDrag, RgnHandle updateRgn )
     {
      ThrowOSStatus( ::UpdateDragHilite( theDrag, updateRgn ) );
     }

   RGBColor GetDragHiliteColor( WindowRef window )
     {
      RGBColor result;
      ThrowOSStatus( ::GetDragHiliteColor( window, &result ) );
      return result;
     }

   void ZoomRects( const Rect&      fromRect,
                   const Rect&      toRect,
                   SInt16           zoomSteps,
                   ZoomAcceleration acceleration )
     {
      ThrowOSStatus( ::ZoomRects( &fromRect, &toRect, zoomSteps, acceleration ) );
     }

   void ZoomRegion( RgnHandle        region,
                    Point            zoomDistance,
                    SInt16           zoomSteps,
                    ZoomAcceleration acceleration )
     {
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
