/*
	strvec.cc
	---------
*/

#include "vxo-string/strvec.hh"

// Standard C++
#include <algorithm>


namespace vxo
{

void StrVec::insert( Item* loc, const_iterator begin, const_iterator end )
{
	Box* inserted = insert_n( (Box*) loc, end - begin );
	
	std::copy( begin, end, (String*) inserted );
}

void StrVec::resize_down( size_t n )
{
	while ( size() > n )
	{
		back() = plus::string::null;
		
		--u.str.length;
	}
}

bool operator==( const StrVec& a, const StrVec& b )
{
	if ( a.size() == b.size() )
	{
		for ( size_t i = 0;  i < a.size();  ++i )
		{
			if ( a[ i ] != b[ i ] )
			{
				return false;
			}
		}
		
		return true;
	}
	
	return false;
}

}
