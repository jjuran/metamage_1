// Nitrogen/QuickDraw.h
// --------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NITROGEN_QUICKDRAW_H
#define NITROGEN_QUICKDRAW_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __QUICKDRAW__
#include FRAMEWORK_HEADER(QD,QuickDraw.h)
#endif
#ifndef NUCLEUS_PSEUDOREFERENCE_H
#include "Nucleus/Pseudoreference.h"
#endif

#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_RESOURCES_H
#include "Nitrogen/Resources.h"
#endif

namespace Nitrogen
{
	
	class QuickDrawErrorsRegistrationDependency
	{
		public:
			QuickDrawErrorsRegistrationDependency();
	};
	
	
	// Constants of types defined elsewhere
	
	static const ResID sysPatListID = ResID( ::sysPatListID );
	
	static const ResID iBeamCursor = ResID( ::iBeamCursor );
	static const ResID crossCursor = ResID( ::crossCursor );
	static const ResID plusCursor  = ResID( ::plusCursor  );
	static const ResID watchCursor = ResID( ::watchCursor );
	
	#pragma mark -
	#pragma mark ¥ Types ¥
	
	typedef Nucleus::Flag< class TransferMode_Tag, ::SInt16 >::Type TransferMode;
	
	static const TransferMode srcCopy = TransferMode( ::srcCopy );
	
	NUCLEUS_DEFINE_FLAG_OPS( TransferMode )
	
	typedef Nucleus::Selector< class PixelType_Tag, ::PixelType >::Type PixelType;
	
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

namespace Nucleus
{
	
	template <> struct Disposer< Nitrogen::RgnHandle > : public std::unary_function< Nitrogen::RgnHandle, void >
	{
		void operator()( Nitrogen::RgnHandle h ) const
		{
			::DisposeRgn( h );
		}
	};
	
	template <> struct Disposer< Nitrogen::PicHandle > : public std::unary_function< Nitrogen::PicHandle, void >
	{
		void operator()( Nitrogen::PicHandle h ) const
		{
			::KillPicture( h );
		}
	};
	
