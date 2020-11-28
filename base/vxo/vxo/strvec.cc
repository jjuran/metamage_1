/*
	strvec.cc
	---------
*/

#include "vxo/strvec.hh"

// Standard C++
#include <algorithm>

// plus
#include "plus/string.hh"


namespace vxo
{

void StrVec::insert( Item* loc, const_iterator begin, const_iterator end )
{
	Box* inserted = insert_n( (Box*) loc, end - begin );
	
	std::copy( begin, end, (String*) inserted );
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
