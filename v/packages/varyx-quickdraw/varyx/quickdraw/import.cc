/*
	import.cc
	---------
*/

#include "varyx/quickdraw/import.hh"

// vlib
#include "vlib/scope.hh"
#include "vlib/type_info.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/struct/type.hh"
#include "vlib/types/type.hh"

// varyx-quickdraw
#include "varyx/quickdraw/Geometry.hh"
#include "varyx/quickdraw/Region.hh"
#include "varyx/quickdraw/Region-procs.hh"


using vlib::lexical_scope;
using vlib::Proc;
using vlib::proc_info;
using vlib::Symbol_const;
using vlib::Type;
using vlib::type_info;
using vlib::Value;

static
void define( lexical_scope& scope, const char* name, const Value& value )
{
	scope.declare( name, Symbol_const ).sym()->assign( value );
}

static
void define( lexical_scope& scope, const proc_info& proc )
{
	define( scope, proc.name, Proc( proc ) );
}

static
void define( lexical_scope& scope, const type_info& type )
{
	define( scope, type.name, Type( type ) );
}

namespace varyx
{
namespace quickdraw
{

void import( lexical_scope& scope )
{
	#define DEFINE_TYPE( name )  define( scope, #name, get_##name() )
	
	#define DEFINE_PROC( name )  define( scope, proc_##name )
	
	DEFINE_TYPE( Point );
	DEFINE_TYPE( Rect  );
	
#if ! __LP64__
	
	define( scope, Region_vtype );
	
	DEFINE_PROC( SetRect );
	
	DEFINE_PROC( OffsetRect );
	DEFINE_PROC( InsetRect  );
	
	DEFINE_PROC( SectRect  );
	DEFINE_PROC( UnionRect );
	
	DEFINE_PROC( PtInRect );
	DEFINE_PROC( Pt2Rect  );
	
	DEFINE_PROC( PtToAngle );
	
	DEFINE_PROC( EqualRect );
	DEFINE_PROC( EmptyRect );
	
	DEFINE_PROC( AddPt );
	DEFINE_PROC( SubPt );
	DEFINE_PROC( SetPt );
	DEFINE_PROC( EqualPt );
	
	DEFINE_PROC( ScalePt );
	DEFINE_PROC( MapPt   );
	DEFINE_PROC( MapRect );
	
	DEFINE_PROC( DeltaPoint );
	
	DEFINE_PROC( SetRectRgn );
	DEFINE_PROC( RectRgn    );
	
	DEFINE_PROC( OffsetRgn );
	DEFINE_PROC( InsetRgn  );
	
	DEFINE_PROC( SectRgn  );
	DEFINE_PROC( UnionRgn );
	DEFINE_PROC( DiffRgn  );
	DEFINE_PROC( XorRgn   );
	
	DEFINE_PROC( PtInRgn   );
	DEFINE_PROC( RectInRgn );
	
	DEFINE_PROC( EqualRgn );
	DEFINE_PROC( EmptyRgn );
	
	DEFINE_PROC( MapRgn );
	
#endif
	
	#undef DEFINE_PROC
	#undef DEFINE_TYPE
}

}
}
