/*
	StringArray.hh
	--------------
*/

#ifndef STRINGARRAY_HH
#define STRINGARRAY_HH

// Standard C++
#include <vector>

// plus
#include "plus/string.hh"


namespace ShellShock
{
	
	std::size_t CountStringArray( char const* const* arrayPointer );
	
	class StringArray
	{
		typedef std::vector< const char* > Array;
		
		private:
			// non-copyable
			StringArray           ( const StringArray& );
			StringArray& operator=( const StringArray& );
		
			std::vector< plus::string > strings;
			Array array;
		
		public:
			StringArray( const std::vector< plus::string >& args );
			
			char** GetPointer()  { return const_cast< char** >( &array[ 0 ] ); }
			
			void Update();
	};
	
}

#endif
