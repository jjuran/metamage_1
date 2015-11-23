/*
	symbol_table.hh
	---------------
*/

#ifndef VLIB_SYMBOLTABLE_HH
#define VLIB_SYMBOLTABLE_HH

// plus
#include "plus/string.hh"

// vlib
#include "vlib/symbol_id.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	enum symbol_type
	{
		Symbol_const,
		Symbol_var,
	};
	
	class Symbol
	{
		private:
			plus::string  its_name;
			Value         its_vtype;
			Value         its_value;
			symbol_type   its_type;
		
		public:
			Symbol() : its_type()
			{
			}
			
			Symbol( symbol_type          type,
					const plus::string&  name,
					const Value&         value = Value_undefined )
			:
				its_name( name ),
				its_value( value ),
				its_type( type )
			{
			}
			
			void constify()  { its_type = Symbol_const; }
			
			void denote( const Value& vtype );
			
			void assign( const Value& v, bool coercive = false );
			
			Value& deref();
			
			const Value& get() const  { return its_value; }
			
			const Value& vtype() const  { return its_vtype; }
			
			bool is_const() const  { return its_type == Symbol_const; }
			bool is_var()   const  { return its_type != Symbol_const; }
			
			const plus::string& name() const  { return its_name; }
			
			bool is_defined() const  { return ! is_undefined( its_value ); }
	};
	
	symbol_id locate_symbol( const plus::string& name );
	symbol_id create_symbol( const plus::string& name, symbol_type type );
	
	inline
	void assign_symbol( Symbol* symbol, const Value& value )
	{
		symbol->assign( value );
	}
	
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