	template <> struct Disposer< Nitrogen::PolyHandle > : public std::unary_function< Nitrogen::PolyHandle, void >
	{
		void operator()( Nitrogen::PolyHandle h ) const
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

namespace Nucleus
{
	
	template <> struct Disposer< Nitrogen::CTabHandle > : public std::unary_function< Nitrogen::CTabHandle, void >
	{
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
	
	namespace Detail
	{
		
		template < class Port >
		class PortDisposer : public std::unary_function< Port, void >
		{
			private:
				typedef pascal void (*Dispose)( Port );
				
				Dispose dispose;
			
			public:
				PortDisposer() : dispose( NULL )  {}
				PortDisposer( Dispose d ) : dispose( d )  {}
				
				void operator()( Port port ) const
				{
					if ( dispose )  dispose( port );
				}
		};
		
	}
	
}

namespace Nucleus
{
	
	/*
	template <> struct Disposer< PixMapHandle > : public std::unary_function< PixMapHandle, void >
	{
		void operator()( PixMapHandle h ) const
		{
			::DisposePixMap( h );
		}
	};
	*/
	
	
	template <> struct Disposer< Nitrogen::PixPatHandle > : public std::unary_function< Nitrogen::PixPatHandle, void >
	{
		void operator()( Nitrogen::PixPatHandle h ) const
		{
			::DisposePixPat( h );
		}
	};
	
	template <> struct Disposer< Nitrogen::CCrsrHandle > : public std::unary_function< Nitrogen::CCrsrHandle, void >
	{
		void operator()( Nitrogen::CCrsrHandle h ) const
		{
			::DisposeCCursor( h );
		}
	};
	
	
	template <> struct Disposer< Nitrogen::GDHandle > : public std::unary_function< Nitrogen::GDHandle, void >
	{
		void operator()( Nitrogen::GDHandle h ) const
		{
			::DisposeGDevice( h );
		}
	};
	
	
	template <> struct OwnedDefaults< Nitrogen::CGrafPtr >
	{
		typedef Nitrogen::Detail::PortDisposer< Nitrogen::CGrafPtr > DisposerType;
	};
	
#if !OPAQUE_TOOLBOX_STRUCTS
	
	template <> struct OwnedDefaults< Nitrogen::GrafPtr >
	{
		typedef Nitrogen::Detail::PortDisposer< Nitrogen::GrafPtr > DisposerType;
	};
	
#endif
	
	template <>
	struct Maker< Nitrogen::RGBColor >
	{
		Nitrogen::RGBColor operator()( unsigned short  red,
		                               unsigned short  green,
		                               unsigned short  blue ) const
		{
			Nitrogen::RGBColor result;
			
			result.red   = red;
			result.green = green;
			result.blue  = blue;
			
			return result;
		}
		
		Nitrogen::RGBColor operator()( unsigned short gray ) const
		{
			return operator()( gray, gray, gray );
		}
		
		Nitrogen::RGBColor operator()() const
		{
			return operator()( 0, 0, 0 );
		}
	};
	
}

namespace Nitrogen
{
	
	#pragma mark -
	#pragma mark ¥ Routines ¥
	
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
	
	Nucleus::Owned< RgnHandle > GetClip();
	
	inline void ClipRect( const Rect& rect )  { ::ClipRect( &rect ); }
	
	inline void BackPat( const Pattern& pat )  { ::BackPat( &pat ); }
	
	using ::InitCursor;
	
	inline void MacSetCursor( const Cursor& cursor )  { ::MacSetCursor( &cursor ); }
	inline void MacSetCursor( CursHandle    cursor )  { ::MacSetCursor( *cursor ); }
	void MacSetCursor( ResID id );  // calls GetCursor()
	
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
	
	inline Rect MacSetRect( short left, short top, short right, short bottom )  { return Nucleus::Make< Rect >( top, left, bottom, right ); }
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
	Nucleus::Owned< RgnHandle > NewRgn();
	
//	extern void OpenRgn(void)
	using ::OpenRgn;

//	extern void CloseRgn(RgnHandle dstRgn)
	using ::CloseRgn;
	
	Nucleus::Owned< RgnHandle > CloseRgn();
	
	RgnHandle BitMapToRegion( RgnHandle region, const BitMap& bitMap );
	Nucleus::Owned< RgnHandle > BitMapToRegion( const BitMap& bitMap );
	
	RgnHandle HandleToRgn( RgnHandle region, Handle oldRegion );
	Nucleus::Owned< RgnHandle > HandleToRgn( Handle oldRegion );
	
	Nucleus::Owned< Handle > RgnToHandle( RgnHandle region );
	
	inline void DisposeRgn( Nucleus::Owned< RgnHandle > )  {}
	
//	extern void MacCopyRgn ( RgnHandle srcRgn, RgnHandle dstRgn );
	using ::MacCopyRgn;
	
	Nucleus::Owned< RgnHandle > MacCopyRgn( RgnHandle srcRgn );
	
//	extern void SetEmptyRgn(RgnHandle rgn);
	using ::SetEmptyRgn;
	
	RgnHandle MacSetRectRgn( RgnHandle  region,
	                         short      left,
	                         short      top,
	                         short      right,
	                         short      bottom );	
	
	Nucleus::Owned< RgnHandle > MacSetRectRgn( short  left,
	                                           short  top,
	                                           short  right,
	                                           short  bottom );
	
	RgnHandle RectRgn( RgnHandle region, const Rect& rect );	
	Nucleus::Owned< RgnHandle > RectRgn( const Rect& rect );
	
	using ::MacOffsetRgn;
	using ::InsetRgn;
	
	using ::SectRgn;
	Nucleus::Owned< RgnHandle > SectRgn( RgnHandle a, RgnHandle b );
	
	using ::MacUnionRgn;
	Nucleus::Owned< RgnHandle > MacUnionRgn( RgnHandle a, RgnHandle b );
	
	using ::DiffRgn;
	Nucleus::Owned< RgnHandle > DiffRgn( RgnHandle a, RgnHandle b );
	
	using ::MacXorRgn;
	Nucleus::Owned< RgnHandle > MacXorRgn( RgnHandle a, RgnHandle b );
	
	inline bool RectInRgn( const Rect& r, RgnHandle rgn )  { return ::RectInRgn( &r, rgn ); }
	
	inline bool MacEqualRgn( RgnHandle a, RgnHandle b )  { return ::MacEqualRgn( a, b ); }
	
	inline bool EmptyRgn( RgnHandle rgn )  { return ::EmptyRgn( rgn ); }
	
	using ::MacFrameRgn;
	using ::MacPaintRgn;
	using ::EraseRgn;
	using ::MacInvertRgn;
	
	inline void MacFillRgn( RgnHandle rgn, const Pattern& pat )  { ::MacFillRgn( rgn, &pat ); }
	
	void ScrollRect( const Rect& r, short dh, short dv, RgnHandle updateRgn );
	
	Nucleus::Owned< RgnHandle > ScrollRect( const Rect& r, short dh, short dv );
	
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
	
	Nucleus::Owned< PicHandle > OpenPicture( const Rect& picFrame );
	
	// PicComment
	
	using ::ClosePicture;
	
	inline void DrawPicture( PicHandle pic, const Rect& r )  { ::DrawPicture( pic, &r ); }
	
	inline void KillPicture( Nucleus::Owned< PicHandle > )  {}
	
	Nucleus::Owned< PolyHandle > OpenPoly();
	
	using ::ClosePoly;
	
	inline void KillPoly( Nucleus::Owned< PolyHandle > )  {}
	
	using ::OffsetPoly;
	using ::FramePoly;
	using ::PaintPoly;
	using ::ErasePoly;
	using ::InvertPoly;
	
	inline void FillPoly( PolyHandle poly, const Pattern& pat )  { ::FillPoly( poly, &pat ); }
	
	inline Point SetPt( short h, short v )  { return Nucleus::Make< Point >( v, h ); }
	
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
	
//	Nucleus::Owned< PixMapHandle > NewPixMap();
	
//	inline void DisposePixMap( Nucleus::Owned< PixMapHandle > )  {}
	
	// CopyPixMap
	
	Nucleus::Owned< PixPatHandle > NewPixPat();
	
	inline void DisposePixPat( Nucleus::Owned< PixPatHandle > )  {}
	
	PixPatHandle CopyPixPat( PixPatHandle srcPP, PixPatHandle dstPP );
	
	Nucleus::Owned< PixPatHandle > CopyPixPat( PixPatHandle pixPat );
	
	using ::PenPixPat;
	using ::BackPixPat;
	
	PixPatHandle GetPixPat( ResID patID );
	
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
	
	inline void DisposeCTable( Nucleus::Owned< CTabHandle > )  {}
	
	// GetCTable
	// GetCCursor
	// SetCCursor
	
	using ::AllocCursor;
	
	inline void DisposeCCursor( Nucleus::Owned< CCrsrHandle > )  {}
	
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
	
	inline void DisposeGDevice( Nucleus::Owned< GDHandle > )  {}
	
	using ::SetGDevice;
	using ::GetGDevice;
	
	// ...
	
	// Only returns errors from Color QuickDraw and Color Manager functions
	void QDError();
	
	// CopyDeepMask
	// DeviceLoop
	// GetMaskTable
	
	PatHandle GetPattern( ResID patternID );
	
	CursHandle MacGetCursor( ResID cursorID );
	
	PicHandle GetPicture( ResID pictureID );
	
	// DeltaPoint
	// ShieldCursor
	// ScreenRes
	
	Pattern GetIndPattern( ResID patternListID, short index );
	
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
	
	Nucleus::Owned< RgnHandle > GetPortVisibleRegion( CGrafPtr port );
	
	RgnHandle GetPortClipRegion( CGrafPtr port, RgnHandle region );
	
	Nucleus::Owned< RgnHandle > GetPortClipRegion( CGrafPtr port );
	
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
	
	Nucleus::Owned< CGrafPtr > CreateNewPort();
	
	inline void DisposePort( Nucleus::Owned< CGrafPtr > )  {}
	
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
	
	// Declared in Nitrogen/CGDirectDisplay.h since it requires CGDirectDisplayID.
	//Nucleus::Owned< CGrafPtr > CreateNewPortForCGDisplayID( CGDirectDisplayID display );
	
	// QDDisplayWaitCursor
	// QDSetPatternOrigin
	// QDGetPatternOrigin
	
	// LowMem accessors
	
	#pragma mark -
	#pragma mark ¥ Pseudoreferences ¥
	
	class Port_Value
	{
		public:
			typedef GrafPtr Value;
			typedef GrafPtr GetResult;
			typedef GrafPtr SetParameter;
			
			static const bool hasSwap = true;
			
			GetResult Get() const                      { return GetPort();          }
			void Set( SetParameter port ) const        { Nitrogen::SetPort( port ); }
			
			GetResult Swap( SetParameter port ) const  { return QDSwapPort( port ); }
	};
	
	class Clip_Value
	{
		public:
			typedef Nucleus::Owned< RgnHandle > Value;
			typedef Nucleus::Owned< RgnHandle > GetResult;
			typedef        RgnHandle   SetParameter;
			
			static const bool hasSwap = false;
			
			GetResult Get() const                  { return GetClip();  }
			void Set( SetParameter region ) const  { SetClip( region ); }
	};
	
	class PenState_Value
	{
		public:
			typedef PenState         Value;
			typedef PenState         GetResult;
			typedef PenState const&  SetParameter;
			
			static const bool hasSwap = false;
			
			GetResult Get() const                 { return GetPenState(); }
			void Set( SetParameter state ) const  { SetPenState( state ); }
	};
	
	class RGBForeColor_Value
	{
		public:
			typedef RGBColor         Value;
			typedef RGBColor         GetResult;
			typedef RGBColor const&  SetParameter;
			
			static const bool hasSwap = false;
			
			GetResult Get() const                 { return GetForeColor(); }
			void Set( SetParameter color ) const  { RGBForeColor( color ); }
	};
	
	class RGBBackColor_Value
	{
		public:
			typedef RGBColor         Value;
			typedef RGBColor         GetResult;
			typedef RGBColor const&  SetParameter;
			
			static const bool hasSwap = false;
			
			GetResult Get() const                 { return GetBackColor(); }
			void Set( SetParameter color ) const  { RGBBackColor( color ); }
	};
	
	class PortClipRegion_Value
	{
		private:
			CGrafPtr port;
			
		public:
			typedef Nucleus::Owned< RgnHandle > Value;
			typedef Nucleus::Owned< RgnHandle > GetResult;
			typedef        RgnHandle   SetParameter;
			
			static const bool hasSwap = false;
			
			PortClipRegion_Value( CGrafPtr port )    : port( port ) {}
			
			GetResult Get() const                    { return GetPortClipRegion( port );            }
			void Set( SetParameter region ) const    { Nitrogen::SetPortClipRegion( port, region ); }
	};
	
	class PortPenSize_Value
	{
		private:
			CGrafPtr port;
      
		public:
			typedef Point Value;
			typedef Point GetResult;
			typedef Point SetParameter;
			
			static const bool hasSwap = false;
			
			PortPenSize_Value( CGrafPtr port )    : port( port ) {}
			
			GetResult Get() const                 { return GetPortPenSize( port );          }
			void Set( SetParameter size ) const   { Nitrogen::SetPortPenSize( port, size ); }
	};
	
	typedef Port_Value            Port_Details;
	typedef Clip_Value            Clip_Details;
	typedef PenState_Value        PenState_Details;
	typedef RGBForeColor_Value    RGBForeColor_Details;
	typedef RGBBackColor_Value    RGBBackColor_Details;
	typedef PortClipRegion_Value  PortClipRegion_Details;
	typedef PortPenSize_Value     PortPenSize_Details;
	
	typedef Nucleus::Pseudoreference< Port_Details           > Port;
	typedef Nucleus::Pseudoreference< Clip_Details           > Clip;
	typedef Nucleus::Pseudoreference< PenState_Details       > The_PenState;
	typedef Nucleus::Pseudoreference< RGBForeColor_Details   > The_RGBForeColor;
	typedef Nucleus::Pseudoreference< RGBBackColor_Details   > The_RGBBackColor;
	typedef Nucleus::Pseudoreference< PortClipRegion_Details > PortClipRegion;
	typedef Nucleus::Pseudoreference< PortPenSize_Details    > PortPenSize;
	
}

namespace Nucleus
{
	
	#pragma mark -
	#pragma mark ¥ Operators ¥
	
	inline Nitrogen::Point operator-( Nitrogen::Point pt )
	{
		return Nitrogen::SetPt( -pt.h,
		                        -pt.v );
	}
	
	inline Nitrogen::Point operator+( Nitrogen::Point a, Nitrogen::Point b )
	{
		return Nitrogen::AddPt( a, b );
	}
	
	inline Nitrogen::Point operator-( Nitrogen::Point a, Nitrogen::Point b )
	{
		return Nitrogen::SubPt( a, b );
	}
	
	template < class Factor >
	Nitrogen::Point operator*( Nitrogen::Point pt, Factor f )
	{
		return Nitrogen::SetPt( pt.h * f,
		                        pt.v * f );
	}
	
	template < class Divisor >
	Nitrogen::Point operator/( Nitrogen::Point pt, Divisor d )
	{
		return Nitrogen::SetPt( pt.h / d, 
		                        pt.v / d );
	}
	
	
	inline Nitrogen::Rect operator+( Nitrogen::Rect r, Nitrogen::Point pt )
	{
		return Nitrogen::MacOffsetRect( r, pt );
	}
	
	inline Nitrogen::Rect operator-( Nitrogen::Rect r, Nitrogen::Point pt )
	{
		return Nitrogen::MacOffsetRect( r, -pt );
	}
	
}

#include "Nucleus/Operators.h"

#endif
