/*
	import.cc
	---------
*/

#include "varyx/quickdraw/import.hh"

// vlib
#include "vlib/scope.hh"
#include "vlib/types/struct/type.hh"

// varyx-quickdraw
#include "varyx/quickdraw/Geometry.hh"


using vlib::lexical_scope;
using vlib::Symbol_const;
using vlib::Value;

static
void define( lexical_scope& scope, const char* name, const Value& value )
{
	scope.declare( name, Symbol_const ).sym()->assign( value );
}

namespace varyx
{
namespace quickdraw
{

void import( lexical_scope& scope )
{
	#define DEFINE_TYPE( name )  define( scope, #name, get_##name() )
	
	DEFINE_TYPE( Point );
	DEFINE_TYPE( Rect  );
	
	#undef DEFINE_TYPE
}

}
}
