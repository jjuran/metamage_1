/*
	own_string.hh
	-------------
*/

#ifndef PLUS_OWNSTRING_HH
#define PLUS_OWNSTRING_HH

// iota
#include "iota/class.hh"

// plus
#include "plus/string.hh"
#include "plus/string_common.hh"


namespace plus
{
	
	class own_string : public string
	{
		NO_NEW_DELETE
		
		private:
			char* mutable_data()  { return const_cast< char* >( string::data() ); }
			
			char* embiggen( size_type new_length );
			
			char* insert_uninitialized( char* p, size_type n );
			
			char* erase_unchecked( char* p, size_type n );
			
			char* replace_setup( char* p, size_type m, difference_type delta );
		
		public:
			typedef char&  reference;
			typedef char*  pointer;
			typedef char*  iterator;
			
			own_string()
			{
			}
			
			own_string( const char*    p,
			            size_type      length,
			            delete_policy  policy )
			{
				assign( p, length, policy );
			}
			
			own_string( const char* p, size_type length )
			{
				assign( p, length );
			}
			
			own_string( const char* p, const char* q )
			{
				assign( p, q );
			}
			
			own_string( const char* s )
			{
				assign( s );
			}
			
			own_string( size_type n, char c )
			{
				assign( n, c );
			}
			
			own_string( datum_movable& m ) : string( m )
			{
				copy_on_write( true );
			}
			
			own_string( const string& other, size_type pos = 0, size_type n = npos )
			{
				assign( other, pos, n );
			}
			
			own_string( const own_string& other )
			{
				assign( other );
			}
			
		#if IOTA_HAS_PASCAL_STRINGS
			
			own_string( const unsigned char* other )
			{
				assign( other );
			}
			
		#endif
			
			own_string& operator=( const own_string& other )
			{
				return assign( other );
			}
			
			own_string& operator=( const string& other )
			{
				return assign( other );
			}
			
			own_string& operator=( datum_movable& m )
			{
				return assign( m );
			}
			
			own_string& operator=( const char* s )  { return assign( s ); }
			
			own_string& operator=( char c )  { return assign( 1, c ); }
			
		#if IOTA_HAS_PASCAL_STRINGS
			
			own_string& operator=( const unsigned char* other )
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
			
			own_string& assign( const char*    p,
			                    size_type      length,
			                    delete_policy  policy,
			                    size_type      capacity = 0 )
			{
				string::assign( p, length, policy, capacity );
				
				copy_on_write( true );
				
				return *this;
			}
			
			own_string& assign( const char* p, size_type length )
			{
				string::assign( p, length );
				
				copy_on_write( true );
				
				return *this;
			}
			
			own_string& assign( const char* p, const char* q )
			{
				string::assign( p, q );
				
				copy_on_write( true );
				
				return *this;
			}
			
			own_string& assign( const char* s )
			{
				string::assign( s );
				
				copy_on_write( true );
				
				return *this;
			}
			
			own_string& assign( size_type n, char c )
			{
				string::assign( n, c );
				
				copy_on_write( true );
				
				return *this;
			}
			
			own_string& assign( datum_movable& m );
			
			own_string& assign( const string& other, size_type pos = 0, size_type n = npos )
			{
				string::assign( other, pos, n );
				
				copy_on_write( true );
				
				return *this;
			}
			
		#if IOTA_HAS_PASCAL_STRINGS
			
			own_string& assign( const unsigned char* other )
			{
				return assign( iota::get_pascal_string_data( other ),
				               iota::get_pascal_string_size( other ) );
			}
			
		#endif
			
			own_string& erase( size_type pos = 0, size_type n = npos );
			
			char* erase( char* p, char* q );
			
			char* erase( char* p )  { return erase( p, p + 1 ); }
			
			void push_back( char c )  { append( 1, c ); }
			void pop_back()           { resize( size() - 1 ); }
			
			void clear()  { string_set_length( store, 0, mutable_data() ); }
			
			void reserve( size_type size )  { string_reserve( store, size ); }
			void resize ( size_type size, char c = '\0' );
			
			own_string& insert( size_type pos, const string& s );
			own_string& insert( size_type pos, const string& s, size_type begin, size_type n );
			
			own_string& insert( size_type pos, const char* s, size_type n );
			own_string& insert( size_type pos, const char* s );
			
			own_string& insert( size_type pos, size_type n, char c );
			
			void insert( char* p, const char* i, const char* j );
			
			void insert( char* p, size_type n, char c );
			
			char* insert( char* p, char c );
			
			own_string& append( const char* p, size_type length );
			
			own_string& append( const char* p, const char* q )
			{
				return append( p, q - p );
			}
			
			own_string& append( const char* s );
			
			own_string& append( size_type n, char c );
			
			own_string& append( const string& other )
			{
				return append( other.data(), other.size() );
			}
			
			own_string& append( const string& other, size_type pos, size_type n );
			
			own_string& replace( size_type pos, size_type m, const string& s );
			own_string& replace( size_type pos, size_type m, const string& s, size_type offset, size_type n );
			
			own_string& replace( size_type pos, size_type m, const char* s, size_type n );
			own_string& replace( size_type pos, size_type m, const char* s );
			
			own_string& replace( size_type pos, size_type m, size_type n, char c );
			
			void replace( char* p, char* q, const string& s );
			
			void replace( char* p, char* q, const char *i, size_type n );
			void replace( char* p, char* q, const char *s );
			
			void replace( char* p, char* q, size_type n, char c );
			
			void replace( char* p, char* q, const char *i, const char *j );
			
			own_string& operator+=( const string& s )  { return append( s ); }
			own_string& operator+=( const char*   s )  { return append( s ); }
			own_string& operator+=( char          c )  { return append( 1, c ); }
			
		#if IOTA_HAS_PASCAL_STRINGS
			
			own_string& operator+=( const unsigned char* s )
			{
				return append( iota::get_pascal_string_data( s ),
				               iota::get_pascal_string_size( s ) );
			}
			
		#endif
	};
	
}

#endif
