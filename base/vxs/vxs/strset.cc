/*
	strset.cc
	---------
*/

#include "vxs/strset.hh"

// Standard C++
#include <algorithm>


namespace vxo
{

StrSet::iterator StrSet::find( const plus::string& item )
{
	iterator lb = std::lower_bound( begin(), end(), item );
	
	if ( lb != end()  &&  *lb != item )
	{
		lb = end();
	}
	
	return lb;
}

StrSet::iterator StrSet::found( const plus::string& item )
{
	iterator lb = std::lower_bound( begin(), end(), item );
	
	if ( lb == end()  ||  *lb != item )
	{
		lb = NULL;
	}
	
	return lb;
}

void StrSet::insert( const plus::string& item )
{
	iterator lb = std::lower_bound( begin(), end(), item );
	
	if ( lb == end()  ||  *lb != item )
	{
		Box* inserted = insert_n( (Box*) lb, 1 );
		
		static_cast< String& >( *inserted ) = item;
	}
}

}
