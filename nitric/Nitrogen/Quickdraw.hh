// Nitrogen/Quickdraw.hh
// ---------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2009 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_QUICKDRAW_HH
#define NITROGEN_QUICKDRAW_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// missing-macos
#if ! __LP64__
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_ERRORSREGISTERED_HH
#include "nucleus/errors_registered.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif
#ifndef NUCLEUS_MAKE_HH
#include "nucleus/make.hh"
#endif
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_QUICKDRAW_UTILITIES_PORTDISPOSER_HH
#include "Mac/Quickdraw/Utilities/Port_Disposer.hh"
#endif
#ifndef MAC_RESOURCES_TYPES_RESID_HH
#include "Mac/Resources/Types/ResID.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( QuickDraw );
	
	
#if ! __LP64__
	
	// Constants of types defined elsewhere
	
	static const Mac::ResID sysPatListID = Mac::ResID( ::sysPatListID );
	
	static const Mac::ResID iBeamCursor = Mac::ResID( ::iBeamCursor );
	static const Mac::ResID crossCursor = Mac::ResID( ::crossCursor );
	static const Mac::ResID plusCursor  = Mac::ResID( ::plusCursor  );
	static const Mac::ResID watchCursor = Mac::ResID( ::watchCursor );
	
#endif
	
	#pragma mark -
	#pragma mark ** Types **
	
	enum TransferMode
	{
		srcCopy = ::srcCopy,
		
		kTransferMode_Max = nucleus::enumeration_traits< SInt16 >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( TransferMode )
	
#if ! __LP64__
	
	enum PixelType
	{
		kPixelType_Max = nucleus::enumeration_traits< ::PixelType >::max
	};
	
#endif
	
}

namespace nucleus
{
	
#if ! __LP64__
	
	template <> struct disposer< RgnHandle >
	{
		typedef RgnHandle  argument_type;
		typedef void       result_type;
		
		void operator()( RgnHandle h ) const
		{
			::DisposeRgn( h );
		}
	};
	
	template <> struct disposer< PicHandle >
	{
		typedef PicHandle  argument_type;
		typedef void       result_type;
		
		void operator()( PicHandle h ) const
		{
			::KillPicture( h );
		}
	};
	
	template <> struct disposer< PolyHandle >
	{
		typedef PolyHandle  argument_type;
		typedef void        result_type;
		
		void operator()( PolyHandle h ) const
		{
			::KillPoly( h );
		}
	};
	
	template <> struct disposer< CTabHandle >
	{
		typedef CTabHandle  argument_type;
		typedef void        result_type;
		
		void operator()( CTabHandle h ) const
		{
			::DisposeCTable( h );
		}
	};
	
	/*
	template <> struct disposer< PixMapHandle >
	{
		typedef PixMapHandle  argument_type;
		typedef void          result_type;
		
		void operator()( PixMapHandle h ) const
		{
			::DisposePixMap( h );
		}
	};
	*/
	
	
	template <> struct disposer< PixPatHandle >
	{
		typedef PixPatHandle  argument_type;
		typedef void          result_type;
		
		void operator()( PixPatHandle h ) const
		{
			::DisposePixPat( h );
		}
	};
	
	template <> struct disposer< CCrsrHandle >
	{
		typedef CCrsrHandle  argument_type;
		typedef void         result_type;
		
		void operator()( CCrsrHandle h ) const
		{
			::DisposeCCursor( h );
		}
	};
	
	
	template <> struct disposer< GDHandle >
	{
		typedef GDHandle  argument_type;
		typedef void      result_type;
		
		void operator()( GDHandle h ) const
		{
			::DisposeGDevice( h );
		}
	};
	
#endif  // #if ! __LP64__
	
	
	template <> struct disposer_class< CGrafPtr >
	{
		typedef Mac::Port_Disposer< CGrafPtr > type;
	};
	
#if !OPAQUE_TOOLBOX_STRUCTS
	
	template <> struct disposer_class< GrafPtr >
	{
		typedef Mac::Port_Disposer< GrafPtr > type;
	};
	
#endif
	
