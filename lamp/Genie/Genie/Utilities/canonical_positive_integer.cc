/*
	canonical_positive_integer.cc
	-----------------------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/Utilities/canonical_positive_integer.hh"

// plus
#include "plus/contains.hh"


namespace Genie
{
	
	struct digit
	{
		static bool applies( char c )
		{
			return c >= '0'  &&  c <= '9';
		}
	};
	
	struct non_digit
	{
		static bool applies( char c )
		{
			return !digit::applies( c );
		}
		
		bool operator()( char c ) const
		{
			return applies( c );
		}
	};
	
	bool canonical_positive_integer::applies( const std::string& name )
	{
		return name[0] != '0'  &&  !plus::contains_if( name, non_digit() );
	}
	
}

