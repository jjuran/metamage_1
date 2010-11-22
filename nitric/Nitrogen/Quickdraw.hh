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

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
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
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif
#ifndef NUCLEUS_SAVED_HH
#include "nucleus/saved.hh"
#endif

// Nitrogen
#ifndef MAC_QUICKDRAW_UTILITIES_PORTDISPOSER_HH
#include "Mac/Quickdraw/Utilities/Port_Disposer.hh"
#endif
#ifndef MAC_RESOURCES_TYPES_RESID_HH
#include "Mac/Resources/Types/ResID.hh"
#endif

#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif
#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif

namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( QuickDraw );
	
	
	// Constants of types defined elsewhere
	
	static const Mac::ResID sysPatListID = Mac::ResID( ::sysPatListID );
	
	static const Mac::ResID iBeamCursor = Mac::ResID( ::iBeamCursor );
	static const Mac::ResID crossCursor = Mac::ResID( ::crossCursor );
	static const Mac::ResID plusCursor  = Mac::ResID( ::plusCursor  );
	static const Mac::ResID watchCursor = Mac::ResID( ::watchCursor );
	
	#pragma mark -
	#pragma mark ** Types **
	
	enum TransferMode
	{
		srcCopy = ::srcCopy,
		
		kTransferMode_Max = nucleus::enumeration_traits< SInt16 >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( TransferMode )
	
	enum PixelType
	{
		kPixelType_Max = nucleus::enumeration_traits< ::PixelType >::max
	};
	
	using ::BitMap;
	using ::BitMapPtr;
	using ::BitMapHandle;
	
	using ::Cursor;
	using ::CursPtr;
	using ::CursHandle;
	
	using ::PenState;
	
	using ::RgnHandle;
	using ::PicHandle;
	using ::PolyHandle;
	
}

namespace nucleus
{
	
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
}

namespace Nitrogen
{
	
	using ::GrafPtr;
	using ::WindowPtr;
	using ::DialogPtr;
	
	using ::WindowRef;
	
	using ::RGBColor;
	
	using ::CTabHandle;
	
}

namespace nucleus
{
	
	template <> struct disposer< CTabHandle >
	{
		typedef CTabHandle  argument_type;
		typedef void        result_type;
		
		void operator()( CTabHandle h ) const
		{
			::DisposeCTable( h );
		}
	};
}

namespace Nitrogen
{
	
	using ::PixMap;
	using ::PixMapPtr;
	using ::PixMapHandle;
	using ::PixPatHandle;
	using ::CCrsrHandle;
	using ::GDHandle;
	using ::CGrafPtr;
	
}

namespace nucleus
{
	
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
	
	GrafPtr GetPort();
	
	inline bool QDSwapPort( CGrafPtr newPort, CGrafPtr& oldPort )  { return ::QDSwapPort( newPort, reinterpret_cast< ::CGrafPtr* >( &oldPort ) ); }
	
	CGrafPtr QDSwapPort( CGrafPtr newPort );
	
#if !OPAQUE_TOOLBOX_STRUCTS
	