	template <>
	struct maker< RGBColor >
	{
		RGBColor operator()( unsigned short  red,
		                     unsigned short  green,
		                     unsigned short  blue ) const
		{
			RGBColor result;
			
			result.red   = red;
			result.green = green;
			result.blue  = blue;
			
			return result;
		}
		
		RGBColor operator()( unsigned short gray ) const
		{
			return operator()( gray, gray, gray );
		}
		
		RGBColor operator()() const
		{
			return operator()( 0, 0, 0 );
		}
	};
	
}

namespace Nitrogen
{
	
#if ! __LP64__
	
	#pragma mark -
	#pragma mark ** Routines **
	
	// InitGraf
	// OpenPort
	// InitPort
	// ClosePort
	
	void LockPortBits  ( GrafPtr port );
	void UnlockPortBits( GrafPtr port );
	
	// QDRegionToRects
	
	using ::MacSetPort;
	
	inline
	GrafPtr GetPort()
	{
		::GrafPtr port;
		::GetPort( &port );
		
		return port;
	}
	
	inline
	bool QDSwapPort( CGrafPtr newPort, CGrafPtr& oldPort )
	{
		return ::QDSwapPort( newPort, (::CGrafPtr*) &oldPort );
	}
	
	inline
	CGrafPtr QDSwapPort( CGrafPtr newPort )
	{
		CGrafPtr oldPort;
		(void) QDSwapPort( newPort, oldPort );
		
		return oldPort;
	}
	
#if !OPAQUE_TOOLBOX_STRUCTS
	
	inline
	GrafPtr QDSwapPort( GrafPtr newPort )
	{
		return (GrafPtr) QDSwapPort( (CGrafPtr) newPort );
	}
	
#endif
	
	// GrafDevice
	// SetPortBits
	
	using ::PortSize;
	using ::MovePortTo;
	using ::SetOrigin;
	using ::SetClip;
	
	nucleus::owned< RgnHandle > GetClip();
	
	inline void ClipRect( const Rect& rect )  { ::ClipRect( &rect ); }
	
	inline void BackPat( const Pattern& pat )  { ::BackPat( &pat ); }
	
	using ::InitCursor;
	
	inline void MacSetCursor( const Cursor& curs )  { ::MacSetCursor( &curs ); }
	inline void MacSetCursor( CursHandle    curs )  { ::MacSetCursor( *curs ); }
	void MacSetCursor( Mac::ResID id );  // calls GetCursor()
	
	using ::HideCursor;
	using ::MacShowCursor;
	using ::ObscureCursor;
	using ::HidePen;
	using ::ShowPen;
	
	Point GetPen();
	
	inline
	PenState GetPenState()
	{
		PenState penState;
		::GetPenState( &penState );
		
		return penState;
	}
	
	inline void SetPenState( const PenState& pen )  { ::SetPenState( &pen ); }
	
	using ::PenSize;
	
	void PenMode( TransferMode mode );
	
	inline void PenPat( const Pattern& pat )  { ::PenPat( &pat ); }
	
	using ::PenNormal;
	using ::MoveTo;
	using ::Move;
	using ::MacLineTo;
	using ::Line;
	
	// ForeColor
	// BackColor
	// ColorBit
	
	inline Rect Make_Rect( short top, short left, short bottom, short right )
	{
		Rect result = { top, left, bottom, right };
		
		return result;
	}
	
	inline Rect MacSetRect( short left, short top, short right, short bottom )
	{
		return Make_Rect( top, left, bottom, right );
	}
	
	inline
	Rect MacSetRect( Point topLeft, Point bottomRight )
	{
		return MacSetRect( topLeft.h, topLeft.v, bottomRight.h, bottomRight.v );
	}
	
	Rect MacOffsetRect( const Rect& r, short dh, short dv );
	
	inline Rect MacOffsetRect( const Rect& r, Point delta )
	{
		return MacOffsetRect( r, delta.h, delta.v );
	}
	
	Rect MacInsetRect( const Rect& r, short dh, short dv );
	
	struct SectRect_Result
	{
		Rect intersection;
		bool intersected;
		
		operator const Rect&() const  { return intersection; }
		operator bool()        const  { return intersected;  }
	};
	
	SectRect_Result SectRect( const Rect& a, const Rect& b );
	
