// Nitrogen/Quickdraw.cc
// ---------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2006 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#include "Nitrogen/Quickdraw.hh"

// Nitrogen
#include "Nitrogen/MacErrors.hh"
#include "Nitrogen/OSStatus.hh"
#include "Nitrogen/Resources.hh"

#if TARGET_RT_MAC_MACHO || TARGET_API_MAC_OSX
#include "Nitrogen/CGDirectDisplay.hh"
#endif


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( QuickDraw )
	
	
	static void RegisterQuickDrawErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class QuickDrawErrorsRegistration
	{
		public:
			QuickDrawErrorsRegistration()  { RegisterQuickDrawErrors(); }
	};
	
	static QuickDrawErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
#if ! __LP64__
	
	nucleus::owned< RgnHandle > GetClip()
	{
		nucleus::owned< RgnHandle > result = NewRgn();
		::GetClip( result.get() );
		
		return result;
	}
	
	void MacSetCursor( Mac::ResID id )
	{
		MacSetCursor( MacGetCursor( id ) );
	}
	
	Rect MacOffsetRect( const Rect& r, short dh, short dv )
	{
		return MacSetRect( r.left   + dh,
		                   r.top    + dv,
		                   r.right  + dh,
		                   r.bottom + dv );
	}
	
	Rect MacInsetRect( const Rect& r, short dh, short dv )
	{
		return MacSetRect( r.left   + dh,
		                   r.top    + dv,
		                   r.right  - dh,
		                   r.bottom - dv );
	}
	
	SectRect_Result SectRect( const Rect& a, const Rect& b )
	{
		SectRect_Result result;
		result.intersected = ::SectRect( &a, &b, &result.intersection );
		
		return result;
	}
	
	nucleus::owned< RgnHandle > NewRgn()
	{
		RgnHandle result = ::NewRgn();
		
		if ( result == NULL )
		{
			throw MemFullErr();
		}
		
		return nucleus::owned< RgnHandle >::seize( result );
	}
	
	nucleus::owned< RgnHandle > CloseRgn()
	{
		nucleus::owned< RgnHandle > result = NewRgn();
		
		::CloseRgn( result.get() );
		
		return result;
	}
	
	nucleus::owned< RgnHandle > MacCopyRgn( RgnHandle srcRgn )
	{
		nucleus::owned< RgnHandle > result = NewRgn();
		::MacCopyRgn( srcRgn, result );
		
		return result;
	}
	
	nucleus::owned< RgnHandle > RectRgn( const Rect& rect )
	{
		nucleus::owned< RgnHandle > result = NewRgn();
		::RectRgn( result, &rect );
		
		return result;
	}
	
	nucleus::owned< RgnHandle > SectRgn( RgnHandle a, RgnHandle b )
	{
		nucleus::owned< RgnHandle > result = NewRgn();
		::SectRgn( a, b, result );
		
		return result;
	}
	
	nucleus::owned< RgnHandle > MacUnionRgn( RgnHandle a, RgnHandle b )
	{
		nucleus::owned< RgnHandle > result = NewRgn();
		::MacUnionRgn( a, b, result );
		
		return result;
	}
	
	nucleus::owned< RgnHandle > DiffRgn( RgnHandle a, RgnHandle b )
	{
		nucleus::owned< RgnHandle > result = NewRgn();
		::DiffRgn( a, b, result );
		
		return result;
	}
	
	nucleus::owned< RgnHandle > MacXorRgn( RgnHandle a, RgnHandle b )
	{
		nucleus::owned< RgnHandle > result = NewRgn();
		::MacXorRgn( a, b, result );
		
		return result;
	}
	
	nucleus::owned< RgnHandle > ScrollRect( const Rect& r, short dh, short dv )
	{
		nucleus::owned< RgnHandle > region = NewRgn();
		ScrollRect( r, dh, dv, region );
		
		return region;
	}
	
	Rect LocalToGlobal( const Rect& rect )
	{
		return MacSetRect( LocalToGlobal( SetPt( rect.left,  rect.top    ) ),
		                   LocalToGlobal( SetPt( rect.right, rect.bottom ) ) );
	}
	
	Rect GlobalToLocal( const Rect& rect )
	{
		return MacSetRect( GlobalToLocal( SetPt( rect.left,  rect.top    ) ),
		                   GlobalToLocal( SetPt( rect.right, rect.bottom ) ) );
	}
	
	Point AddPt( Point a, Point b )
	{
		return SetPt( a.h + b.h,
		              a.v + b.v );
	}
	
	Point SubPt( Point a, Point b )
	{
		return SetPt( a.h - b.h,
		              a.v - b.v );
	}
	
	nucleus::owned< CTabHandle > GetCTable( short ctabID )
	{
		CTabHandle result = ::GetCTable( ctabID );
		
		if ( result == NULL )
		{
			QDError();
		}
		
		return nucleus::owned< CTabHandle >::seize( result );
	}
	
	void QDError()
	{
		Mac::ThrowOSStatus( ::QDError() );
	}
	
	CursHandle MacGetCursor( Mac::ResID id )
	{
		return CheckResource( ::MacGetCursor( id ) );
	}
	
	nucleus::owned< RgnHandle > GetPortVisibleRegion( CGrafPtr port )
	{
		nucleus::owned< RgnHandle > region = NewRgn();
		(void)Nitrogen::GetPortVisibleRegion( port, region );
		
		return region;
	}
	
	nucleus::owned< RgnHandle > GetPortClipRegion( CGrafPtr port )
	{
		nucleus::owned< RgnHandle > region = NewRgn();
		Nitrogen::GetPortClipRegion( port, region );
		
		return region;
	}
	
	nucleus::owned< CGrafPtr > CreateNewPort()
	{
		return nucleus::owned< CGrafPtr >::seize( ::CreateNewPort(),
		                                          &::DisposePort );
	}
	
