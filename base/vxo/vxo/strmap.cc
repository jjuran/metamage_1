/*
	strmap.cc
	---------
*/

#include "vxo/strmap.hh"

// Standard C++
#include <algorithm>

// plus
#include "plus/string.hh"


namespace vxo
{

static inline
bool operator<( const StrMap::Pair& pair, const plus::string& key )
{
	return pair.first < key;
}

StrMap::iterator StrMap::find( const plus::string& key )
{
	iterator lb = std::lower_bound( begin(), end(), key );
	
	if ( lb != end()  &&  lb->first != key )
	{
		lb = end();
	}
	
	return lb;
}

StrMap::iterator StrMap::found( const plus::string& key )
{
	iterator lb = std::lower_bound( begin(), end(), key );
	
	if ( lb == end()  ||  lb->first != key )
	{
		lb = NULL;
	}
	
	return lb;
}

Box& StrMap::operator[]( const plus::string& key )
{
	iterator lb = std::lower_bound( begin(), end(), key );
	
	if ( lb != end()  &&  lb->first == key )
	{
		return lb->second;
	}
	
	String* inserted = (String*) insert_n( (Box*) lb, 2 );
	
	*inserted++ = key;
	
	return *inserted;
}

}