	inline
	Rect MacUnionRect( const Rect& a, const Rect& b )
	{
		Rect result;
		::MacUnionRect( &a, &b, &result );
		
		return result;
	}
	
	bool MacEqualRect( const Rect& a, const Rect& b );
	
	bool EmptyRect( const Rect& r );
	
	inline void MacFrameRect ( const Rect& r )  { ::MacFrameRect ( &r ); }
	inline void PaintRect    ( const Rect& r )  { ::PaintRect    ( &r ); }
	inline void EraseRect    ( const Rect& r )  { ::EraseRect    ( &r ); }
	inline void MacInvertRect( const Rect& r )  { ::MacInvertRect( &r ); }
	
	inline
	void MacFillRect( const Rect&     r,
	                  const Pattern&  pat )  { ::MacFillRect( &r, &pat ); }
	
	inline void FrameOval ( const Rect& r )  { ::FrameOval ( &r ); }
	inline void PaintOval ( const Rect& r )  { ::PaintOval ( &r ); }
	inline void EraseOval ( const Rect& r )  { ::EraseOval ( &r ); }
	inline void InvertOval( const Rect& r )  { ::InvertOval( &r ); }
	
	inline void FillOval( const Rect&     r,
	                      const Pattern&  pat )  { ::FillOval( &r, &pat ); }
	
	// RoundRect
	// Arc
	
#endif  // #if ! __LP64__
	
//	Region handling
	nucleus::owned< RgnHandle > NewRgn();
	
#if ! __LP64__
	
//	extern void OpenRgn(void)
	using ::OpenRgn;

//	extern void CloseRgn(RgnHandle dstRgn)
	using ::CloseRgn;
	
	nucleus::owned< RgnHandle > CloseRgn();
	
	RgnHandle BitMapToRegion( RgnHandle region, const BitMap& bitMap );
	nucleus::owned< RgnHandle > BitMapToRegion( const BitMap& bitMap );
	
	RgnHandle HandleToRgn( RgnHandle region, Handle oldRegion );
	nucleus::owned< RgnHandle > HandleToRgn( Handle oldRegion );
	
	nucleus::owned< Handle > RgnToHandle( RgnHandle region );
	
	inline void DisposeRgn( nucleus::owned< RgnHandle > )  {}
	
//	extern void MacCopyRgn ( RgnHandle srcRgn, RgnHandle dstRgn );
	using ::MacCopyRgn;
	
	nucleus::owned< RgnHandle > MacCopyRgn( RgnHandle srcRgn );
	
//	extern void SetEmptyRgn(RgnHandle rgn);
	using ::SetEmptyRgn;
	
	RgnHandle MacSetRectRgn( RgnHandle  region,
	                         short      left,
	                         short      top,
	                         short      right,
	                         short      bottom );	
	
	nucleus::owned< RgnHandle > MacSetRectRgn( short  left,
	                                           short  top,
	                                           short  right,
	                                           short  bottom );
	
	inline
	RgnHandle RectRgn( RgnHandle region, const Rect& rect )
	{
		::RectRgn( region, &rect );
		
		return region;
	}
	
	nucleus::owned< RgnHandle > RectRgn( const Rect& rect );
	
	using ::MacOffsetRgn;
	using ::InsetRgn;
	
	using ::SectRgn;
	nucleus::owned< RgnHandle > SectRgn( RgnHandle a, RgnHandle b );
	
	using ::MacUnionRgn;
	nucleus::owned< RgnHandle > MacUnionRgn( RgnHandle a, RgnHandle b );
	
	using ::DiffRgn;
	nucleus::owned< RgnHandle > DiffRgn( RgnHandle a, RgnHandle b );
	
	using ::MacXorRgn;
	nucleus::owned< RgnHandle > MacXorRgn( RgnHandle a, RgnHandle b );
	
	inline
	bool RectInRgn( const Rect& r, RgnHandle rgn )
	{
		return ::RectInRgn( &r, rgn );
	}
	
	inline
	bool MacEqualRgn( RgnHandle a, RgnHandle b )
	{
		return ::MacEqualRgn( a, b );
	}
	
	inline bool EmptyRgn( RgnHandle rgn )  { return ::EmptyRgn( rgn ); }
	
	using ::MacFrameRgn;
	using ::MacPaintRgn;
	using ::EraseRgn;
	using ::MacInvertRgn;
	