#if TARGET_API_MAC_OSX
	
	// Declared in Nitrogen/CGDirectDisplay.hh, since it depends on CGDirectDisplayID
	
	nucleus::owned< CGrafPtr > CreateNewPortForCGDisplayID( CGDirectDisplayID display )
	{
		UInt32 id = (UInt32) display;
		
		return nucleus::owned< CGrafPtr >::seize( ::CreateNewPortForCGDisplayID( id ),
		                                          &::DisposePort );
	}
	
#endif
	
#endif  // #if ! __LP64__
	
	void RegisterQuickDrawErrors()
	{
		NUCLEUS_REQUIRE_ERRORS( MemoryManager );
		
		// CopyBits couldn't allocate required temporary memory
		RegisterOSStatus< -143                    >();
		
		// Ran out of stack space while drawing polygon
		RegisterOSStatus< -144                    >();
		
		RegisterOSStatus< noMemForPictPlaybackErr >();
		RegisterOSStatus< rgnOverflowErr          >();
		RegisterOSStatus< pixMapTooDeepErr        >();
		RegisterOSStatus< insufficientStackErr    >();
		RegisterOSStatus< cMatchErr               >();
		RegisterOSStatus< cTempMemErr             >();
		RegisterOSStatus< cNoMemErr               >();
		RegisterOSStatus< cRangeErr               >();
		RegisterOSStatus< cProtectErr             >();
		RegisterOSStatus< cDevErr                 >();
		RegisterOSStatus< cResErr                 >();
		RegisterOSStatus< cDepthErr               >();
		RegisterOSStatus< rgnTooBigErr            >();
		RegisterOSStatus< updPixMemErr            >();
		RegisterOSStatus< pictInfoVersionErr      >();
		RegisterOSStatus< pictInfoIDErr           >();
		RegisterOSStatus< pictInfoVerbErr         >();
		RegisterOSStatus< cantLoadPickMethodErr   >();
		RegisterOSStatus< colorsRequestedErr      >();
		RegisterOSStatus< pictureDataErr          >();
	}
  
}
