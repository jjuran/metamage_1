/*
	string.hh
	---------
*/

#ifndef PLUS_STRING_HH
#define PLUS_STRING_HH

// iota
#include "iota/pascal_string.hh"
#include "iota/swap.hh"

// vxo
#include "vxo/datum_access.hh"
#include "vxo/datum_alloc.hh"

// plus
#include "plus/string_common.hh"


namespace plus
{
	
	typedef signed char cmp_t;
	
	class string
	{
		public:
			typedef vxo::datum_movable datum_movable;
			typedef vxo::delete_policy delete_policy;
			
			typedef char value_type;
			
			typedef unsigned long  size_type;
			typedef long           difference_type;
			
			typedef const char&  reference;
			typedef const char&  const_reference;
			typedef const char*  pointer;
			typedef const char*  const_pointer;
			typedef const char*  iterator;
			typedef const char*  const_iterator;
			
			static size_type max_size()
			{
				// 32 or 64
				const int n_bits = sizeof (size_type) * 8;
				
				// 0x7fffffff[ffffffff]
				const size_type max = (size_type( 1 ) << (n_bits - 1)) - 1;
				
				return max;
			}
			
			static void check_size( size_type size )  { string_check_size( size ); }
			
			enum
			{
				max_offset = vxo::datum_buffer_size - 1
			};
		
		protected:
			vxo::datum_storage store;
		
		private:
			// The _ is mnemonic for ~
			signed char _policy() const  { return store.small[ max_offset ]; }
			
			void _policy( char negated )  { store.small[ max_offset ] = negated; }
		
		protected:
			char* copy_on_write( bool tainting )
			{
				return vxo::copy_on_write( store, tainting );
			}
		
		public:
			static const size_type npos = size_type( -1 );
			
			static const string null;
			
			string()
			{
				construct_from_default( store );
			}
			
			string( const char* p, size_type length, delete_policy policy, size_type capacity = 0 );
			
			string( const char* p, size_type length );
			
			string( const char* p, const char* q );
			
			string( const char* s );
			
			string( size_type n, char c );
			
			~string()
			{
				if ( has_dynamic_extent( store ) )
				{
					destroy( store );
				}
			}
			
			string( datum_movable& m )
			{
				construct_from_move_untaint( store, m );
			}
			
			string( const string& other, size_type pos, size_type n = npos );
			
		#if IOTA_HAS_PASCAL_STRINGS
			
			string( const unsigned char* other )
			{
				store.small[ max_offset ] = max_offset;  // empty string
				
				assign( other );
			}
			
		#endif
			
			string( const string& other )
			{
				construct_from_copy( store, other.store );
			}
			
			string& operator=( const string& other )
			{
				return assign( other );
			}
			
			string& operator=( datum_movable& m )
			{
				return assign( m );
			}
			
		#if IOTA_HAS_PASCAL_STRINGS
			
			string& operator=( const unsigned char* other )
			{
				return assign( other );
			}
			
		#endif
			
			bool is_small() const  { return (unsigned char) _policy() <= max_offset; }
			
			bool empty() const  { return size() == 0; }
			
			size_type size() const  { return vxo::size( store ); }
			
			size_type length() const  { return size(); }
			
			size_type capacity() const  { return vxo::capacity( store ); }
			
			const char* data() const  { return vxo::begin( store ); }
			
			const char* c_str() const;
			
			const char* begin() const  { return data(); }
			
			const char* end() const;
			
			const char& front() const  { return begin()[  0 ]; }
			const char& back () const  { return end  ()[ -1 ]; }
			
			const char& operator[]( size_type i ) const  { return data()[ i ]; }
			
			string& assign( const char*    p,
			                size_type      length,
			                delete_policy  policy,
			                size_type      capacity = 0 );
			
			string& assign( const char* p, size_type length );
			
			string& assign( const char* p, const char* q )
			{
				return assign( p, q - p );
			}
			
			string& assign( const char* s );
			
			string& assign( size_type n, char c );
			
			string& assign( datum_movable& m );
			
			string& assign( const string& other )
			{
				assign_from_copy( store, other.store );
				
				return *this;
			}
			
			string& assign( const string& other, size_type pos, size_type n = npos );
			
		#if IOTA_HAS_PASCAL_STRINGS
			
			string& assign( const unsigned char* other )
			{
				return assign( iota::get_pascal_string_data( other ),
				               iota::get_pascal_string_size( other ) );
			}
			
		#endif
			
			char* reset_nothrow( size_type n = 0 );
			char* reset        ( size_type n = 0 );
			
			string& operator=( const char* s )  { return assign( s ); }
			
