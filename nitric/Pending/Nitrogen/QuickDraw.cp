// QuickDraw.cp

#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/QuickDraw.h"
#endif

#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif

#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

#ifndef NITROGEN_SCOPED_H
#include "Nitrogen/Scoped.h"
#endif

namespace Nitrogen
  {
	
	GrafPtr GetPort()
	{
		::GrafPtr port;
		::GetPort( &port );
		return port;
	}
	
	bool QDSwapPort( CGrafPtr newPort, CGrafPtr& oldPort )
	{
	#if TARGET_API_MAC_CARBON
		
		return ::QDSwapPort( newPort, &oldPort );
		
	#else
		
		oldPort = GetPort();
		SetPort( newPort );
		return oldPort != newPort;
		
	#endif
	}
	
	CGrafPtr QDSwapPort( CGrafPtr newPort )
	{
		CGrafPtr oldPort;
		bool portChanged = QDSwapPort( newPort, oldPort );
		return oldPort;
	}
	
	Owned< RgnHandle > GetClip()
	{
		Owned< RgnHandle > result = NewRgn();
		::GetClip( result.Get() );
		return result;
	}
	
	void MacSetCursor( ResID id )
	{
		MacSetCursor( MacGetCursor( id ) );
	}
	
	PenState GetPenState()
	{
		PenState penState;
		GetPenState( &penState );
		return penState;
	}
	
	Rect MacOffsetRect( const Rect& r, short dh, short dv )
	{
		return MacSetRect
		(
			r.left   + dh, 
			r.top    + dv, 
			r.right  + dh, 
			r.bottom + dv
		);
	}
	
	Rect MacInsetRect( const Rect& r, short dh, short dv )
	{
		return MacSetRect
		(
			r.left   + dh, 
			r.top    + dv, 
			r.right  - dh, 
			r.bottom - dv
		);
	}
	
	SectRect_Result SectRect( const Rect& a, const Rect& b )
	{
		SectRect_Result result;
		result.intersected = ::SectRect( &a, &b, &result.intersection );
		return result;
	}
	
	Rect MacUnionRect( const Rect& a, const Rect& b )
	{
		Rect result;
		::MacUnionRect( &a, &b, &result );
		return result;
	}
	
	Owned<RgnHandle> NewRgn(void) {
		OnlyOnce<RegisterQuickDrawErrors> ();
		RgnHandle result = ::NewRgn ();
		if ( result == NULL )
			throw MemFullErr ();
		return Owned<RgnHandle>::Seize ( result );
		}
	
	Owned<RgnHandle> MacCopyRgn ( RgnHandle srcRgn ) {
		Owned<RgnHandle> result = Owned<RgnHandle>::Seize ( ::NewRgn ());
		::MacCopyRgn ( srcRgn, result );
		return result;
		}
	
	RgnHandle RectRgn( RgnHandle region, const Rect& rect )
	{
		::RectRgn( region, &rect );
		return region;
	}
	
	Owned< RgnHandle > RectRgn( const Rect& rect )
	{
		Owned< RgnHandle > result = NewRgn();
		::RectRgn( result, &rect );
		return result;
	}
	
	Owned< RgnHandle > SectRgn( RgnHandle a, RgnHandle b )
	{
		Owned< RgnHandle > result = NewRgn();
		SectRgn( a, b, result );
		return result;
	}
	
	Owned< RgnHandle > DiffRgn( RgnHandle a, RgnHandle b )
	{
		Owned< RgnHandle > result = NewRgn();
		DiffRgn( a, b, result );
		return result;
	}
	
	void ScrollRect( const Rect& r, short dh, short dv, RgnHandle updateRgn )
	{
		::ScrollRect( &r, dh, dv, updateRgn );
	}
	
	Owned< RgnHandle > ScrollRect( const Rect& r, short dh, short dv )
	{
		Owned< RgnHandle > region = NewRgn();
		ScrollRect( r, dh, dv, region );
		return region;
	}
	
	void CopyBits
	(
		const BitMap* srcBits, 
		const BitMap* dstBits, 
		const Rect& srcRect, 
		const Rect& dstRect, 
		TransferMode mode, 
		RgnHandle maskRgn )
	{
		::CopyBits( srcBits, dstBits, &srcRect, &dstRect, mode, maskRgn );
	}
	
	Point LocalToGlobal( Point point )
	{
		::LocalToGlobal( &point );
		return point;
	}
	
	Rect LocalToGlobal( const Rect& rect )
	{
		return MacSetRect
		(
			LocalToGlobal( SetPt( rect.left,  rect.top    ) ), 
			LocalToGlobal( SetPt( rect.right, rect.bottom ) )
		);
	}
	
	Point GlobalToLocal( Point point )
	{
		::GlobalToLocal( &point );
		return point;
	}
	
	Rect GlobalToLocal( const Rect& rect )
	{
		return MacSetRect
		(
			GlobalToLocal( SetPt( rect.left,  rect.top    ) ), 
			GlobalToLocal( SetPt( rect.right, rect.bottom ) )
		);
	}
	
	Point AddPt( Point a, Point b )
	{
		return SetPt
		(
			a.h + b.h, 
			a.v + b.v
		);
	}
	
	Point SubPt( Point a, Point b )
	{
		return SetPt
		(
			a.h - b.h, 
			a.v - b.v
		);
	}
	
	CursHandle MacGetCursor( ResID id )
	{
		return CheckResource( ::MacGetCursor( id ) );
	}
	
#if OPAQUE_TOOLBOX_STRUCTS
	
	Rect GetPortBounds( CGrafPtr port )
	{
		Rect bounds;
		return *( ::GetPortBounds( port, &bounds ) );
	}
	
#endif
	
	RGBColor GetPortForeColor( CGrafPtr port )
	{
	#if TARGET_API_MAC_CARBON
		
		RGBColor result;
		::GetPortForeColor( port, &result );
		return result;
		
	#else
		
		return ::CGrafPtr( port )->rgbFgColor;
		
	#endif
	}
	
	RGBColor GetPortBackColor( CGrafPtr port )
	{
	#if TARGET_API_MAC_CARBON
		
		RGBColor result;
		::GetPortBackColor( port, &result );
		return result;
		
	#else
		
		return ::CGrafPtr( port )->rgbBkColor;
		
	#endif
	}
	
#if TARGET_CPU_PPC
	
	Owned< RgnHandle > GetPortVisibleRegion( CGrafPtr port )
	{
		Owned< RgnHandle > region = NewRgn();
		::GetPortVisibleRegion( port, region );
		return region;
	}
	
#endif
	
#if !OPAQUE_TOOLBOX_STRUCTS
	
	RgnHandle GetPortClipRegion( CGrafPtr port, RgnHandle region )
	{
		Scoped< ::GrafPtr& > thePort( qd.thePort, port );
		GetClip( region );
		return region;
	}
	
#endif
	
	Owned< RgnHandle > GetPortClipRegion( CGrafPtr port )
	{
		Owned< RgnHandle > region = NewRgn();
		GetPortClipRegion( port, region );
		return region;
	}
	
#if TARGET_CPU_PPC
	
	Point GetPortPenSize( CGrafPtr port )
	{
		Point result;
		::GetPortPenSize( port, &result );
		return result;
	}
	
#endif
	
	bool IsPortColor( CGrafPtr port )
	{
	#if TARGET_API_MAC_CARBON
		
		return ::IsPortColor( port );
		
	#else
		
		// Taken from QISA/MoreIsBetter/MoreQuickDraw.cp
		return ::CGrafPtr( port )->portVersion < 0;
		
	#endif
	}
	
#if OPAQUE_TOOLBOX_STRUCTS
	
	template < class Type >
	Type& QDGlobalsVar
	(
	#if !TARGET_RT_MAC_MACHO
		pascal
	#endif
		Type* (getter)(Type*)
	)
	{
		static Type var;
		return *getter( &var );
	}
	
	// Thread-safety note:
	//
	// GetQDGlobalsScreenBits() and GetQDGlobalsArrow() should be okay,
	// since two simultaneous accesses would be copying the same data.
	// The Pattern-returning functions are not thread-safe.
	
	const BitMap&  GetQDGlobalsScreenBits()  { return QDGlobalsVar( ::GetQDGlobalsScreenBits ); }
	const Cursor&  GetQDGlobalsArrow()       { return QDGlobalsVar( ::GetQDGlobalsArrow ); }
	const Pattern& GetQDGlobalsDarkGray()    { return QDGlobalsVar( ::GetQDGlobalsDarkGray ); }
	const Pattern& GetQDGlobalsLightGray()   { return QDGlobalsVar( ::GetQDGlobalsLightGray ); }
	const Pattern& GetQDGlobalsGray()        { return QDGlobalsVar( ::GetQDGlobalsGray ); }
	const Pattern& GetQDGlobalsBlack()       { return QDGlobalsVar( ::GetQDGlobalsBlack ); }
	const Pattern& GetQDGlobalsWhite()       { return QDGlobalsVar( ::GetQDGlobalsWhite ); }
	
#endif
	
	void RegisterQuickDrawErrors () {
		RegisterOSStatus< memFullErr >();
		}
  
  }