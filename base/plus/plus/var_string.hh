/*
	var_string.hh
	-------------
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
			char* mutable_data()  { return const_cast< char* >( string::data() ); }
			
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
			            delete_policy  policy )
			{
				assign( p, length, policy );
			}
			
			var_string( const char* p, size_type length )
			{
				assign( p, length );
			}
			
			var_string( const char* p, const char* q )
			{
				assign( p, q );
			}
			
			var_string( const char* s )
			{
				assign( s );
			}
			
			var_string( size_type n, char c )
			{
				assign( n, c );
			}
			
			var_string( const move_t& m ) : string( m )
			{
			}
			
			var_string( const string& other, size_type pos = 0, size_type n = npos )
			{
				assign( other, pos, n );
			}
			
			var_string( const var_string& other )
			{
				assign( other );
			}
			
		#if IOTA_HAS_PASCAL_STRINGS
			
			var_string( const unsigned char* other )
			{
				assign( other );
			}
			
		#endif
			
			var_string& operator=( const var_string& other )
			{
				return assign( other );
			}
			
			var_string& operator=( const string& other )
			{
				return assign( other );
			}
			
			var_string& operator=( const move_t& m )
			{
				return assign( m );
			}
			
			var_string& operator=( const char* s )  { return assign( s ); }
			
			var_string& operator=( char c )  { return assign( 1, c ); }
			
		#if IOTA_HAS_PASCAL_STRINGS
			
			var_string& operator=( const unsigned char* other )
			{
				return assign( other );
			}
			
		#endif
			
			const char* begin() const  { return string::begin(); }
			const char* end  () const  { return string::end  (); }
			
			char* begin()  { return const_cast< char* >( string::begin() ); }
			char* end  ()  { return const_cast< char* >( string::end  () ); }
			
			char& front()  { return begin()[  0 ]; }
			char& back ()  { return end  ()[ -1 ]; }
			
			char& operator[]( size_type i )  { return begin()[ i ]; }
			
			var_string& assign( const char*    p,
			                    size_type      length,
			                    delete_policy  policy,
			                    size_type      capacity = 0 )
			{
				string::assign( p, length, policy, capacity );
				
				copy_on_write( true );
				
				return *this;
			}
			
			var_string& assign( const char* p, size_type length )
			{
				string::assign( p, length );
				
				copy_on_write( true );
				
				return *this;
			}
			
			var_string& assign( const char* p, const char* q )
			{
				string::assign( p, q );
				
				copy_on_write( true );
				
				return *this;
			}
			
			var_string& assign( const char* s )
			{
				string::assign( s );
				
				copy_on_write( true );
				
				return *this;
			}
			
			var_string& assign( size_type n, char c )
			{
				string::assign( n, c );
				
				copy_on_write( true );
				
				return *this;
			}
			
			var_string& assign( const move_t& m );
			
			var_string& assign( const string& other, size_type pos = 0, size_type n = npos )
			{
				string::assign( other, pos, n );
				
				copy_on_write( true );
				
				return *this;
			}
			
		#if IOTA_HAS_PASCAL_STRINGS
			
			var_string& assign( const unsigned char* other )
			{
				return assign( iota::get_pascal_string_data( other ),
				               iota::get_pascal_string_size( other ) );
			}
			
		#endif
			
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
			
			char* insert( char* p, char c );
			
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
			
		#if IOTA_HAS_PASCAL_STRINGS
			
			var_string& operator+=( const unsigned char* s )
			{
				return append( iota::get_pascal_string_data( s ),
				               iota::get_pascal_string_size( s ) );
			}
			
		#endif
	};
	
}

#endif

