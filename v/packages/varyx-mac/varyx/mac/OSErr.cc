/*
	OSErr.cc
	--------
*/

#include "varyx/mac/OSErr.hh"

// vlib
#include "vlib/exceptions.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/record.hh"
#include "vlib/types/string.hh"


namespace varyx
{
namespace mac
{

using namespace vlib;

void throw_MacOS_error( int err, const char* routine )
{
	if ( err  )
	{
		Value code = mapping( "OSErr", Integer( err ) );
		Value from = mapping( "from", String( routine ) );
		
		const Value exception = Record( Value( code, from ) );
		
		throw user_exception( exception, source_spec() );
	}
}

}
}
