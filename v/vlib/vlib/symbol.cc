/*
	symbol.cc
	---------
*/

#include "vlib/symbol.hh"

// debug
#include "debug/assert.hh"

// vlib
#include "vlib/assign.hh"
#include "vlib/is_type.hh"
#include "vlib/throw.hh"
#include "vlib/types/term.hh"  // codependent


namespace vlib
{
	
	void Symbol::denote( const Value& vtype )
	{
		if ( ! is_type( vtype ) )
		{
			THROW( "type annotation not a type" );
		}
		
		if ( its_vtype )
		{
			THROW( "reannotation of type-annotated symbol" );
		}
		
		if ( is_defined() )
		{
			THROW( "type annotation of defined symbol" );
		}
		
		its_vtype = vtype;
	}
	
	void Symbol::assign( const Value& v, bool coercive )
	{
		vlib::assign( target(), v, coercive );
	}
	
	bool Symbol::check_type_invariant() const
	{
		return ! vtype()  ||  as_assigned( vtype(), get() );
	}
	
	Target Symbol::target()
	{
		if ( is_immutable() )
		{
			THROW( "reassignment of constant" );
		}
		
		Target result = { &its_value, &its_vtype };
		
		return result;
	}
	
	void assign( const Target&  target,
	             const Value&   v,
	             bool           coercive,
	             const Symbol*  sym )
	{
		ASSERT( sym != 0 );  // NULL
		
		Value& dst         = *target.addr;
		Value const& vtype = *target.type;
		
		if ( vtype )
		{
			return assign( target, v, coercive );
		}
		
		Value tmp = v;
		
		swap( dst, tmp );
		
		if ( ! sym->check_type_invariant() )
		{
			// Oops, higher-level type violation.
			
			swap( dst, tmp );
			
			THROW( "complex type mismatch in assignment" );
		}
	}
	
	Value& Symbol::deref()
	{
		if ( is_const() )
		{
			THROW( "modification of constant" );
		}
		
		its_value.unshare();
		
		return its_value;
	}
	
	Value Symbol::clone() const
	{
		Term result( its_type, name() );
		
		Symbol* sym = result.sym();
		
		if ( its_vtype )
		{
			sym->denote( its_vtype );
		}
		
		sym->assign( its_value );
		
		return result;
	}
	
}
