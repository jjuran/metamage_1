/*
	var_string.hh
	-------------
	
	Copyright 2010, Joshua Juran
*/

#ifndef PLUS_VARSTRING_HH
#define PLUS_VARSTRING_HH

// plus
#include "plus/string.hh"


namespace plus
{
	
	class var_string : public string
	{
		private:
			char* mutable_data();
			
			char* embiggen( size_type new_length, size_type new_capacity = 0 );
			
			char* insert_uninitialized( char* p, size_type n );
			
			char* erase_unchecked( char* p, size_type n );
			
			char* replace_setup( char* p, size_type m, difference_type delta );
		
		public:
			typedef char&  reference;
			typedef char*  pointer;
			typedef char*  iterator;
			
			var_string()
			{
			}
			
			var_string( const char*    p,
			            size_type      length,
			            delete_policy  policy ) : string( p, length, policy )
			{
			}
			
			var_string( const char* p, size_type length ) : string( p, length )
			{
			}
			
			var_string( const char* p, const char* q ) : string( p, q )
			{
			}
			
			var_string( const char* s ) : string( s )
			{
			}
			
			var_string( size_type n, char c ) : string( n, c )
			{
			}
			
			var_string( const string& other, size_type pos, size_type n = npos )
			:
				string( other, pos, n )
			{
			}
			
			var_string( const string& other ) : string( other )
			{
			}
			
			var_string& operator=( const var_string& other )
			{
				return assign( other );
			}
			
			var_string& operator=( const string& other )
			{
				return assign( other );
			}
			
			var_string& operator=( const char* s )  { return assign( s ); }
			
			var_string& operator=( char c )  { return assign( 1, c ); }
			
			const char* begin() const  { return string::begin(); }
			const char* end  () const  { return string::end  (); }
			
			char* begin()  { return mutable_data(); }
			
			char* end();
			
			char& front()  { return begin()[  0 ]; }
			char& back ()  { return end  ()[ -1 ]; }
			
			char& operator[]( size_type i )  { return mutable_data()[ i ]; }
			
			var_string& assign( const char*    p,
			                    size_type      length,
			                    delete_policy  policy,
			                    size_type      capacity = 0 )
			{
				string::assign( p, length, policy, capacity );
				
				return *this;
			}
			
			var_string& assign( const char* p, size_type length )
			{
				string::assign( p, length );
				
				return *this;
			}
			
			var_string& assign( const char* p, const char* q )
			{
				string::assign( p, q );
				
				return *this;
			}
			
			var_string& assign( const char* s )
			{
				string::assign( s );
				
				return *this;
			}
			
			var_string& assign( size_type n, char c )
			{
				string::assign( n, c );
				
				return *this;
			}
			
			var_string& assign( const string& other, size_type pos, size_type n = npos )
			{
				// Always allocates, even with pos == 0 and n == npos
				
				string::assign( other, pos, n );
				
				return *this;
			}
			
			var_string& assign( const string& other )
			{
				string::assign( other );
				
				return *this;
			}
			
			char* reset( size_type n = 0 )  { return reallocate( n ); }
			
			var_string& erase( size_type pos = 0, size_type n = npos );
			
			char* erase( char* p, char* q );
			
			char* erase( char* p )  { return erase( p, p + 1 ); }
			
			void push_back( char c )  { append( 1, c ); }
			void pop_back()           { resize( size() - 1 ); }
			
			void clear()  { set_length( 0 ); }
			
			void reserve( size_type size );
			void resize ( size_type size, char c = '\0' );
			
			var_string& insert( size_type pos, const string& s );
			var_string& insert( size_type pos, const string& s, size_type begin, size_type n );
			
			var_string& insert( size_type pos, const char* s, size_type n );
			var_string& insert( size_type pos, const char* s );
			
			var_string& insert( size_type pos, size_type n, char c );
			
			void insert( char* p, char* i, char* j );
			
			void insert( char* p, size_type n, char c );
			
			char* insert( char* p, char c )
			{
				return &(insert_uninitialized( p, 1 )[0] = c);
			}
			
			var_string& append( const char* p, size_type length );
			
			var_string& append( const char* p, const char* q )
			{
				return append( p, q - p );
			}
			
			var_string& append( const char* s );
			
			var_string& append( size_type n, char c );
			
			var_string& append( const string& other )
			{
				return append( other.data(), other.size() );
			}
			
			var_string& append( const string& other, size_type pos, size_type n );
			
			var_string& replace( size_type pos, size_type m, const string& s );
			var_string& replace( size_type pos, size_type m, const string& s, size_type offset, size_type n );
			
			var_string& replace( size_type pos, size_type m, const char* s, size_type n );
			var_string& replace( size_type pos, size_type m, const char* s );
			
			var_string& replace( size_type pos, size_type m, size_type n, char c );
			
			void replace( char* p, char* q, const string& s );
			
			void replace( char* p, char* q, const char *i, size_type n );
			void replace( char* p, char* q, const char *s );
			
			void replace( char* p, char* q, size_type n, char c );
			
			void replace( char* p, char* q, const char *i, const char *j );
			
			var_string& operator+=( const string& s )  { return append( s ); }
			var_string& operator+=( const char*   s )  { return append( s ); }
			var_string& operator+=( char          c )  { return append( 1, c ); }
	};
	
}

#endif

