// Nitrogen/Drag.hh
// ----------------
//
// Maintained by Marshall Clow

// Part of the Nitrogen project.
//
// Written 2004-2009 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_DRAG_HH
#define NITROGEN_DRAG_HH

#ifndef __DRAG__
#include <Drag.h>
#endif
#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif
#ifndef NITROGEN_EVENTS_HH
#include "Nitrogen/Events.hh"
#endif
#ifndef NITROGEN_FILES_HH
#include "Nitrogen/Files.hh"
#endif
#ifndef NITROGEN_APPLEEVENTS_HH
#include "Nitrogen/AppleEvents.hh"
#endif
#ifndef NITROGEN_QUICKDRAW_HH
#include "Nitrogen/Quickdraw.hh"
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"

#ifndef NUCLEUS_FLATTENER_H
#include "Nucleus/Flattener.h"
#endif

namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( DragManager );
	
   
   using ::DragRef;
	
	enum DragItemRef
	{
		kDragItemRef_Max = nucleus::enumeration_traits< ::DragItemRef >::max
	};
	
	enum FlavorType
	{
		kFlavorType_Max = nucleus::enumeration_traits< ::FlavorType >::max
	};
	
	enum DragAttributes
	{
		kDragAttributes_Max = nucleus::enumeration_traits< ::DragAttributes >::max
	};
	
   NUCLEUS_DEFINE_FLAG_OPS( DragAttributes )
   
	enum DragBehaviors
	{
		kDragBehaviors_Max = nucleus::enumeration_traits< ::DragBehaviors >::max
	};
	
   NUCLEUS_DEFINE_FLAG_OPS( DragBehaviors )
   
	enum DragImageFlags
	{
		kDragImageFlags_Max = nucleus::enumeration_traits< ::DragImageFlags >::max
	};
	
   NUCLEUS_DEFINE_FLAG_OPS( DragImageFlags )
    
	enum DragRegionMessage
	{
		kDragRegionMessage_Max = nucleus::enumeration_traits< ::DragRegionMessage >::max
	};
	
	enum ZoomAcceleration
	{
		kZoomAcceleration_Max = nucleus::enumeration_traits< ::ZoomAcceleration >::max
	};
	
	enum FlavorFlags
	{
		kFlavorFlags_Max = nucleus::enumeration_traits< ::FlavorFlags >::max
	};
	
   NUCLEUS_DEFINE_FLAG_OPS( FlavorFlags )
    
	enum DragTrackingMessage
	{
		kDragTrackingMessage_Max = nucleus::enumeration_traits< ::DragTrackingMessage >::max
	};
	
	enum DragActions
	{
		kDragActions_Max = nucleus::enumeration_traits< ::DragActions >::max
	};
	
   NUCLEUS_DEFINE_FLAG_OPS( DragActions )
   
   using ::HFSFlavor;
   using ::PromiseHFSFlavor;
   
   // FlavorType_Traits havce the same format as Flatteners.
   template < ::FlavorType > struct FlavorType_Traits;
   
   template <> struct FlavorType_Traits< kDragFlavorTypeHFS                  >: public Nucleus::PodFlattener< HFSFlavor > {};
   template <> struct FlavorType_Traits< kDragFlavorTypePromiseHFS           >: public Nucleus::PodFlattener< PromiseHFSFlavor > {};
   template <> struct FlavorType_Traits< kFlavorTypeClippingName             >: public Nucleus::StringFlattener< std::string > {};
   template <> struct FlavorType_Traits< kFlavorTypeClippingFilename         >: public Nucleus::StringFlattener< std::string > {};
   template <> struct FlavorType_Traits< kFlavorTypeDragToTrashOnly          >: public Nucleus::NoDataFlattener {};
   template <> struct FlavorType_Traits< kFlavorTypeFinderNoTrackingBehavior >: public Nucleus::NoDataFlattener {};
   
   /* UPPs... */
   /* Handlers... */

  }

namespace nucleus
  {
   template <> struct disposer< Nitrogen::DragRef >: public std::unary_function< Nitrogen::DragRef, void >
     {
      void operator()( Nitrogen::DragRef i ) const
        {
         NUCLEUS_REQUIRE_ERRORS( Nitrogen::DragManager );
         
         ::Nitrogen::HandleDestructionOSStatus( ::DisposeDrag( i ) );
        }
     };
  }

