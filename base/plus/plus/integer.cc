/*
	integer.cc
	----------
*/

#include "plus/integer.hh"

// Standard C
#include <string.h>


namespace plus
{
	
	integer& integer::operator+=( const integer& y )
	{
		integer& x = *this;
		
		if ( y.is_zero() )  return x;
		if ( x.is_zero() )  return x = y;
		
		const bool x_neg = x.is_negative();
		const bool y_neg = y.is_negative();
		
		if ( x_neg == y_neg )
		{
			box.add( y.box );
		}
		else
		{
			box.subtract( y.box );
		}
		
		return x;
	}
	
	integer& integer::operator-=( const integer& y )
	{
		return *this += -y;
	}
	
	integer& integer::operator*=( const integer& y )
	{
		integer& x = *this;
		
		if ( x.is_zero() )  return x;
		if ( y.is_zero() )  return x = y;
		
		box.multiply_by( y.box );
		
		return x;
	}
	
	void integer::copy_into( char* buffer ) const
	{
		memcpy( buffer, box.data(), byte_size() );
	}
	
}
