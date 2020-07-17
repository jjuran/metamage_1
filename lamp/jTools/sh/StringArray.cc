/*
	StringArray.cc
	--------------
*/

#include "StringArray.hh"


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
		
		strings.reserve( args.size() );
		
		for ( std::size_t i = 0;  i < args.size();  ++i )
		{
			strings.push_back( args[ i ] );
		}
		
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
