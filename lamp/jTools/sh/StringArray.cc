/*
	StringArray.cc
	--------------
*/

#include "StringArray.hh"

// Standard C++
#include <algorithm>


namespace ShellShock
{
	
	std::size_t CountStringArray( char const* const* arrayPointer )
	{
		std::size_t result = 0;
		
		while ( *arrayPointer++ )
		{
			++result;
		}
		
		return result;
	}
	
	StringArray::StringArray( const std::vector< plus::string >& args )
	{
		// Assume arrayPointer to be already sorted.
		std::copy( args.begin(),
		           args.end(),
		           std::back_inserter( strings ) );
		
		Update();
	}
		
	void StringArray::Update()
	{
		array.reserve( strings.size() + 1 );
		
		for ( std::size_t i = 0;  i < strings.size();  ++i )
		{
			array.push_back( strings[ i ].c_str() );
		}
		
		array.push_back( NULL );
	}
	
}
