/*
	Region-procs.cc
	---------------
*/

#include "varyx/quickdraw/Region-procs.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// vlib
#include "vlib/types/endec/stdint.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/stdint.hh"
#include "vlib/types/struct/struct.hh"
#include "vlib/types/struct/type.hh"
#include "vlib/types/type.hh"

// varyx-quickdraw
#include "varyx/quickdraw/Geometry.hh"
#include "varyx/quickdraw/Region.hh"


namespace varyx
{
namespace quickdraw
{

#if ! __LP64__

static
Value v_SetRectRgn( const Value& v )
{
	list_iterator args( v );
	
	const Integer& l = static_cast< const Integer& >( args.use() );
	const Integer& t = static_cast< const Integer& >( args.use() );
	const Integer& r = static_cast< const Integer& >( args.use() );
	const Integer& b = static_cast< const Integer& >( args.get() );
	
	Region region;
	
	::RgnHandle rgn = (::RgnHandle) region.get_handle();
	
	short _l = l.number().clipped_to< short >();
	short _t = t.number().clipped_to< short >();
	short _r = r.number().clipped_to< short >();
	short _b = b.number().clipped_to< short >();
	
	SetRectRgn( rgn, _l, _t, _r, _b );
	
	return region;
}

static
Value v_RectRgn( const Value& v )
{
	const Struct& rect = static_cast< const Struct& >( v );
	
	Region region;
	
	::RgnHandle rgn = (::RgnHandle) region.get_handle();
	
	const ::Rect& _rect = *(::Rect*) rect.get_data().string().data();
	
	RectRgn( rgn, &_rect );
	
	return region;
}

static
Value v_OffsetRgn( const Value& v )
{
	list_iterator args( v );
	
	Region region = static_cast< const Region& >( args.use() );
	
	const Integer& dh = static_cast< const Integer& >( args.use() );
	const Integer& dv = static_cast< const Integer& >( args.get() );
	
	region.unshare();
	
	::RgnHandle rgn = (::RgnHandle) region.get_handle();
	
	short _dh = dh.number().clipped_to< short >();
	short _dv = dv.number().clipped_to< short >();
	
	OffsetRgn( rgn, _dh, _dv );
	
	return region;
}

static
Value v_InsetRgn( const Value& v )
{
	list_iterator args( v );
	
	Region region = static_cast< const Region& >( args.use() );
	
	const Integer& dh = static_cast< const Integer& >( args.use() );
	const Integer& dv = static_cast< const Integer& >( args.get() );
	
	region.unshare();
	
	::RgnHandle rgn = (::RgnHandle) region.get_handle();
	
	short _dh = dh.number().clipped_to< short >();
	short _dv = dv.number().clipped_to< short >();
	
	InsetRgn( rgn, _dh, _dv );
	
	return region;
}

static
Value v_SectRgn( const Value& v )
{
	const Region& a = static_cast< const Region& >( first( v ) );
	const Region& b = static_cast< const Region& >( rest ( v ) );
	
	Region c;
	
	::RgnHandle _a = (::RgnHandle) a.get_handle();
	::RgnHandle _b = (::RgnHandle) b.get_handle();
	::RgnHandle _c = (::RgnHandle) c.get_handle();
	
	SectRgn( _a, _b, _c );
	
	return c;
}

static
Value v_UnionRgn( const Value& v )
{
	const Region& a = static_cast< const Region& >( first( v ) );
	const Region& b = static_cast< const Region& >( rest ( v ) );
	
	Region c;
	
	::RgnHandle _a = (::RgnHandle) a.get_handle();
	::RgnHandle _b = (::RgnHandle) b.get_handle();
	::RgnHandle _c = (::RgnHandle) c.get_handle();
	
	UnionRgn( _a, _b, _c );
	
	return c;
}

static
Value v_DiffRgn( const Value& v )
{
	const Region& a = static_cast< const Region& >( first( v ) );
	const Region& b = static_cast< const Region& >( rest ( v ) );
	
	Region c;
	
	::RgnHandle _a = (::RgnHandle) a.get_handle();
	::RgnHandle _b = (::RgnHandle) b.get_handle();
	::RgnHandle _c = (::RgnHandle) c.get_handle();
	
	DiffRgn( _a, _b, _c );
	
	return c;
}

static
Value v_XorRgn( const Value& v )
{
	const Region& a = static_cast< const Region& >( first( v ) );
	const Region& b = static_cast< const Region& >( rest ( v ) );
	
	Region c;
	
	::RgnHandle _a = (::RgnHandle) a.get_handle();
	::RgnHandle _b = (::RgnHandle) b.get_handle();
	::RgnHandle _c = (::RgnHandle) c.get_handle();
	
	XorRgn( _a, _b, _c );
	
	return c;
}

static
Value v_PtInRgn( const Value& v )
{
	const Struct& pt     = static_cast< const Struct& >( first( v ) );
	const Region& region = static_cast< const Region& >( rest ( v ) );
	
	const ::Point& _pt = *(::Point*) pt.get_data().string().data();
	
	::RgnHandle rgn = (::RgnHandle) region.get_handle();
	
	return vlib::Boolean( PtInRgn( _pt, rgn ) );
}

static
Value v_RectInRgn( const Value& v )
{
	const Struct& rect   = static_cast< const Struct& >( first( v ) );
	const Region& region = static_cast< const Region& >( rest ( v ) );
	
	const ::Rect& _rect = *(::Rect*) rect.get_data().string().data();
	
	::RgnHandle rgn = (::RgnHandle) region.get_handle();
	
	return vlib::Boolean( RectInRgn( &_rect, rgn ) );
}

static
Value v_EqualRgn( const Value& v )
{
	const Region& a = static_cast< const Region& >( first( v ) );
	const Region& b = static_cast< const Region& >( rest ( v ) );
	
	::RgnHandle _a = (::RgnHandle) a.get_handle();
	::RgnHandle _b = (::RgnHandle) b.get_handle();
	
	return vlib::Boolean( EqualRgn( _a, _b ) );
}

static
Value v_EmptyRgn( const Value& v )
{
	const Region& region = static_cast< const Region& >( v );
	
	::RgnHandle rgn = (::RgnHandle) region.get_handle();
	
	return vlib::Boolean( EmptyRgn( rgn ) );
}

static
Value v_MapRgn( const Value& v )
{
	list_iterator args( v );
	
	Region region = static_cast< const Region& >( args.use() );
	
	const Struct& src = static_cast< const Struct& >( args.use() );
	const Struct& dst = static_cast< const Struct& >( args.get() );
	
	region.unshare();
	
	::RgnHandle rgn = (::RgnHandle) region.get_handle();
	
	const ::Rect& _src = *(::Rect*) src.get_data().string().data();
	const ::Rect& _dst = *(::Rect*) dst.get_data().string().data();
	
	MapRgn( rgn, &_src, &_dst );
	
	return region;
}

static const Type i16 = i16_vtype;
static const Type Rgn = Region_vtype;

static const Value& Point = get_Point();
static const Value& Rect  = get_Rect();

static const Value i16_x2   ( i16,   i16  );
static const Value Rgn_x2   ( Rgn,   Rgn  );
static const Value Rect_x2  ( Rect,  Rect );
static const Value Rect_Rgn ( Rect,  Rgn  );
static const Value Point_Rgn( Point, Rgn  );

static const Value i16_x3     ( i16,  i16_x2  );
static const Value i16_x4     ( i16,  i16_x3  );
static const Value Rgn_i16_x2 ( Rgn,  i16_x2  );
static const Value Rgn_Rect_x2( Rgn,  Rect_x2 );

#define PROC( name, prototype )  \
	const proc_info proc_##name = { #name, &v_##name, &prototype, Proc_pure }

PROC( SetRectRgn, i16_x4 );
PROC( RectRgn,    Rect   );

PROC( OffsetRgn, Rgn_i16_x2 );
PROC( InsetRgn,  Rgn_i16_x2 );

PROC( SectRgn,  Rgn_x2 );
PROC( UnionRgn, Rgn_x2 );
PROC( DiffRgn,  Rgn_x2 );
PROC( XorRgn,   Rgn_x2 );

PROC( PtInRgn,   Point_Rgn );
PROC( RectInRgn, Rect_Rgn  );

PROC( EqualRgn, Rgn_x2 );
PROC( EmptyRgn, Rgn    );

PROC( MapRgn, Rgn_Rect_x2 );

#else

int dummy;

#endif  // #if ! __LP64__

}
}
