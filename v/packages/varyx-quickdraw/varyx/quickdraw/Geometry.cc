/*
	Geometry.cc
	-----------
*/

#include "varyx/quickdraw/Geometry.hh"

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

// iota
#include "iota/endian.hh"

// plus
#include "plus/string.hh"

// vlib
#include "vlib/list-utils.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/endec/stdint.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/stdint.hh"
#include "vlib/types/string.hh"
#include "vlib/types/struct/struct.hh"
#include "vlib/types/struct/type.hh"
#include "vlib/types/type.hh"


#define POD( obj )  (&(obj)), (sizeof (obj))

#if __GNUC__  &&  __GNUC__ < 4
#define VEXING_CONSTRUCTION( Class, name )  Class name = Class
#else
#define VEXING_CONSTRUCTION( Class, name )  Class name
#endif


namespace varyx
{
namespace quickdraw
{

#define FIELD( name, type )  Value( String( name ), Op_mapping, type )

#define STRUCT( name )  const Struct_Type name( name##_fields, String( #name ) )

const Struct_Type& get_Point()
{
	static const VEXING_CONSTRUCTION( Value, Point_fields )
	(
		FIELD( "v", i16_type() ),
		FIELD( "h", i16_type() )
	);
	
	static STRUCT( Point );
	
	return Point;
}

const Struct_Type& get_Rect()
{
	static const VEXING_CONSTRUCTION( Value, Rect_fields )
	(
		FIELD( "top", i16_type() ),
		Value(
			FIELD( "left", i16_type() ),
			Value(
				FIELD( "bottom", i16_type() ),
				FIELD( "right", i16_type() ) ) )
	);
	
	static STRUCT( Rect );
	
	return Rect;
}

#if ! __LP64__

static
Value v_SetRect( const Value& v )
{
	list_iterator args( v );
	
	const Integer& l = static_cast< const Integer& >( args.use() );
	const Integer& t = static_cast< const Integer& >( args.use() );
	const Integer& r = static_cast< const Integer& >( args.use() );
	const Integer& b = static_cast< const Integer& >( args.get() );
	
	short _l = l.number().clipped_to< short >();
	short _t = t.number().clipped_to< short >();
	short _r = r.number().clipped_to< short >();
	short _b = b.number().clipped_to< short >();
	
	::Rect rect;
	
	::SetRect( &rect, _l, _t, _r, _b );
	
	return Struct( get_Rect(), plus::string( (char*) POD( rect ) ) );
}

static
Value v_OffsetRect( const Value& v )
{
	list_iterator args( v );
	
	Struct rect = static_cast< const Struct& >( args.use() );
	
	const Integer& dh = static_cast< const Integer& >( args.use() );
	const Integer& dv = static_cast< const Integer& >( args.get() );
	
	rect.unshare();
	
	::Rect& _rect = *(::Rect*) rect.get_data().string().data();
	
	short _dh = dh.number().clipped_to< short >();
	short _dv = dv.number().clipped_to< short >();
	
	OffsetRect( &_rect, _dh, _dv );
	
	return rect;
}

static
Value v_InsetRect( const Value& v )
{
	list_iterator args( v );
	
	Struct rect = static_cast< const Struct& >( args.use() );
	
	const Integer& dh = static_cast< const Integer& >( args.use() );
	const Integer& dv = static_cast< const Integer& >( args.get() );
	
	rect.unshare();
	
	::Rect& _rect = *(::Rect*) rect.get_data().string().data();
	
	short _dh = dh.number().clipped_to< short >();
	short _dv = dv.number().clipped_to< short >();
	
	InsetRect( &_rect, _dh, _dv );
	
	return rect;
}

static
Value v_SectRect( const Value& v )
{
	const Struct& a = static_cast< const Struct& >( first( v ) );
	const Struct& b = static_cast< const Struct& >( rest ( v ) );
	
	const ::Rect& _a = *(::Rect*) a.get_data().string().data();
	const ::Rect& _b = *(::Rect*) b.get_data().string().data();
	
	::Rect dst;
	
	SectRect( &_a, &_b, &dst );
	
	return Struct( get_Rect(), plus::string( (char*) POD( dst ) ) );
}

static
Value v_UnionRect( const Value& v )
{
	const Struct& a = static_cast< const Struct& >( first( v ) );
	const Struct& b = static_cast< const Struct& >( rest ( v ) );
	
	const ::Rect& _a = *(::Rect*) a.get_data().string().data();
	const ::Rect& _b = *(::Rect*) b.get_data().string().data();
	
	::Rect dst;
	
	UnionRect( &_a, &_b, &dst );
	
	return Struct( get_Rect(), plus::string( (char*) POD( dst ) ) );
}

static
Value v_PtInRect( const Value& v )
{
	const Struct& pt   = static_cast< const Struct& >( first( v ) );
	const Struct& rect = static_cast< const Struct& >( rest ( v ) );
	
	const ::Point& _pt  = *(::Point*) pt .get_data().string().data();
	const ::Rect& _rect = *(::Rect*) rect.get_data().string().data();
	
	return vlib::Boolean( PtInRect( _pt, &_rect ) );
}

static
Value v_Pt2Rect( const Value& v )
{
	const Struct& a = static_cast< const Struct& >( first( v ) );
	const Struct& b = static_cast< const Struct& >( rest ( v ) );
	
	const ::Point& _a = *(::Point*) a.get_data().string().data();
	const ::Point& _b = *(::Point*) b.get_data().string().data();
	
	::Rect rect;
	
	Pt2Rect( _a, _b, &rect );
	
	return Struct( get_Rect(), plus::string( (char*) POD( rect ) ) );
}

static
Value v_PtToAngle( const Value& v )
{
	const Struct& rect = static_cast< const Struct& >( first( v ) );
	const Struct& pt   = static_cast< const Struct& >( rest ( v ) );
	
	const ::Rect& _rect = *(::Rect*) rect.get_data().string().data();
	const ::Point& _pt  = *(::Point*) pt .get_data().string().data();
	
	short angle;
	
	PtToAngle( &_rect, _pt, &angle );
	
	return vlib::Integer( angle );
}

static
Value v_EqualRect( const Value& v )
{
	const Struct& a = static_cast< const Struct& >( first( v ) );
	const Struct& b = static_cast< const Struct& >( rest ( v ) );
	
	const ::Rect& _a = *(::Rect*) a.get_data().string().data();
	const ::Rect& _b = *(::Rect*) b.get_data().string().data();
	
	return vlib::Boolean( EqualRect( &_a, &_b ) );
}

static
Value v_EmptyRect( const Value& v )
{
	const Struct& rect = static_cast< const Struct& >( v );
	
	const ::Rect& _rect = *(::Rect*) rect.get_data().string().data();
	
	return vlib::Boolean( EmptyRect( &_rect ) );
}

static
Value v_AddPt( const Value& v )
{
	Struct const& src = static_cast< const Struct& >( first( v ) );
	Struct        dst = static_cast< const Struct& >( rest ( v ) );
	
	dst.unshare();
	
	::Point const& _src = *(::Point*) src.get_data().string().data();
	::Point&       _dst = *(::Point*) dst.get_data().string().data();
	
	AddPt( _src, &_dst );
	
	return dst;
}

static
Value v_SubPt( const Value& v )
{
	Struct const& src = static_cast< const Struct& >( first( v ) );
	Struct        dst = static_cast< const Struct& >( rest ( v ) );
	
	dst.unshare();
	
	::Point const& _src = *(::Point*) src.get_data().string().data();
	::Point&       _dst = *(::Point*) dst.get_data().string().data();
	
	SubPt( _src, &_dst );
	
	return dst;
}

static
Value v_SetPt( const Value& value )
{
	const Integer& v = static_cast< const Integer& >( first( value ) );
	const Integer& h = static_cast< const Integer& >( rest ( value ) );
	
	short _v = v.number().clipped_to< short >();
	short _h = h.number().clipped_to< short >();
	
	::Point pt;
	
	::SetPt( &pt, _v, _h );
	
	return Struct( get_Point(), plus::string( (char*) POD( pt ) ) );
}

static
Value v_EqualPt( const Value& v )
{
	const Struct& a = static_cast< const Struct& >( first( v ) );
	const Struct& b = static_cast< const Struct& >( rest ( v ) );
	
	const ::Point& _a = *(::Point*) a.get_data().string().data();
	const ::Point& _b = *(::Point*) b.get_data().string().data();
	
	return vlib::Boolean( EqualPt( _a, _b ) );
}

static
Value v_ScalePt( const Value& v )
{
	list_iterator args( v );
	
	Struct pt = static_cast< const Struct& >( args.use() );
	
	const Struct& src = static_cast< const Struct& >( args.use() );
	const Struct& dst = static_cast< const Struct& >( args.get() );
	
	pt.unshare();
	
	::Point& _pt = *(::Point*) pt.get_data().string().data();
	
	const ::Rect& _src = *(::Rect*) src.get_data().string().data();
	const ::Rect& _dst = *(::Rect*) dst.get_data().string().data();
	
	ScalePt( &_pt, &_src, &_dst );
	
	return pt;
}

static
Value v_MapPt( const Value& v )
{
	list_iterator args( v );
	
	Struct pt = static_cast< const Struct& >( args.use() );
	
	const Struct& src = static_cast< const Struct& >( args.use() );
	const Struct& dst = static_cast< const Struct& >( args.get() );
	
	pt.unshare();
	
	::Point& _pt = *(::Point*) pt.get_data().string().data();
	
	const ::Rect& _src = *(::Rect*) src.get_data().string().data();
	const ::Rect& _dst = *(::Rect*) dst.get_data().string().data();
	
	MapPt( &_pt, &_src, &_dst );
	
	return pt;
}

static
Value v_MapRect( const Value& v )
{
	list_iterator args( v );
	
	Struct r = static_cast< const Struct& >( args.use() );
	
	const Struct& src = static_cast< const Struct& >( args.use() );
	const Struct& dst = static_cast< const Struct& >( args.get() );
	
	r.unshare();
	
	::Rect& _r = *(::Rect*) r.get_data().string().data();
	
	const ::Rect& _src = *(::Rect*) src.get_data().string().data();
	const ::Rect& _dst = *(::Rect*) dst.get_data().string().data();
	
	MapRect( &_r, &_src, &_dst );
	
	return r;
}

static
Value v_DeltaPoint( const Value& v )
{
	const Struct& a = static_cast< const Struct& >( first( v ) );
	const Struct& b = static_cast< const Struct& >( rest ( v ) );
	
	const ::Point& _a = *(::Point*) a.get_data().string().data();
	const ::Point& _b = *(::Point*) b.get_data().string().data();
	
	long delta = DeltaPoint( _a, _b );
	
	if ( iota::is_little_endian() )
	{
		delta = (UInt32) delta << 16
		      | (UInt32) delta >> 16;
	}
	
	return Struct( get_Point(), plus::string( (char*) POD( delta ) ) );
}

static const Type i16 = i16_vtype;

static const Value& Point = get_Point();
static const Value& Rect  = get_Rect();

static const Value i16_x2    ( i16,   i16   );
static const Value Rect_x2   ( Rect,  Rect  );
static const Value Point_x2  ( Point, Point );
static const Value Point_Rect( Point, Rect  );
static const Value Rect_Point( Rect,  Point );

static const Value i16_x3       ( i16,   i16_x2  );
static const Value i16_x4       ( i16,   i16_x3  );
static const Value Rect_i16_x2  ( Rect,  i16_x2  );
static const Value Rect_x3      ( Rect,  Rect_x2 );
static const Value Point_Rect_x2( Point, Rect_x2 );

#define PROC( name, prototype )  \
	const proc_info proc_##name = { #name, &v_##name, &prototype, Proc_pure }

PROC( SetRect, i16_x4 );

PROC( OffsetRect, Rect_i16_x2 );
PROC( InsetRect,  Rect_i16_x2 );

PROC( SectRect,  Rect_x2 );
PROC( UnionRect, Rect_x2 );

PROC( Pt2Rect,  Point_x2   );
PROC( PtInRect, Point_Rect );

PROC( PtToAngle, Rect_Point );

PROC( EqualRect, Rect_x2 );
PROC( EmptyRect, Rect    );

PROC( AddPt,   Point_x2 );
PROC( SubPt,   Point_x2 );
PROC( SetPt,   i16_x2   );
PROC( EqualPt, Point_x2 );

PROC( ScalePt, Point_Rect_x2 );
PROC( MapPt,   Point_Rect_x2 );
PROC( MapRect, Rect_x3       );

PROC( DeltaPoint, Point_x2 );

#endif  // #if ! __LP64__

}
}