namespace Nitrogen
  {
   
   nucleus::owned<DragRef> NewDrag();
   
   void DisposeDrag( nucleus::owned<DragRef> );

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

   class AddDragItemFlavor_Putter
     {
      private:
         DragRef     drag;
         DragItemRef item;
         FlavorType  type;
         FlavorFlags flags;
      
         static UInt32 Distance( const void *begin, const void *end )
           {
            return static_cast< UInt32 >(   static_cast< const char * >( end   )
                                          - static_cast< const char * >( begin ) );
           }
         
      public:
         AddDragItemFlavor_Putter( DragRef     theDrag,
                                   DragItemRef theItemRef,
                                   FlavorType  theType,
                                   FlavorFlags theFlags = FlavorFlags() )
           : drag( theDrag ),
             item( theItemRef ),
             type( theType ),
             flags( theFlags )
           {}
         
         void operator()( const void *begin, const void *end ) const
           {
            Nitrogen::AddDragItemFlavor( drag, item, type, begin, Distance( begin, end ), flags );
           }
     };
   
   template < ::FlavorType theType >
   struct AddDragItemFlavor_Traits
     {
      typedef typename FlavorType_Traits< theType >::Put_Parameter Data_Type;
     };
   
   template < ::FlavorType theType >
   void AddDragItemFlavor( DragRef                                               theDrag,
                           DragItemRef                                           theItemRef,
                           typename AddDragItemFlavor_Traits<theType>::Data_Type data,
                           FlavorFlags                                           theFlags = FlavorFlags() )
     {
      FlavorType_Traits< theType >::Put( data, AddDragItemFlavor_Putter( theDrag, theItemRef, theType, theFlags ) );
     }

   void SetDragItemFlavorData( DragRef      theDrag,
                               DragItemRef  theItemRef,
                               FlavorType   theType,
                               const void  *dataPtr,
                               Size         dataSize,
                               UInt32       dataOffset = 0 );


   class SetDragItemFlavorData_Putter
     {
      private:
         DragRef     drag;
         DragItemRef item;
         FlavorType  type;
         UInt32      offset;
         
         static UInt32 Distance( const void *begin, const void *end )
           {
            return static_cast< UInt32 >(   static_cast< const char * >( end   )
                                          - static_cast< const char * >( begin ) );
           }
         
      public:
         SetDragItemFlavorData_Putter( DragRef     theDrag,
                                       DragItemRef theItemRef,
                                       FlavorType  theType,
                                       UInt32      dataOffset = 0 )
           : drag( theDrag ),
             item( theItemRef ),
             type( theType ),
             offset( dataOffset )
           {}
         
         void operator()( const void *begin, const void *end ) const
           {
            Nitrogen::SetDragItemFlavorData( drag, item, type, begin, Distance( begin, end ), offset );
           }
     };

   template < ::FlavorType theType >
   struct SetDragItemFlavorData_Traits
     {
      typedef typename FlavorType_Traits< theType >::Put_Parameter Data_Type;
     };
   
   template < ::FlavorType theType >
   void SetDragItemFlavorData( DragRef                                                   theDrag,
                               DragItemRef                                               theItemRef,
                               typename SetDragItemFlavorData_Traits<theType>::Data_Type data )
     {
      FlavorType_Traits< theType >::Put( data, SetDragItemFlavorData_Putter( theDrag, theItemRef, theType ) );
     }
   
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

   class GetFlavorData_Getter
     {
      private:
         DragRef     drag;
         DragItemRef item;
         FlavorType  type;
         UInt32      offset;
         
         static UInt32 Distance( void *begin, void *end )
           {
            return static_cast< UInt32 >(   static_cast< const char * >( end   )
                                          - static_cast< const char * >( begin ) );
           }
         
      public:
         GetFlavorData_Getter( DragRef     theDrag,
                               DragItemRef theItemRef,
                               FlavorType  theType,
                               UInt32      dataOffset = 0 )
           : drag( theDrag ),
             item( theItemRef ),
             type( theType ),
             offset( dataOffset )
           {}
         
         std::size_t size() const
           {
            return GetFlavorDataSize( drag, item, type ) - offset;
           }
         
         void operator()( void *begin, void *end ) const
           {
            Nitrogen::GetFlavorData( drag, item, type, begin, Distance( begin, end ), offset );
           }
     };

   template < ::FlavorType theType >
   struct GetFlavorData_Traits
     {
      typedef typename FlavorType_Traits< theType >::Get_Result Result;
     };
   
   template < ::FlavorType theType >
   typename GetFlavorData_Traits<theType>::Result GetFlavorData( DragRef theDrag, DragItemRef theItemRef )
     {
      return FlavorType_Traits< theType >::Get( GetFlavorData_Getter( theDrag, theItemRef, theType ) );
     }
   
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
