// ==============
// StringArray.hh
// ==============

#ifndef SHELLSHOCK_STRINGARRAY_HH
#define SHELLSHOCK_STRINGARRAY_HH

// Standard C++
#include <string>
#include <vector>


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
			std::vector< std::string > strings;
			Array array;
		
		public:
			StringArray();
			StringArray( char const* const* arrayPointer );
			StringArray( const std::vector< std::string >& args );
			
			char** GetPointer()  { return const_cast< char** >( &array[ 0 ] ); }
			
			void Append ( const std::string& s );
			void Prepend( const std::string& s );
			
			void Insert   ( SVector::iterator location, const std::string& s );
			void Overwrite( SVector::iterator location, const std::string& s );
			void Remove   ( SVector::iterator location );
			void Clear();
			
			void Update();
			
			void Swap( StringArray& other );
	};
	
}

#endif