	inline GrafPtr QDSwapPort( GrafPtr newPort )
	{
		return reinterpret_cast< GrafPtr >( QDSwapPort( reinterpret_cast< CGrafPtr >( newPort ) ) );
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
	
	inline void MacSetCursor( const Cursor& cursor )  { ::MacSetCursor( &cursor ); }
	inline void MacSetCursor( CursHandle    cursor )  { ::MacSetCursor( *cursor ); }
	void MacSetCursor( Mac::ResID id );  // calls GetCursor()
	
	using ::HideCursor;
	using ::MacShowCursor;
	using ::ObscureCursor;
	using ::HidePen;
	using ::ShowPen;
	
	Point GetPen();
	
	PenState GetPenState();
	
	inline void SetPenState( const PenState& penState )  { ::SetPenState( &penState ); }
	
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
	
	inline Rect MacSetRect( short left, short top, short right, short bottom )  { return nucleus::make< Rect >( top, left, bottom, right ); }
	inline Rect MacSetRect( Point topLeft, Point bottomRight )  { return MacSetRect( topLeft.h, topLeft.v, bottomRight.h, bottomRight.v ); }
	
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
	
	Rect MacUnionRect( const Rect& a, const Rect& b );
	
	bool MacEqualRect( const Rect& a, const Rect& b );
	
	bool EmptyRect( const Rect& r );
	
	inline void MacFrameRect ( const Rect& r )  { ::MacFrameRect ( &r ); }
	inline void PaintRect    ( const Rect& r )  { ::PaintRect    ( &r ); }
	inline void EraseRect    ( const Rect& r )  { ::EraseRect    ( &r ); }
	inline void MacInvertRect( const Rect& r )  { ::MacInvertRect( &r ); }
	
	inline void MacFillRect( const Rect&     r,
	                         const Pattern&  pat )  { ::MacFillRect( &r, &pat ); }
	
	inline void FrameOval ( const Rect& r )  { ::FrameOval ( &r ); }
	inline void PaintOval ( const Rect& r )  { ::PaintOval ( &r ); }
	inline void EraseOval ( const Rect& r )  { ::EraseOval ( &r ); }
	inline void InvertOval( const Rect& r )  { ::InvertOval( &r ); }
	
	inline void FillOval( const Rect&     r,
	                      const Pattern&  pat )  { ::FillOval( &r, &pat ); }
	
	// RoundRect
	// Arc
	
//	Region handling
	nucleus::owned< RgnHandle > NewRgn();
	
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
	
	RgnHandle RectRgn( RgnHandle region, const Rect& rect );	
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
	
	inline bool RectInRgn( const Rect& r, RgnHandle rgn )  { return ::RectInRgn( &r, rgn ); }
	
	inline bool MacEqualRgn( RgnHandle a, RgnHandle b )  { return ::MacEqualRgn( a, b ); }
	
	inline bool EmptyRgn( RgnHandle rgn )  { return ::EmptyRgn( rgn ); }
	
	using ::MacFrameRgn;
	using ::MacPaintRgn;
	using ::EraseRgn;
	using ::MacInvertRgn;
	
	inline void MacFillRgn( RgnHandle rgn, const Pattern& pat )  { ::MacFillRgn( rgn, &pat ); }
	
	void ScrollRect( const Rect& r, short dh, short dv, RgnHandle updateRgn );
	
	nucleus::owned< RgnHandle > ScrollRect( const Rect& r, short dh, short dv );
	
	void CopyBits( const BitMap*  srcBits,
	               const BitMap*  dstBits,
	               const Rect&    srcRect,
	               const Rect&    dstRect,
	               TransferMode   mode,
	               RgnHandle      maskRgn = NULL );
	
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
	
	inline void DrawPicture( PicHandle pic, const Rect& r )  { ::DrawPicture( pic, &r ); }
	
	inline void KillPicture( nucleus::owned< PicHandle > )  {}
	
	nucleus::owned< PolyHandle > OpenPoly();
	
	using ::ClosePoly;
	
	inline void KillPoly( nucleus::owned< PolyHandle > )  {}
	
	using ::OffsetPoly;
	using ::FramePoly;
	using ::PaintPoly;
	using ::ErasePoly;
	using ::InvertPoly;
	
	inline void FillPoly( PolyHandle poly, const Pattern& pat )  { ::FillPoly( poly, &pat ); }
	
	inline Point SetPt( short h, short v )  { return nucleus::make< Point >( v, h ); }
	
	Point LocalToGlobal( Point        point );
	Rect  LocalToGlobal( const Rect&  rect );
	
	Point GlobalToLocal( Point        point );
	Rect  GlobalToLocal( const Rect&  rect );
	
	using ::Random;
	
	// StuffHex
	
	inline bool MacGetPixel( short h, short v )  { return ::MacGetPixel( h, v ); }
	
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
	
	inline bool MacPtInRect( Point pt, const Rect& rect )  { return ::MacPtInRect( pt, &rect ); }
	
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
	
	inline void RGBForeColor( const RGBColor& color )  { ::RGBForeColor( &color ); }
	inline void RGBBackColor( const RGBColor& color )  { ::RGBBackColor( &color ); }
	
	inline void SetCPixel( short h, short v, const RGBColor& color )
	{
		::SetCPixel( h, v, &color );
	}
	
	using ::SetPortPix;
	
	RGBColor GetCPixel( short h, short v );
	
	RGBColor GetForeColor();
	RGBColor GetBackColor();
	
	// SeedCFill
	// CalcCMask
	// OpenCPicture
	// OpColor
	// HiliteColor
	
	inline void DisposeCTable( nucleus::owned< CTabHandle > )  {}
	
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
	
	const BitMap* GetPortBitMapForCopyBits( CGrafPtr port );
	
	Rect GetPortBounds( CGrafPtr port );
	
	RGBColor GetPortForeColor( CGrafPtr port );
	RGBColor GetPortBackColor( CGrafPtr port );
	
	// ...
	
	RgnHandle GetPortVisibleRegion( CGrafPtr port, RgnHandle region );
	
	nucleus::owned< RgnHandle > GetPortVisibleRegion( CGrafPtr port );
	
	RgnHandle GetPortClipRegion( CGrafPtr port, RgnHandle region );
	
	nucleus::owned< RgnHandle > GetPortClipRegion( CGrafPtr port );
	
	// ...
	
	Point GetPortPenSize( CGrafPtr );
	
	// ...
	
	bool IsPortColor( CGrafPtr port );
	
	// ...
	
	void SetPortClipRegion( CGrafPtr port, RgnHandle clipRgn );
	
	// ...
	
	void SetPortPenSize( CGrafPtr port, Point penSize );
	
	// ...
	
	// GetQDGlobalsRandomSeed
	
#if ACCESSOR_CALLS_ARE_FUNCTIONS
	
	const BitMap&  GetQDGlobalsScreenBits();
	const Cursor&  GetQDGlobalsArrow();
	const Pattern& GetQDGlobalsDarkGray();
	const Pattern& GetQDGlobalsLightGray();
	const Pattern& GetQDGlobalsGray();
	const Pattern& GetQDGlobalsBlack();
	const Pattern& GetQDGlobalsWhite();
	
#else
	
	inline const BitMap&  GetQDGlobalsScreenBits()  { return qd.screenBits; }
	inline const Cursor&  GetQDGlobalsArrow()       { return qd.arrow; }
	inline const Pattern& GetQDGlobalsDarkGray()    { return qd.dkGray; }
	inline const Pattern& GetQDGlobalsLightGray()   { return qd.ltGray; }
	inline const Pattern& GetQDGlobalsGray()        { return qd.gray; }
	inline const Pattern& GetQDGlobalsBlack()       { return qd.black; }
	inline const Pattern& GetQDGlobalsWhite()       { return qd.white; }
	
#endif
	
	using ::GetQDGlobalsThePort;
	
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
	
	// Declared in Nitrogen/CGDirectDisplay.hh since it requires CGDirectDisplayID.
	//nucleus::owned< CGrafPtr > CreateNewPortForCGDisplayID( CGDirectDisplayID display );
	
	// QDDisplayWaitCursor
	// QDSetPatternOrigin
	// QDGetPatternOrigin
	
	// LowMem accessors
	
	#pragma mark -
	#pragma mark ** Pseudoreferences **
	
	class Port
	{
		public:
			typedef GrafPtr  value_type;
			typedef GrafPtr  param_type;
			
			value_type get() const             { return GetPort();          }
			void set( param_type port ) const  { Nitrogen::SetPort( port ); }
	};
	
	class Clip
	{
		public:
			typedef nucleus::owned< RgnHandle >  value_type;
			typedef                 RgnHandle    param_type;
			
			value_type get() const               { return GetClip();  }
			void set( param_type region ) const  { SetClip( region ); }
	};
	
	class Pen_State
	{
		public:
			typedef PenState         value_type;
			typedef PenState const&  param_type;
			
			value_type get() const              { return GetPenState(); }
			void set( param_type state ) const  { SetPenState( state ); }
	};
	
	class RGBForeColor_Setting
	{
		public:
			typedef RGBColor         value_type;
			typedef RGBColor const&  param_type;
			
			value_type get() const              { return GetForeColor(); }
			void set( param_type color ) const  { RGBForeColor( color ); }
	};
	
	class RGBBackColor_Setting
	{
		public:
			typedef RGBColor         value_type;
			typedef RGBColor const&  param_type;
			
			value_type get() const              { return GetBackColor(); }
			void set( param_type color ) const  { RGBBackColor( color ); }
	};
	
	class Port_ClipRegion
	{
		private:
			CGrafPtr port;
			
		public:
			typedef nucleus::owned< RgnHandle >  value_type;
			typedef                 RgnHandle    param_type;
			
			Port_ClipRegion( CGrafPtr port )     : port( port ) {}
			
			value_type get() const               { return GetPortClipRegion( port );            }
			void set( param_type region ) const  { Nitrogen::SetPortClipRegion( port, region ); }
	};
	
	class Port_PenSize
	{
		private:
			CGrafPtr port;
      
		public:
			typedef Point value_type;
			typedef Point param_type;
			
			Port_PenSize( CGrafPtr port )      : port( port ) {}
			
			value_type get() const             { return GetPortPenSize( port );          }
			void set( param_type size ) const  { Nitrogen::SetPortPenSize( port, size ); }
	};
	
}

namespace nucleus
{
	
	template <>
	struct swap_with_saved< Nitrogen::Port >
	{
		static GrafPtr apply( Nitrogen::Port, GrafPtr port )
		{
			return Nitrogen::QDSwapPort( port );
		}
	};
	
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
