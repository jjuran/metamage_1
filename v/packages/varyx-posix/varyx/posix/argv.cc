/*
	argv.cc
	-------
*/

#include "varyx/posix/argv.hh"

// vlib
#include "vlib/array-utils.hh"
#include "vlib/iterators/list_builder.hh"
#include "vlib/symbol.hh"
#include "vlib/types.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"


namespace varyx
{
namespace posix
{

using namespace vlib;


void set_argv( Symbol& sym, const char* arg0, char* const* args )
{
	list_builder argv;
	
	if ( arg0 )
	{
		argv.append( String( arg0 ) );
	}
	
	while ( const char* arg = *args++ )
	{
		argv.append( String( arg ) );
	}
	
	sym.denote( Value( Type( c_str_vtype ), Op_subscript, empty_list ) );
	
	sym.assign( make_array( argv ) );
}

}
}
