// ==============
// StringArray.hh
// ==============

#ifndef SHELLSHOCK_STRINGARRAY_HH
#define SHELLSHOCK_STRINGARRAY_HH

// Standard C++
#include <vector>

// plus
#include "plus/string.hh"


namespace ShellShock
{
	
	typedef std::vector< const char* > SVector;
	
	std::size_t CountStringArray( char const* const* arrayPointer );
	
	void InitializeSVector( SVector& sv, std::size_t size );
	
	class StringArray
	{
		typedef SVector Array;
		
		private:
			StringArray( const StringArray& );
			
			StringArray& operator=( const StringArray& );
		
		protected:
			std::vector< plus::string > strings;
			Array array;
		
		public:
			StringArray();
			StringArray( char const* const* arrayPointer );
			StringArray( const std::vector< plus::string >& args );
			
			char** GetPointer()  { return const_cast< char** >( &array[ 0 ] ); }
			
			void Append ( const plus::string& s );
			void Prepend( const plus::string& s );
			
			void Insert   ( SVector::iterator location, const plus::string& s );
			void Overwrite( SVector::iterator location, const plus::string& s );
			void Remove   ( SVector::iterator location );
			void Clear();
			
			void Update();
			
			void Swap( StringArray& other );
	};
	
}

#endif