	inline
	void MacFillRgn( RgnHandle rgn, const Pattern& pat )
	{
		::MacFillRgn( rgn, &pat );
	}
	
	inline
	void ScrollRect( const Rect& r, short dh, short dv, RgnHandle updateRgn )
	{
		::ScrollRect( &r, dh, dv, updateRgn );
	}
	
	nucleus::owned< RgnHandle > ScrollRect( const Rect& r, short dh, short dv );
	
	inline
	Rect GetPortBounds( CGrafPtr port )
	{
		Rect bounds;
		return *( ::GetPortBounds( port, &bounds ) );
	}
	
	inline
	void CopyBits( const BitMap*  srcBits,
	               const BitMap*  dstBits,
	               const Rect&    srcRect,
	               const Rect&    dstRect,
	               TransferMode   mode    = srcCopy,
	               RgnHandle      maskRgn = NULL )
	{
		::CopyBits( srcBits, dstBits, &srcRect, &dstRect, mode, maskRgn );
	}
	
	inline
	void CopyBits( const CGrafPtr  src,
	               const CGrafPtr  dst,
	               const Rect&     srcRect,
	               const Rect&     dstRect,
	               TransferMode    mode    = srcCopy,
	               RgnHandle       maskRgn = NULL )
	{
		CopyBits( GetPortBitMapForCopyBits( src ),
		          GetPortBitMapForCopyBits( dst ),
		          srcRect,
		          dstRect,
		          mode,
		          maskRgn );
	}
	
	inline
	void CopyBits( const CGrafPtr  src,
	               const CGrafPtr  dst,
	               TransferMode    mode    = srcCopy,
	               RgnHandle       maskRgn = NULL )
	{
		CopyBits( src,
		          dst,
		          GetPortBounds( src ),
		          GetPortBounds( dst ),
		          mode,
		          maskRgn );
	}
	
	// SeedFill
	// CalcMask
	
	void CopyMask( const BitMap*  srcBits,
	               const BitMap*  maskBits,
	               const BitMap*  dstBits,
	               const Rect&    srcRect,
	               const Rect&    maskRect,
	               const Rect&    dstRect );
	
	nucleus::owned< PicHandle > OpenPicture( const Rect& picFrame );
	
	// PicComment
	
	using ::ClosePicture;
	
	inline
	void DrawPicture( PicHandle pic, const Rect& r )
	{
		::DrawPicture( pic, &r );
	}
	
	inline void KillPicture( nucleus::owned< PicHandle > )  {}
	
	nucleus::owned< PolyHandle > OpenPoly();
	
	using ::ClosePoly;
	
	inline void KillPoly( nucleus::owned< PolyHandle > )  {}
	
	using ::OffsetPoly;
	using ::FramePoly;
	using ::PaintPoly;
	using ::ErasePoly;
	using ::InvertPoly;
	
	inline
	void FillPoly( PolyHandle poly, const Pattern& pat )
	{
		::FillPoly( poly, &pat );
	}
	
	inline Point Make_Point( short v, short h )
	{
		Point result = { v, h };
		
		return result;
	}
	
	inline Point SetPt( short h, short v )  { return Make_Point( v, h ); }
	
	inline
	Point LocalToGlobal( Point point )
	{
		::LocalToGlobal( &point );
		
		return point;
	}
	
	inline
	Point GlobalToLocal( Point point )
	{
		::GlobalToLocal( &point );
		
		return point;
	}
	
	Rect  LocalToGlobal( const Rect&  rect );
	Rect  GlobalToLocal( const Rect&  rect );
	
	using ::Random;
	
	// StuffHex
	
	inline
	bool MacGetPixel( short h, short v )
	{
		return ::MacGetPixel( h, v );
	}
	
	/*
	Point ScalePt( Point pt, const Rect& srcRect, const Rect& dstRect );
	Point MapPt  ( Point pt, const Rect& srcRect, const Rect& dstRect );
	
	Rect MapRect( const Rect& r, const Rect& srcRect, const Rect& dstRect );
	*/
	
