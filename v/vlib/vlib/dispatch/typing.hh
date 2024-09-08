/*
	typing.hh
	---------
*/

#ifndef VLIB_DISPATCH_TYPING_HH
#define VLIB_DISPATCH_TYPING_HH


namespace vlib
{
	
	class Value;
	
	typedef bool  (*typechecker)( const Value& type, const Value& v );
	typedef Value (*constructor)( const Value& type, const Value& v );
	typedef Value (*transformer)( const Value& type, const Value& v );
	
	struct typing
	{
		typechecker  typecheck;
		constructor  construct;
		transformer  transform;
		int          flags;
	};
	
}

#endif
