/*
	symbol.hh
	---------
*/

#ifndef VLIB_SYMBOL_HH
#define VLIB_SYMBOL_HH

// iota
#include "iota/typed_enum.hh"

// plus
#include "plus/string.hh"

// vlib
#include "vlib/value.hh"
#include "vlib/target.hh"


namespace vlib
{
	
	TYPED_ENUM( mark_type, unsigned char )
	{
		Mark_none,   // not participating in GC
		Mark_white,  // not reached
		Mark_black,  // reachable from a root
	};
	
	class Symbol
	{
		private:
			plus::string  its_name;
			Value         its_vtype;
			Value         its_value;
			symbol_type   its_type;
			mark_type     its_mark;
		
		public:
			Symbol() : its_type(), its_mark()
			{
			}
			
			Symbol( symbol_type          type,
					const plus::string&  name,
					const Value&         value = undefined )
			:
				its_name( name ),
				its_value( value ),
				its_type( type ),
				its_mark()
			{
			}
			
			void constify()  { its_type = Symbol_const; }
			
			void denote( const Value& vtype );
			
			void expire()
			{
				its_value = Value_expired;
				its_vtype = Value_expired;
			}
			
			void assign( const Value& v, bool coercive = false );
			
			bool check_type_invariant() const;
			
			Target target();
			
			Value& deref();
			
			Value& deref_unsafe()  { return its_value; }
			
			void set_mark( mark_type mark )  { its_mark = mark; }
			
			mark_type mark() const  { return its_mark; }
			
			const Value& get() const  { return its_value; }
			
			const Value& vtype() const  { return its_vtype; }
			
			bool is_const() const  { return its_type == Symbol_const; }
			bool is_var()   const  { return its_type != Symbol_const; }
			
			const plus::string& name() const  { return its_name; }
			
			bool is_defined() const  { return ! is_undefined( its_value ); }
			
			bool is_immutable() const  { return is_const()  &&  is_defined(); }
			
			Value clone() const;
	};
	
	void assign( const Target&  target,
	             const Value&   v,
	             bool           coercive,
	             const Symbol*  sym );
	
	inline
	const Value& lookup_symbol( const Symbol* symbol )
	{
		return symbol->get();
	}
	
	inline
	Value& modify_symbol( Symbol* symbol )
	{
		return symbol->deref();
	}
	
}

#endif
