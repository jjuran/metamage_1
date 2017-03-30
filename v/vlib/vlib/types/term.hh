/*
	term.hh
	-------
*/

#ifndef VLIB_TYPES_TERM_HH
#define VLIB_TYPES_TERM_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class Term : public Value
	{
		public:
			Term( symbol_type symtype, const plus::string& name );
	};
	
	class Constant : public Term
	{
		public:
			Constant( const plus::string& name ) : Term( Symbol_const, name )
			{
			}
	};
	
	class Variable : public Term
	{
		public:
			Variable( const plus::string& name ) : Term( Symbol_var, name )
			{
			}
	};
	
}

#endif
