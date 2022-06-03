/*
	FSSpec.cc
	---------
*/

#include "varyx/mac/FSSpec.hh"

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

// plus
#include "plus/mac_utf8.hh"

// Divergence
#include "Divergence/Utilities.hh"

// vlib
#include "vlib/type_info.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/stringify.hh"
#include "vlib/types/integer.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/record.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"


namespace varyx
{
namespace mac
{

namespace Div = Divergence;

/*
	An FSSpec can be constructed from a string which is taken to be a Unix-
	style pathname, whether relative or absolute.  Since it's not obvious
	that that's how it should work, a rationale is provided.
	
	We interpret `FSSpec "path/file.txt"` to mean construction from a Unix
	pathname because (a) we need to, and (b) it doesn't cause any problems.
	
	We need to use this syntax because there isn't a decent alternative.
	We wish to avoid creating new symbol names beyond what Mac OS provides,
	instead reusing the names that exist in order to limit cognitive load.
	So although we could invent FSPathMakeSpec(), we'd prefer not to if at
	all possible.  The routine on which it's based, FSPathMakeRef() is not
	a viable option either, since (a) FSSpecs should work in OS versions
	like System 7, which don't have FSRef support, and (b) FSPathMakeRef()
	uses Mac path syntax in classic Mac OS anyway.  FSMakeFSSpec() isn't
	suitable either, again because conversion from Unix pathname is not what
	the OS-provided function does, so having Varyx do it would be confusing.
	
	One could argue that the same is true of FSSpec itself, but in Mac OS
	it's simply a struct name.  There are constructor functions, and FSSpec
	deals in mere names, not paths.  In other words, FSMakeFSSpec() actually
	parses Mac-style paths, but FSSpec construction doesn't parse anything --
	so we're merely adding behavior rather than changing it.
	
	That said, who's to say that "path/file.txt" is a Unix path rather than
	a Mac filename?  If that is indeed what's intended, the user could write
	`FSSpec (0, 0, mac_filename)` (once such a construction is implemented).
	In practice, dealing with Unix pathnames is going to be far more common,
	so it's appropriate for it to get the shorter syntax (as per Huffman).
*/

Value FSSpec::coerce( const Value& v )
{
	if ( const String* s = v.is< String >() )
	{
		const char* unix_path = s->string().c_str();
		
		return FSSpec( unix_path );
	}
	
	return NIL;
}

static
const char* FSSpec_str_data( const Value& v )
{
	return "<FSSpec>";
}

static const stringify FSSpec_str =
{
	&FSSpec_str_data,
	NULL,
	NULL,
};

static
const char* FSSpec_bin_data( const Value& v )
{
	const FSSpec& spec = static_cast< const FSSpec& >( v );
	
	return (const char*) &spec.get();
}

static
size_t FSSpec_bin_size( const Value& v )
{
	return sizeof (::FSSpec);
}

static const stringify FSSpec_bin =
{
	&FSSpec_bin_data,
	&FSSpec_bin_size,
	NULL,
};

const stringifiers FSSpec_stringifiers =
{
	&FSSpec_str,
	NULL,         // rep: ditto
	&FSSpec_bin,
};

static
Value unary_op_handler( op_type op, const Value& v )
{
	switch ( op )
	{
		case Op_typeof:
			return Type( FSSpec_vtype );
		
		default:
			break;
	}
	
	return NIL;
}

static const operators ops =
{
	&unary_op_handler,
};

const dispatch FSSpec_dispatch =
{
	&FSSpec_stringifiers,
	NULL,
	NULL,
	&ops,
};

#if ! __LP64__

FSSpec::FSSpec()
:
	Value( sizeof (::FSSpec),
	       NULL,
	       Value_other,
	       &FSSpec_dispatch )
{
	::FSSpec& spec = *pointer();
	
	memset( &spec, '\0', sizeof spec );
}

FSSpec::FSSpec( const char* unix_path )
:
	Value( sizeof (::FSSpec),
	       NULL,
	       Value_other,
	       &FSSpec_dispatch )
{
	::FSSpec& spec = *pointer();
	
	spec = Div::ResolvePathToFSSpec( unix_path );
	
	size_t name_len = 1 + *spec.name;
	size_t n_unused = sizeof spec.name - name_len;
	
	uint8_t* begin = spec.name + name_len;
	
	memset( begin, '\0', n_unused );
}

static
Value FSSpec_member( const Value&         obj,
                     const plus::string&  member )
{
	const FSSpec& spec = static_cast< const FSSpec& >( obj );
	
	if ( member == "name" )
	{
		return String( plus::utf8_from_mac( spec.get().name ) );
	}
	
	if ( member == "parID" )
	{
		return Integer( spec.get().parID );
	}
	
	if ( member == "vRefNum" )
	{
		return Integer( spec.get().vRefNum );
	}
	
	return NIL;
}

const type_info FSSpec_vtype =
{
	"FSSpec",
	&assign_to< FSSpec >,
	&FSSpec::coerce,
	&FSSpec_member,
};

#endif  // #if ! __LP64__

}
}
