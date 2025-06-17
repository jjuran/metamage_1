/*
	FSRef.cc
	--------
*/

#include "varyx/mac/FSRef.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// Standard C
#include <string.h>

// mac-sys-utils
#include "mac_sys/errno_from_mac_error.hh"

// mac-relix-utils
#include "mac_relix/FSRef_from_path.hh"

// vlib
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"

// varyx-posix
#include "varyx/posix/exception.hh"

// varyx-mac
#include "varyx/mac/OSErr.hh"


namespace varyx
{
namespace mac
{

/*
	An FSRef can be constructed from a string which is taken
	to be a Unix-style pathname, whether relative or absolute.
*/

Value FSRef::coerce( const Value& v )
{
	if ( const String* s = v.is< String >() )
	{
		const char* unix_path = s->string().c_str();
		
		return FSRef( unix_path );
	}
	
	return NIL;
}

static
const char* FSRef_str_data( const Value& v )
{
	return "<FSRef>";
}

static const stringify FSRef_str =
{
	&FSRef_str_data,
	NULL,
	NULL,
};

static
const char* FSRef_bin_data( const Value& v )
{
	const FSRef& spec = static_cast< const FSRef& >( v );
	
	return (const char*) &spec.get();
}

static
size_t FSRef_bin_size( const Value& v )
{
	return sizeof (::FSRef);
}

static const stringify FSRef_bin =
{
	&FSRef_bin_data,
	&FSRef_bin_size,
	NULL,
};

const stringifiers FSRef_stringifiers =
{
	&FSRef_str,
	NULL,         // rep: ditto
	&FSRef_bin,
};

static
Value unary_op_handler( op_type op, const Value& v )
{
	switch ( op )
	{
		case Op_typeof:
			return Type( FSRef_vtype );
		
		default:
			break;
	}
	
	return NIL;
}

static const operators ops =
{
	&unary_op_handler,
};

const dispatch FSRef_dispatch =
{
	&FSRef_stringifiers,
	NULL,
	NULL,
	&ops,
};

FSRef::FSRef()
:
	Value( sizeof (::FSRef),
	       NULL,
	       Value_other,
	       &FSRef_dispatch )
{
	::FSRef& ref = *pointer();
	
	memset( &ref, '\0', sizeof ref );
}

FSRef::FSRef( const char* unix_path )
:
	Value( sizeof (::FSRef),
	       NULL,
	       Value_other,
	       &FSRef_dispatch )
{
	using ::mac::relix::FSRef_from_path;
	
	using ::mac::sys::Error;
	using ::mac::sys::errno_from_mac_error;
	
	using varyx::posix::path_error;
	
	::FSRef& file = *pointer();
	
	Error err = FSRef_from_path( unix_path, file );
	
	int errnum = is_errno( err ) ? errno_from_muxed( err )
	                             : errno_from_mac_error( err );
	
	if ( errnum > 0 )
	{
		path_error( unix_path, errnum );
	}
	
	if ( err )
	{
		throw_MacOS_error( err, unix_path );
	}
}

const type_info FSRef_vtype =
{
	"FSRef",
	&assign_to< FSRef >,
	&FSRef::coerce,
	NULL,
	NULL,
	0,
};

}
}