	// MapRgn
	// MapPoly
	// SetStdProcs
	// StdRect
	// StdRRect
	// StdOval
	// StdArc
	// StdPoly
	// StdRgn
	// StdBits
	// StdComment
	// StdGetPic
	// StdPutPic
	// StdOpcode
	
	Point AddPt( Point a, Point b );
	
	inline bool EqualPt( Point a, Point b )  { return ::EqualPt( a, b ); }
	
	inline
	bool MacPtInRect( Point pt, const Rect& rect )
	{
		return ::MacPtInRect( pt, &rect );
	}
	
	// Pt2Rect
	// PtToAngle
	
	Point SubPt( Point a, Point b );
	
	// PtInRgn
	// StdLine
	// OpenCPort
	// InitCPort
	// CloseCPort
	
//	nucleus::owned< PixMapHandle > NewPixMap();
	
//	inline void DisposePixMap( nucleus::owned< PixMapHandle > )  {}
	
	// CopyPixMap
	
	nucleus::owned< PixPatHandle > NewPixPat();
	
	inline void DisposePixPat( nucleus::owned< PixPatHandle > )  {}
	
	PixPatHandle CopyPixPat( PixPatHandle srcPP, PixPatHandle dstPP );
	
	nucleus::owned< PixPatHandle > CopyPixPat( PixPatHandle pixPat );
	
	using ::PenPixPat;
	using ::BackPixPat;
	
	PixPatHandle GetPixPat( Mac::ResID patID );
	
	// MakeRGBPat
	// FillCRect
	// FillCOval
	// FillCRoundRect
	// FillCArc
	// FillCRgn
	// FillCPoly
	
	inline void RGBForeColor( const RGBColor& rgb )  { ::RGBForeColor( &rgb ); }
	inline void RGBBackColor( const RGBColor& rgb )  { ::RGBBackColor( &rgb ); }
	
	inline void SetCPixel( short h, short v, const RGBColor& color )
	{
		::SetCPixel( h, v, &color );
	}
	
	using ::SetPortPix;
	
	inline
	RGBColor GetCPixel( short h, short v )
	{
		RGBColor result;
		::GetCPixel( h, v, &result );
		
		return result;
	}
	
	inline
	RGBColor GetForeColor()
	{
		RGBColor result;
		::GetForeColor( &result );
		
		return result;
	}
	
	inline
	RGBColor GetBackColor()
	{
		RGBColor result;
		::GetBackColor( &result );
		
		return result;
	}
	
	// SeedCFill
	// CalcCMask
	// OpenCPicture
	// OpColor
	// HiliteColor
	
	inline void DisposeCTable( nucleus::owned< CTabHandle > )  {}
	
	nucleus::owned< CTabHandle > GetCTable( short ctabID );
	
	// GetCTable
	// GetCCursor
	// SetCCursor
	
	using ::AllocCursor;
	
	inline void DisposeCCursor( nucleus::owned< CCrsrHandle > )  {}
	
	// SetStdCProcs
	// GetMaxDevice
	// GetCTSeed
	
	using ::GetDeviceList;
	using ::GetMainDevice;
	using ::GetNextDevice;
	
	// TestDeviceAttribute
	// SetDeviceAttribute
	// InitGDevice
	// NewGDevice
	
	inline void DisposeGDevice( nucleus::owned< GDHandle > )  {}
	
	using ::SetGDevice;
	using ::GetGDevice;
	
	// ...
	
	// Only returns errors from Color QuickDraw and Color Manager functions
	void QDError();
	
	// CopyDeepMask
	// DeviceLoop
	// GetMaskTable
	
	PatHandle GetPattern( Mac::ResID patternID );
	
	CursHandle MacGetCursor( Mac::ResID cursorID );
	
	PicHandle GetPicture( Mac::ResID pictureID );
	
	// DeltaPoint
	// ShieldCursor
	// ScreenRes
	
	Pattern GetIndPattern( Mac::ResID patternListID, short index );
	
	// PackBits
	// UnpackBits
	// SlopeFromAngle
	// AngleFromSlope
	
	// GetPortCustomXFerProc
	// SetPortCustomXFerProc
	
	// OpenCursorComponent
	// CloseCursorComponent
	// SetCursorComponent
	// CursorComponentChanged
	// CursorComponentSetData
	
	bool IsValidPort( CGrafPtr port );
	
