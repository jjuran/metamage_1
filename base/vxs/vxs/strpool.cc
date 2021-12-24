/*
	strpool.cc
	----------
*/

#include "vxs/strpool.hh"

// Standard C
#include <stdlib.h>

// Standard C++
#include <algorithm>

// more-libc
#include "more/string.h"

// debug
#include "debug/assert.hh"


namespace vxo
{

const size_t default_lane_size = 1024;

const char* Lane::append( const char* p, size_t n )
{
	ASSERT( bytes_free() >= n );
	
	char* dst = its_data + its_mark;
	
	mempcpy( dst, p, n );
	
	its_mark += n;
	
	return dst;
}

StrPool::~StrPool()
{
	for ( size_t i = 0;  i < its_lanes.size();  ++i )
	{
		free( its_lanes[ i ] );
	}
}

const char* StrPool::operator[]( const plus::string& key )
{
	Item* lb = std::lower_bound( its_bone.begin(), its_bone.end(), key );
	
	if ( lb != end()  &&  *lb == key )
	{
		return *lb;
	}
	
	// We didn't find the string already interned, so add it.
	
	const size_t size = key.size() + 1;  // Leave room for a trailing NUL byte
	
	if ( its_lanes.empty()  ||  its_lanes.back()->bytes_free() < size )
	{
		size_t lane_size = size > default_lane_size ? size : default_lane_size;
		size_t full_size = sizeof (Lane) - max_lane_size + lane_size;
		
		if ( Lane* lane = (Lane*) malloc( full_size ) )
		{
			its_lanes.push_back( lane );
		}
		else
		{
			return NULL;
		}
	}
	
	const char* inserted = its_lanes.back()->append( key.c_str(), size );
	
	*its_bone.insert( lb ) = inserted;
	
	return inserted;
}

}
