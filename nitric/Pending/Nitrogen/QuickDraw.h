// QuickDraw.h

#ifndef NITROGEN_QUICKDRAW_H
#define NITROGEN_QUICKDRAW_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __QUICKDRAW__
#include FRAMEWORK_HEADER(QD,QuickDraw.h)
#endif
#ifndef NITROGEN_PSEUDOREFERENCE_H
#include "Nitrogen/Pseudoreference.h"
#endif

#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif
#ifndef NITROGEN_RESOURCES_H
#include "Nitrogen/Resources.h"
#endif

namespace Nitrogen
  {
	
	#pragma mark -
	#pragma mark ¥ Transfer modes ¥
	
	class TransferMode_Tag {};
	typedef FlagType< TransferMode_Tag, ::SInt16 > TransferMode;
	
	inline TransferMode SrcCopy()             { return srcCopy;       }
	inline TransferMode SrcOr()               { return srcOr;         }
	inline TransferMode SrcXor()              { return srcXor;        }
	inline TransferMode SrcBic()              { return srcBic;        }
	inline TransferMode NotSrcCopy()          { return notSrcCopy;    }
	inline TransferMode NotSrcOr()            { return notSrcOr;      }
	inline TransferMode NotSrcXor()           { return notSrcXor;     }
	inline TransferMode NotSrcBic()           { return notSrcBic;     }
	
	inline TransferMode PatCopy()             { return patCopy;       }
	inline TransferMode PatOr()               { return patOr;         }
	inline TransferMode PatXor()              { return patXor;        }
	inline TransferMode PatBic()              { return patBic;        }
	inline TransferMode NotPatCopy()          { return notPatCopy;    }
	inline TransferMode NotPatOr()            { return notPatOr;      }
	inline TransferMode NotPatXor()           { return notPatXor;     }
	inline TransferMode NotPatBic()           { return notPatBic;     }
	
	inline TransferMode GrayishTextOr()       { return grayishTextOr; }
	inline TransferMode Hilite()              { return hilite;        }
	inline TransferMode Hilitetransfermode()  { return hilite;        }  // dup
	
	inline TransferMode Blend()               { return blend;         }
	inline TransferMode AddPin()              { return addPin;        }
	inline TransferMode AddOver()             { return addOver;       }
	inline TransferMode SubPin()              { return subPin;        }
	inline TransferMode AddMax()              { return addMax;        }
	inline TransferMode AdMax()               { return addMax;        }  // dup
	inline TransferMode SubOver()             { return subOver;       }
	inline TransferMode AdMin()               { return adMin;         }
	inline TransferMode DitherCopy()          { return ditherCopy;    }
	inline TransferMode Transparent()         { return transparent;   }
	
	#pragma mark -
	#pragma mark ¥ Cursors ¥
	
	inline ResID IBeamCursor()  { return ResID::Make( iBeamCursor ); }
	inline ResID CrossCursor()  { return ResID::Make( crossCursor ); }
	inline ResID PlusCursor()   { return ResID::Make( plusCursor );  }
	inline ResID WatchCursor()  { return ResID::Make( watchCursor ); }
	
	#pragma mark -
	#pragma mark ¥ Pixel types ¥
	
	class PixelType_Tag {};
	typedef SelectorType< PixelType_Tag, ::PixelType > PixelType;
	
	inline PixelType Chunky()        { return PixelType( chunky ); }
	inline PixelType ChunkyPlanar()  { return PixelType( chunkyPlanar ); }
	inline PixelType Planar()        { return PixelType( planar ); }
	
	#pragma mark -
	#pragma mark ¥ Types ¥
	
	using ::BitMap;
	using ::BitMapPtr;
	using ::BitMapHandle;
	
	using ::Cursor;
	using ::CursPtr;
	using ::CursHandle;
	
	using ::PenState;
	
	using ::RgnHandle;
	
  template <> struct Disposer< RgnHandle >: public std::unary_function< RgnHandle, void >
     {
      void operator()( RgnHandle h ) const
        {
         ::DisposeRgn( h );
        }
     };

#if OPAQUE_TOOLBOX_STRUCTS
	
	using ::GrafPtr;
	using ::WindowPtr;
	using ::DialogPtr;
	
#else
	
	class GrafPtr
	{
		private:
			typedef ::GrafPtr UnderlyingType;
			UnderlyingType value;
			
			::GrafPtr  GetGrafPtr()  const                         { return value; }
			::CGrafPtr GetCGrafPtr() const                         { return reinterpret_cast< ::CGrafPtr >( value ); }
			
		public:
			GrafPtr() : value(0)  {}
			GrafPtr( ::GrafPtr  value ) : value( value )  {}
			GrafPtr( ::CGrafPtr value ) : value( reinterpret_cast< ::GrafPtr >( value ) )  {}
			
			operator ::GrafPtr()  const                            { return GetGrafPtr();  }
			operator ::CGrafPtr() const                            { return GetCGrafPtr(); }
			
			static GrafPtr Make( ::GrafPtr  v )  { return GrafPtr( v ); }
			static GrafPtr Make( ::CGrafPtr v )  { return GrafPtr( v ); }
			
			template < class T > T Get() const;
			
			friend bool operator==( GrafPtr a, GrafPtr b )     { return a.value == b.value; }
			friend bool operator==( GrafPtr a, ::GrafPtr b )   { return a.Get< ::GrafPtr >() == b; }
			friend bool operator==( ::GrafPtr a, GrafPtr b )   { return a == b.Get< ::GrafPtr >(); }
			friend bool operator==( GrafPtr a, ::CGrafPtr b )  { return a.Get< ::CGrafPtr >() == b; }
			friend bool operator==( ::CGrafPtr a, GrafPtr b )  { return a == b.Get< ::CGrafPtr >(); }
			
			friend bool operator!=( GrafPtr a, GrafPtr b )     { return a.value != b.value; }
			friend bool operator!=( GrafPtr a, ::GrafPtr b )   { return a.Get< ::GrafPtr >() != b; }
			friend bool operator!=( ::GrafPtr a, GrafPtr b )   { return a != b.Get< ::GrafPtr >(); }
			friend bool operator!=( GrafPtr a, ::CGrafPtr b )  { return a.Get< ::CGrafPtr >() != b; }
			friend bool operator!=( ::CGrafPtr a, GrafPtr b )  { return a != b.Get< ::CGrafPtr >(); }
	};
	
	template <> inline ::GrafPtr  GrafPtr::Get< ::GrafPtr  >() const  { return GetGrafPtr();  }
	template <> inline ::CGrafPtr GrafPtr::Get< ::CGrafPtr >() const  { return GetCGrafPtr(); }
	
	class WindowPtr
	{
		private:
			typedef ::WindowPtr UnderlyingType;
			UnderlyingType value;
			
		public:
			WindowPtr() : value( NULL )  {}
			WindowPtr( ::WindowPtr  value ) : value( value )  {}
			
			operator ::WindowPtr()  const                          { return value;  }
			
			static WindowPtr Make( ::WindowPtr  v )                { return WindowPtr( v ); }
			
			::WindowPtr Get() const                                { return value; }
			
			friend bool operator==( WindowPtr a, WindowPtr b )     { return a.Get() == b.Get(); }
			friend bool operator==( WindowPtr a, ::WindowPtr b )   { return a.Get() == b; }
			friend bool operator==( ::WindowPtr a, WindowPtr b )   { return a == b.Get(); }
			
			friend bool operator!=( WindowPtr a, WindowPtr b )     { return a.Get() != b.Get(); }
			friend bool operator!=( WindowPtr a, ::WindowPtr b )   { return a.Get() != b; }
			friend bool operator!=( ::WindowPtr a, WindowPtr b )   { return a != b.Get(); }
	};
	
	class DialogPtr
	{
		private:
			typedef ::DialogPtr UnderlyingType;
			UnderlyingType value;
			
		public:
			DialogPtr() : value( NULL )  {}
			DialogPtr( ::DialogPtr  value ) : value( value )  {}
			
			operator ::DialogPtr()  const                          { return value;  }
			
			static DialogPtr Make( ::DialogPtr  v )                { return DialogPtr( v ); }
			
			::DialogPtr Get() const                                { return value; }
			
			friend bool operator==( DialogPtr a, DialogPtr b )     { return a.Get() == b.Get(); }
			friend bool operator==( DialogPtr a, ::DialogPtr b )   { return a.Get() == b; }
			friend bool operator==( ::DialogPtr a, DialogPtr b )   { return a == b.Get(); }
			
			friend bool operator!=( DialogPtr a, DialogPtr b )     { return a.Get() != b.Get(); }
			friend bool operator!=( DialogPtr a, ::DialogPtr b )   { return a.Get() != b; }
			friend bool operator!=( ::DialogPtr a, DialogPtr b )   { return a != b.Get(); }
	};
	
#endif
	
	typedef WindowPtr WindowRef;
	
	using ::RGBColor;
	
	using ::PixMap;
	using ::PixMapPtr;
	using ::PixMapHandle;
	
	typedef GrafPtr CGrafPtr;
	
	template <> struct Disposer< CGrafPtr > : public std::unary_function< CGrafPtr, void >
	{
		void operator()( CGrafPtr port ) const
		{
			::DisposePort( port );
		}
	};
	
	template <>
	struct Maker< RGBColor >
	{
		RGBColor operator()( unsigned short red, unsigned short green, unsigned short blue ) const
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
	
	#pragma mark -
	#pragma mark ¥ Routines ¥
	
	void RegisterQuickDrawErrors ();
	
	// 2466
	using ::MacSetPort;
	
	// 2481
	GrafPtr GetPort();
	
	// 2493
	bool QDSwapPort( CGrafPtr newPort, CGrafPtr& oldPort );
	CGrafPtr QDSwapPort( CGrafPtr newPort );
	
	// 2531, 2545, 2559, 2573
	using ::PortSize;
	using ::MovePortTo;
	using ::SetOrigin;
	using ::SetClip;
	
	// 2585
	Owned< RgnHandle > GetClip();
	
	// 2597
	inline void ClipRect( const Rect& rect )  { ::ClipRect( &rect ); }
	
	// 2621
	using ::InitCursor;
	
	// 2633
	inline void MacSetCursor( const Cursor& cursor )  { ::MacSetCursor( &cursor ); }
	inline void MacSetCursor( CursHandle    cursor )  { ::MacSetCursor( *cursor ); }
	void MacSetCursor( ResID id );  // calls GetCursor()
	
	// 2723
	PenState GetPenState();
	
	// 2735
	inline void SetPenState( const PenState& penState )  { ::SetPenState( &penState ); }
	
	// 2892
	inline Rect MacSetRect( short left, short top, short right, short bottom )  { return Make< Rect >( top, left, bottom, right ); }
	inline Rect MacSetRect( Point topLeft, Point bottomRight )  { return MacSetRect( topLeft.h, topLeft.v, bottomRight.h, bottomRight.v ); }
	
	// 2912
	Rect MacOffsetRect( const Rect& r, short dh, short dv );
	
	inline Rect MacOffsetRect( const Rect& r, Point delta )
	{
		return MacOffsetRect( r, delta.h, delta.v );
	}
	
	// 2930
	Rect MacInsetRect( const Rect& r, short dh, short dv );
	
	struct SectRect_Result
	{
		Rect intersection;
		bool intersected;
		
		operator const Rect&() const  { return intersection; }
		operator bool() const  { return intersected; }
	};
	
	// 2948
	SectRect_Result SectRect( const Rect& a, const Rect& b );
	
	// 2963
	Rect MacUnionRect( const Rect& a, const Rect& b );
	
	// 2981
	bool MacEqualRect( const Rect& a, const Rect& b );
	
	// 2998
	bool EmptyRect( const Rect& r );
	
	// 3010, 3025, 3037, 3049
	inline void MacFrameRect ( const Rect& r )  { ::MacFrameRect ( &r ); }
	inline void PaintRect    ( const Rect& r )  { ::PaintRect    ( &r ); }
	inline void EraseRect    ( const Rect& r )  { ::EraseRect    ( &r ); }
	inline void MacInvertRect( const Rect& r )  { ::MacInvertRect( &r ); }
	
	// 3064
	inline void MacFillRect( const Rect& r, const Pattern& pat )  { ::MacFillRect( &r, &pat ); }
	
	// 3081, 3093, 3105, 3117
	inline void FrameOval ( const Rect& r )  { ::FrameOval ( &r ); }
	inline void PaintOval ( const Rect& r )  { ::PaintOval ( &r ); }
	inline void EraseOval ( const Rect& r )  { ::EraseOval ( &r ); }
	inline void InvertOval( const Rect& r )  { ::InvertOval( &r ); }
	
	// 3129
	inline void FillOval( const Rect& r, const Pattern& pat )  { ::FillOval( &r, &pat ); }
	
//	Region handling
	// 3295
	Owned<RgnHandle> NewRgn (void);
	
	// 3307, 3319
//	extern void OpenRgn(void)
	using ::OpenRgn;

//	extern void CloseRgn(RgnHandle dstRgn)
	using ::CloseRgn;
	
	// 3373
	inline void DisposeRgn( Owned< RgnHandle > )  {}
	
	// 3385
//	extern void MacCopyRgn ( RgnHandle srcRgn, RgnHandle dstRgn );
	using ::MacCopyRgn;
	
	Owned<RgnHandle> MacCopyRgn ( RgnHandle srcRgn );
	
	// 3402
//	extern void SetEmptyRgn(RgnHandle rgn);
	using ::SetEmptyRgn;
	
	// 3434
	RgnHandle RectRgn( RgnHandle region, const Rect& rect );	
	Owned< RgnHandle > RectRgn( const Rect& rect );
	
	// 3481
	using ::SectRgn;
	Owned< RgnHandle > SectRgn( RgnHandle a, RgnHandle b );
	
	// 3514
	using ::DiffRgn;
	Owned< RgnHandle > DiffRgn( RgnHandle a, RgnHandle b );
	
	// 3620, 3632
	using ::EraseRgn;
	using ::MacInvertRgn;
	
	// 3664
	void ScrollRect( const Rect& r, short dh, short dv, RgnHandle updateRgn );
	
	Owned< RgnHandle > ScrollRect( const Rect& r, short dh, short dv );
	
	// 3680
	void CopyBits(
		const BitMap* srcBits, 
		const BitMap* dstBits, 
		const Rect& srcRect, 
		const Rect& dstRect, 
		TransferMode mode, 
		RgnHandle maskRgn = NULL
	);
	
	// 3736
	void CopyMask(
		const BitMap* srcBits, 
		const BitMap* maskBits, 
		const BitMap* dstBits, 
		const Rect& srcRect, 
		const Rect& maskRect, 
		const Rect& dstRect
	);
	
	// 3932
	inline Point SetPt( short h, short v )  { return Make< Point >( v, h ); }
	
	// 3947
	Point LocalToGlobal( Point point );
	Rect  LocalToGlobal( const Rect& rect );
	
	// 3959
	Point GlobalToLocal( Point point );
	Rect  GlobalToLocal( const Rect& rect );
	
	// 4265
	Point AddPt( Point a, Point b );
	
	// 4293
	inline bool MacPtInRect( Point pt, const Rect& rect )  { return ::MacPtInRect( pt, &rect ); }
	
	// 4340
	Point SubPt( Point a, Point b );
	
	// 4633
	inline void RGBForeColor( const RGBColor& color )  { ::RGBForeColor( &color ); }
	
	// 4645
	inline void RGBBackColor( const RGBColor& color )  { ::RGBBackColor( &color ); }
	
	// 5329
	CursHandle MacGetCursor( ResID id );
	
	// 4657
	inline void SetCPixel( int x, int y, const RGBColor& color )
	{
		::SetCPixel( x, y, &color );
	}
	
#if OPAQUE_TOOLBOX_STRUCTS
	
	// 5835
	using ::GetPortBitMapForCopyBits;
	
	// 5858
	Rect GetPortBounds( CGrafPtr port );
	
#endif
	
	// 5872
	RGBColor GetPortForeColor( CGrafPtr port );
	
	// 5886
	RGBColor GetPortBackColor( CGrafPtr port );
	
	// 6036
	Owned< RgnHandle > GetPortVisibleRegion( CGrafPtr port );
	
#if OPAQUE_TOOLBOX_STRUCTS
	
	// 6050
	using ::GetPortClipRegion;
	
#else
	
	RgnHandle GetPortClipRegion( CGrafPtr port, RgnHandle region );
	
#endif
	
	Owned< RgnHandle > GetPortClipRegion( CGrafPtr port );
	
	// 6106
	Point GetPortPenSize( CGrafPtr );
	
	// 6194
	bool IsPortColor( CGrafPtr port );
	
#if OPAQUE_TOOLBOX_STRUCTS
	
	// 6438
	using ::SetPortClipRegion;
	
#endif
	
	// 6494
	using ::SetPortPenSize;
	
#if OPAQUE_TOOLBOX_STRUCTS
	
	// 6577, 6589, 6601, 6613, 6625, 6637, 6649
	const BitMap&  GetQDGlobalsScreenBits();
	const Cursor&  GetQDGlobalsArrow();
	const Pattern& GetQDGlobalsDarkGray();
	const Pattern& GetQDGlobalsLightGray();
	const Pattern& GetQDGlobalsGray();
	const Pattern& GetQDGlobalsBlack();
	const Pattern& GetQDGlobalsWhite();
	
	// 6661
	using ::GetQDGlobalsThePort;
	
#endif
	
	// 6729
	inline Owned< CGrafPtr > CreateNewPort()  { return Owned< CGrafPtr >::Seize( ::CreateNewPort() ); }
	
	// 6741
	inline void DisposePort( Owned< CGrafPtr > )  {}
	
	// Declared in Veneer/CGDirectDisplay.hh since it requires CGDirectDisplayID.
	
	// 7105
	//Owned< CGrafPtr > CreateNewPortForCGDisplayID( CGDirectDisplayID display );
	
	#pragma mark -
	#pragma mark ¥ Pseudoreferences ¥
	
	class Port_Details
	{
		public:
			typedef GrafPtr Value;
			typedef Value GetResult;
			typedef Value SetParameter;
			
			GetResult Get() const                        { return GetPort(); }
			void Set( SetParameter port ) const          { SetPort( port ); }
	};
	
	typedef Pseudoreference< Port_Details > Port;
   	
   class PortPenSize_Details
     {
      private:
         CGrafPtr port;
      
      public:
         typedef Point Value;
         typedef Value GetResult;
         typedef Value SetParameter;
         
         PortPenSize_Details( CGrafPtr thePort )     : port( thePort ) {}
         GetResult Get() const                       { return GetPortPenSize( port ); }
         void Set( SetParameter size ) const         { SetPortPenSize( port, size ); }
     };
   
  }

#endif
