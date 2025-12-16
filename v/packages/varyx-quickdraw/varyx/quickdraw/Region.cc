/*
	Region.cc
	---------
*/

#include "varyx/quickdraw/Region.hh"

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

// mac-qd-utils
#include "mac_qd/get_region_bounds.hh"

// vlib
#include "vlib/assign.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/types/struct/struct.hh"
#include "vlib/types/struct/type.hh"
#include "vlib/types/type.hh"

// varyx-quickdraw
#include "varyx/quickdraw/Geometry.hh"


#define POD( obj )  (&(obj)), (sizeof (obj))


namespace varyx
{
namespace quickdraw
{

Value Region::coerce( const Value& v )
{
	if ( test( v ) )
	{
		return v;
	}
	
#if ! __LP64__
	
	if ( as_assigned( get_Rect(), v ) )
	{
		const Struct& rect = static_cast< const Struct& >( v );
		
		const ::Rect& _rect = *(::Rect*) rect.get_data().string().data();
		
		Region result;
		
		::RgnHandle rgn = *(::RgnHandle*) result.pointer();
		
		RectRgn( rgn, &_rect );
		
		return result;
	}
	
#endif
	
	switch ( v.type() )
	{
		case Value_empty_list:
			break;
		
		default:
			THROW( "can't coerce to Region" );
	}
	
	return Region();
}

static
const char* Region_str_data( const Value& v )
{
	return "<Region>";
}

static const stringify Region_str =
{
	&Region_str_data,
	NULL,
	NULL,
};

static
size_t Region_bin_size( const Value& v )
{
	const Region& region = static_cast< const Region& >( v );
	
	return region.data_size();
}

static
char* Region_bin_copy( char* p, const Value& v )
{
	const Region& region = static_cast< const Region& >( v );
	
	return region.copy_data( p );
}

static const stringify Region_bin =
{
	NULL,
	&Region_bin_size,
	&Region_bin_copy,
};

static const stringifiers Region_stringifiers =
{
	&Region_str,
	NULL,         // rep: ditto
	&Region_bin,
};

static
Value unary_op_handler( op_type op, const Value& v )
{
	switch ( op )
	{
		case Op_typeof:
			return Type( Region_vtype );
		
		default:
			break;
	}
	
	return Value();
}

static const operators ops =
{
	&unary_op_handler,
	NULL,
	NULL,
	NULL,
};

const dispatch Region_dispatch =
{
	&Region_stringifiers,
	NULL,
	NULL,
	&ops,
};

#if ! __LP64__

static
void Region_destructor( void* ptr )
{
	::RgnHandle rgn = *(::RgnHandle*) ptr;
	
	::DisposeRgn( rgn );
}

static
bool Region_duplicator( void* ptr )
{
	::RgnHandle dup = NewRgn();
	
	if ( dup )
	{
		::RgnHandle& rgn = *(::RgnHandle*) ptr;
		
		CopyRgn( rgn, dup );
		
		rgn = dup;
	}
	
	return dup;
}

Region::Region()
:
	Value( sizeof (::RgnHandle),
	       &Region_destructor,
	       &Region_duplicator,
	       Value_other,
	       &Region_dispatch )
{
	RgnHandle rgn = NewRgn();
	
	*(::RgnHandle*) pointer() = rgn;
}

unsigned long Region::data_size() const
{
	return GetHandleSize( get_handle() );
}

char* Region::copy_data( char* p ) const
{
	Size size = data_size();
	
	void* src = *get_handle();
	
	BlockMoveData( src, p, size );
	
	return p + size;
}

#endif  // #if ! __LP64__

static
Value Region_member( const Value&         obj,
                     const plus::string&  member )
{
	const Region& region = static_cast< const Region& >( obj );
	
	::RgnHandle rgn = (::RgnHandle) region.get_handle();
	
#if ! __LP64__
	
	using mac::qd::get_region_bounds_unsafe;
	
	if ( member == "bounds" )
	{
		/*
			This is safe:  A Rect is small enough that
			constructing the string doesn't allocate
			memory, and get_Rect() only allocates on
			the first call -- which already happened
			in import(), which is a prerequisite for
			constructing a Region in the first place.
		*/
		
		const Rect& bounds = get_region_bounds_unsafe( rgn );
		
		return Struct( get_Rect(), plus::string( (char*) POD( bounds ) ) );
	}
	
#endif
	
	THROW( "nonexistent Region member" );
	
	return Value();
}

const type_info Region_vtype =
{
	"Region",
	&assign_to< Region >,
	&Region::coerce,
	NULL,
	&Region_member,
	0,
};

}
}
