/*
	string.cc
	---------
*/

#include "vlib/types/string.hh"

// vlib
#include "vlib/list-utils.hh"
#include "vlib/proc_info.hh"
#include "vlib/string-utils.hh"
#include "vlib/throw.hh"
#include "vlib/type_info.hh"
#include "vlib/types/any.hh"


namespace vlib
{
	
	extern const proc_info proc_lines;
	
	Value String::coerce( const Value& v )
	{
		return make_string( v, Stringified_to_print );
	}
	
	static
	Value string_member( const Value& obj,
	                     const plus::string& member )
	{
		if ( member == "length" )
		{
			return obj.string().size();
		}
		
		if ( member == "join" )
		{
			return bind_args( proc_join, obj );
		}
		
		if ( member == "lines" )
		{
			return bind_args( proc_lines, obj );
		}
		
		THROW( "nonexistent string member" );
		
		return Value_nothing;
	}
	
	const type_info string_vtype =
	{
		"string",
		&assign_to< String >,
		&String::coerce,
		&string_member,
	};
	
	static
	Value v_join( const Value& args )
	{
		const Value& glue = first( args );
		
		const Value pieces = rest( args );
		
		return join( glue.string(), pieces, count( pieces ) );
	}
	
	static
	Value v_lines( const Value& v )
	{
		return lines( v.string() );
	}
	
	static const Value string     = string_vtype;
	static const Value string_etc = Value( string_vtype, etc_vtype );
	
	const proc_info proc_join  = { &v_join,  "join",  &string_etc };
	const proc_info proc_lines = { &v_lines, "lines", &string     };
	
}
