// ==============
// StringArray.cc
// ==============

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
	
	void InitializeSVector( SVector& sv, std::size_t size )
	{
		sv.resize( size + 1 );
		
		// In case allocation fails mid-way, we'll know which slots have data.
		// This also fills NULL into the last slot.
		std::fill( sv.begin(),
		           sv.end(),
		           (char*)NULL );
	}
	
	StringArray::StringArray()
	{
		array.push_back( NULL );
	}
	
	StringArray::StringArray( char const* const* arrayPointer )
	{
		int count = CountStringArray( arrayPointer );
		
		// Assume arrayPointer to be already sorted.
		std::copy( arrayPointer,
		           arrayPointer + count,
		           std::back_inserter( strings ) );
		
		Update();
	}
	
	StringArray::StringArray( const std::vector< plus::string >& args )
	{
		// Assume arrayPointer to be already sorted.
		std::copy( args.begin(),
		           args.end(),
		           std::back_inserter( strings ) );
		
		Update();
	}
	
	void StringArray::Append( const plus::string& s )
	{
		strings.push_back( s );
		
		/*
		array.back() = strings.back().c_str();
		array.push_back( NULL );
		*/
		
		Update();
	}
	
	void StringArray::Prepend( const plus::string& s )
	{
		//strings.push_front( s );
		
		strings.resize( strings.size() + 1 );
		
		std::copy( strings.begin(),
		           strings.end() - 1,
		           strings.begin() + 1 );
		
		strings.front() = s;
		
		/*
		array.resize( array.size() + 1 );
		
		std::copy( array.begin(),
		           array.end() - 1,
		           array.begin() + 1 );
		
		array.front() = strings.front().c_str();
		*/
		
		Update();
	}
	
	void StringArray::Insert( SVector::iterator location, const plus::string& s )
	{
		std::size_t offset = location - array.begin();
		
		strings.resize( strings.size() + 1 );
		
		std::copy_backward( strings.begin() + offset,
		                    strings.end() - 1,
		                    strings.end() );
		
		strings[ offset ] = s;
		
		Update();
	}
	
	void StringArray::Overwrite( SVector::iterator location, const plus::string& s )
	{
		std::size_t offset = location - array.begin();
		
		strings[ offset ] = s;
		
		Update();
	}
	
	void StringArray::Remove( SVector::iterator location )
	{
		std::size_t offset = location - array.begin();
		
		std::copy( strings.begin() + offset + 1,
		           strings.end(),
		           strings.begin() + offset );
		
		strings.resize( strings.size() - 1 );
		
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
	
	void StringArray::Clear()
	{
		array.clear();  // Don't leave dangling pointers; clear array first
		array.push_back( NULL );
		strings.clear();
	}
	
	void StringArray::Swap( StringArray& other )
	{
		using std::swap;
		
		swap( strings, other.strings );
		swap( array,   other.array   );
	}
	
}
