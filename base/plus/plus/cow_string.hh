/*
	cow_string.hh
	-------------
*/

#ifndef PLUS_COWSTRING_HH
#define PLUS_COWSTRING_HH

// plus
#include "plus/string.hh"
#include "plus/string_common.hh"


namespace plus
{
	
	class cow_string : public string
	{
		private:
			char* mutable_data()  { return copy_on_write( false ); }
			
			char* embiggen( size_type new_length );
			
			char* insert_uninitialized( char* p, size_type n );
			
			char* erase_unchecked( char* p, size_type n );
			
			char* replace_setup( char* p, size_type m, difference_type delta );
		
		public:
			typedef char&  reference;
			typedef char*  pointer;
			typedef char*  iterator;
			
			cow_string()
			{
			}
			
			cow_string( const char*    p,
			            size_type      length,
			            delete_policy  policy ) : string( p, length, policy )
			{
			}
			
			cow_string( const char* p, size_type length ) : string( p, length )
			{
			}
			
			cow_string( const char* p, const char* q ) : string( p, q )
			{
			}
			
			cow_string( const char* s ) : string( s )
			{
			}
			
			cow_string( size_type n, char c ) : string( n, c )
			{
			}
			
			cow_string( datum_movable& m ) : string( m )
			{
			}
			
			cow_string( const string& other, size_type pos, size_type n = npos )
			:
				string( other, pos, n )
			{
			}
			
			cow_string( const string& other ) : string( other )
			{
			}
			
		#if IOTA_HAS_PASCAL_STRINGS
			
			cow_string( const unsigned char* other )
			{
				assign( other );
			}
			
		#endif
			
			cow_string& operator=( const cow_string& other )
			{
				return assign( other );
			}
			
			cow_string& operator=( const string& other )
			{
				return assign( other );
			}
			
			cow_string& operator=( datum_movable& m )
			{
				return assign( m );
			}
			
			cow_string& operator=( const char* s )  { return assign( s ); }
			
			cow_string& operator=( char c )  { return assign( 1, c ); }
			
		#if IOTA_HAS_PASCAL_STRINGS
			
			cow_string& operator=( const unsigned char* other )
			{
				return assign( other );
			}
			
		#endif
			
			const char* begin() const  { return string::begin(); }
			const char* end  () const  { return string::end  (); }
			
			char* begin()  { return copy_on_write( true ); }
			
			char* end();
			
			char& front()  { return begin()[  0 ]; }
			char& back ()  { return end  ()[ -1 ]; }
			
			char& operator[]( size_type i )  { return begin()[ i ]; }
			
			cow_string& assign( const char*    p,
			                    size_type      length,
			                    delete_policy  policy,
			                    size_type      capacity = 0 )
			{
				string::assign( p, length, policy, capacity );
				
				return *this;
			}
			
			cow_string& assign( const char* p, size_type length )
			{
				string::assign( p, length );
				
				return *this;
			}
			
			cow_string& assign( const char* p, const char* q )
			{
				string::assign( p, q );
				
				return *this;
			}
			
			cow_string& assign( const char* s )
			{
				string::assign( s );
				
				return *this;
			}
			
			cow_string& assign( size_type n, char c )
			{
				string::assign( n, c );
				
				return *this;
			}
			
			cow_string& assign( datum_movable& m )
			{
				string::assign( m );
				
				return *this;
			}
			
			cow_string& assign( const string& other, size_type pos, size_type n = npos )
			{
				// Always allocates, even with pos == 0 and n == npos
				
				string::assign( other, pos, n );
				
				return *this;
			}
			
			cow_string& assign( const string& other )
			{
				string::assign( other );
				
				return *this;
			}
			
		#if IOTA_HAS_PASCAL_STRINGS
			
			cow_string& assign( const unsigned char* other )
			{
				return assign( iota::get_pascal_string_data( other ),
				               iota::get_pascal_string_size( other ) );
			}
			
		#endif
			
			cow_string& erase( size_type pos = 0, size_type n = npos );
			
			char* erase( char* p, char* q );
			
			char* erase( char* p )  { return erase( p, p + 1 ); }
			
			void push_back( char c )  { append( 1, c ); }
			void pop_back()           { resize( size() - 1 ); }
			
			void clear()  { resize( 0 ); }
			
			void reserve( size_type size )  { string_reserve( store, size ); }
			void resize ( size_type size, char c = '\0' );
			
			cow_string& insert( size_type pos, const string& s );
			cow_string& insert( size_type pos, const string& s, size_type begin, size_type n );
			
			cow_string& insert( size_type pos, const char* s, size_type n );
			cow_string& insert( size_type pos, const char* s );
			
			cow_string& insert( size_type pos, size_type n, char c );
			
			void insert( char* p, char* i, char* j );
			
			void insert( char* p, size_type n, char c );
			
			char* insert( char* p, char c );
			
			cow_string& append( const char* p, size_type length );
			
			cow_string& append( const char* p, const char* q )
			{
				return append( p, q - p );
			}
			
			cow_string& append( const char* s );
			
			cow_string& append( size_type n, char c );
			
			cow_string& append( const string& other )
			{
				return append( other.data(), other.size() );
			}
			
			cow_string& append( const string& other, size_type pos, size_type n );
			
			cow_string& replace( size_type pos, size_type m, const string& s );
			cow_string& replace( size_type pos, size_type m, const string& s, size_type offset, size_type n );
			
			cow_string& replace( size_type pos, size_type m, const char* s, size_type n );
			cow_string& replace( size_type pos, size_type m, const char* s );
			
			cow_string& replace( size_type pos, size_type m, size_type n, char c );
			
			void replace( char* p, char* q, const string& s );
			
			void replace( char* p, char* q, const char *i, size_type n );
			void replace( char* p, char* q, const char *s );
			
			void replace( char* p, char* q, size_type n, char c );
			
			void replace( char* p, char* q, const char *i, const char *j );
			
			cow_string& operator+=( const string& s )  { return append( s ); }
			cow_string& operator+=( const char*   s )  { return append( s ); }
			cow_string& operator+=( char          c )  { return append( 1, c ); }
			
		#if IOTA_HAS_PASCAL_STRINGS
			
			cow_string& operator+=( const unsigned char* s )
			{
				return append( iota::get_pascal_string_data( s ),
				               iota::get_pascal_string_size( s ) );
			}
			
		#endif
	};
	
}

#endif