	// GetPortPixMap
	// ...
	
	using ::GetPortBitMapForCopyBits;
	
	inline
	RGBColor GetPortForeColor( CGrafPtr port )
	{
		RGBColor result;
		::GetPortForeColor( port, &result );
		
		return result;
	}
	
	inline
	RGBColor GetPortBackColor( CGrafPtr port )
	{
		RGBColor result;
		::GetPortBackColor( port, &result );
		
		return result;
	}
	
	// ...
	
	inline
	RgnHandle GetPortVisibleRegion( CGrafPtr port, RgnHandle region )
	{
		return ::GetPortVisibleRegion( port, region );
	}
	
	nucleus::owned< RgnHandle > GetPortVisibleRegion( CGrafPtr port );
	
	inline
	RgnHandle GetPortClipRegion( CGrafPtr port, RgnHandle region )
	{
		return ::GetPortClipRegion( port, region );
	}
	
	nucleus::owned< RgnHandle > GetPortClipRegion( CGrafPtr port );
	
	// ...
	
	inline
	Point GetPortPenSize( CGrafPtr port )
	{
		Point result;
		(void)::GetPortPenSize( port, &result );
		
		return result;
	}
	
	// ...
	
	inline
	bool IsPortColor( CGrafPtr port )
	{
		return ::IsPortColor( port );
	}
	
	// ...
	
	nucleus::owned< CGrafPtr > CreateNewPort();
	
	inline void DisposePort( nucleus::owned< CGrafPtr > )  {}
	
	// SetQDError
	
	// ...
	
	// QDIsPortBuffered
	// QDIsPortBufferDirty
	// QDFlushPortBuffer
	// QDGetDirtyRegion
	// QDSetDirtyRegion
	// QDAddRectToDirtyRegion
	// QDAddRegionToDirtyRegion
	// CreateCGContextForPort
	// ClipCGContextToRegion
	// SyncCGContextOriginWithPort
	// QDBeginCGContext
	// QDEndCGContext
	// QDSaveRegionBits
	// QDRestoreRegionBits
	// QDDisposeRegionBits
	
	/*
		CreateNewPortForCGDisplayID() is declared in CGDirectDisplay.hh, since
		it requires CGDirectDisplayID.
	*/
	
	// QDDisplayWaitCursor
	// QDSetPatternOrigin
	// QDGetPatternOrigin
	
	// LowMem accessors
	
#endif  // #if ! __LP64__
	
}

namespace nucleus
{
	
#if ! __LP64__
	
	#pragma mark -
	#pragma mark ** Operators **
	
	inline Point operator-( Point pt )
	{
		return Nitrogen::SetPt( -pt.h,
		                        -pt.v );
	}
	
	inline Point operator+( Point a, Point b )
	{
		return Nitrogen::AddPt( a, b );
	}
	
	inline Point operator-( Point a, Point b )
	{
		return Nitrogen::SubPt( a, b );
	}
	
	template < class Factor >
	Point operator*( Point pt, Factor f )
	{
		return Nitrogen::SetPt( pt.h * f,
		                        pt.v * f );
	}
	
	template < class Divisor >
	Point operator/( Point pt, Divisor d )
	{
		return Nitrogen::SetPt( pt.h / d, 
		                        pt.v / d );
	}
	
	
	inline Rect operator+( Rect r, Point pt )
	{
		return Nitrogen::MacOffsetRect( r, pt );
	}
	
	inline Rect operator-( Rect r, Point pt )
	{
		return Nitrogen::MacOffsetRect( r, -pt );
	}
	
#endif  // #if ! __LP64__
	
	inline bool operator==( const Point& a, const Point& b )
	{
		return    a.v == b.v
		       && a.h == b.h;
	}
	
	inline bool operator!=( const Point& a, const Point& b )
	{
		return !( a == b );
	}
	
	inline bool operator==( const Rect& a, const Rect& b )
	{
		return    a.top    == b.top
		       && a.left   == b.left
		       && a.bottom == b.bottom
		       && a.right  == b.right;
	}
	
	inline bool operator!=( const Rect& a, const Rect& b )
	{
		return !( a == b );
	}
	
}

#include "nucleus/operators.hh"

#endif