			string& operator=( char c )  { return assign( 1, c ); }
			
			void swap( string& other )
			{
				using iota::swap;
				
				swap( store, other.store );
			}
			
			datum_movable& move()  { return (datum_movable&) store; }
			
			friend datum_movable& move( string& s )  { return s.move(); }
			
			friend unsigned long area( const string& s )  { return area( s.store ); }
			
			size_type copy( char* buffer, size_type n, size_type pos = 0 ) const;
			
			size_type find( const string& s, size_type pos = 0 ) const;
			size_type find( const char*   s, size_type pos = 0 ) const;
			
			size_type find( const char* s, size_type pos, size_type n ) const;
			
			size_type find( char c, size_type pos = 0 ) const;
			
			size_type rfind( const string& s, size_type pos = npos ) const;
			size_type rfind( const char*   s, size_type pos = npos ) const;
			
			size_type rfind( const char* s, size_type pos, size_type n ) const;
			
			size_type rfind( char c, size_type pos = npos ) const;
			
			size_type find_first_of( const string& s, size_type pos = 0 ) const;
			size_type find_first_of( const char*   s, size_type pos = 0 ) const;
			
			size_type find_first_of( const char* s, size_type pos, size_type n ) const;
			
			size_type find_first_of( char c, size_type pos = 0 ) const;
			
			size_type find_last_of( const string& s, size_type pos = npos ) const;
			size_type find_last_of( const char*   s, size_type pos = npos ) const;
			
			size_type find_last_of( const char* s, size_type pos, size_type n ) const;
			
			size_type find_last_of( char c, size_type pos = npos ) const;
			
			size_type find_first_not_of( const string& s, size_type pos = 0 ) const;
			size_type find_first_not_of( const char*   s, size_type pos = 0 ) const;
			
			size_type find_first_not_of( const char* s, size_type pos, size_type n ) const;
			
			size_type find_first_not_of( char c, size_type pos = 0 ) const;
			
			size_type find_last_not_of( const string& s, size_type pos = npos ) const;
			size_type find_last_not_of( const char*   s, size_type pos = npos ) const;
			
			size_type find_last_not_of( const char* s, size_type pos, size_type n ) const;
			
			size_type find_last_not_of( char c, size_type pos = npos ) const;
			
			string substr( size_type pos = 0, size_type n = npos ) const;
			
			int compare( const string& b ) const;
			int compare( size_type a_pos, size_type a_n, const string& b ) const;
			int compare( size_type a_pos, size_type a_n, const string& b, size_type b_pos, size_type b_n ) const;
			
			int compare( const char* s ) const;
			int compare( size_type a_pos, size_type a_n, const char* b, size_type b_n = npos ) const;
	};
	
	
	cmp_t compare( const string& a, const string& b );
	
	bool operator==( const string& a, const string& b );
	
	bool operator==( const string& a, const char* b );
	
	inline bool operator==( const char* a, const string& b )
	{
		return b == a;
	}
	
	
	inline bool operator!=( const string& a, const string& b )
	{
		return !( a == b );
	}
	
	inline bool operator!=( const string& a, const char* b )
	{
		return !( a == b );
	}
	
	inline bool operator!=( const char* a, const string& b )
	{
		return !( a == b );
	}
	
	
	bool operator<( const string& a, const string& b );
	
	bool operator<( const string& a, const char* b );
	
	bool operator<( const char* a, const string& b );
	
	
	inline bool operator>( const string& a, const string& b )
	{
		return b < a;
	}
	
	inline bool operator>( const string& a, const char* b )
	{
		return b < a;
	}
	
	inline bool operator>( const char* a, const string& b )
	{
		return b < a;
	}
	
	
	inline bool operator<=( const string& a, const string& b )
	{
		return !( a > b );
	}
	
	inline bool operator<=( const string& a, const char* b )
	{
		return !( a > b );
	}
	
	inline bool operator<=( const char* a, const string& b )
	{
		return !( a > b );
	}
	
	
	inline bool operator>=( const string& a, const string& b )
	{
		return !( a < b );
	}
	
	inline bool operator>=( const string& a, const char* b )
	{
		return !( a < b );
	}
	
	inline bool operator>=( const char* a, const string& b )
	{
		return !( a < b );
	}
	
	
	inline void swap( string& a, string& b )
	{
		a.swap( b );
	}
	
	inline const char* get_string_c_str( const string& s )
	{
		return s.c_str();
	}
	
	inline const char* get_string_data( const string& s )
	{
		return s.data();
	}
	
	inline string::size_type get_string_size( const string& s )
	{
		return s.size();
	}
	
}

#endif
