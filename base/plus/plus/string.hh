/*
	string.hh
	---------
	
	Copyright 2010, Joshua Juran
*/

#ifndef PLUS_STRING_HH
#define PLUS_STRING_HH

// iota
#include "iota/string_traits.hh"


namespace plus
{
	
	enum delete_policy
	{
		delete_never,  // propagates, for static storage like argv members
		delete_shared, // Refcounted delete, for everything by default
		delete_owned,  // Stored as shared, but can't be shared again
		delete_none,   // copies will reallocate, for stack-based storage
		delete_basic   // Standard-issue delete, for caller-supplied handoffs
	};
	
	
	class string
	{
		public:
			typedef char value_type;
			
			typedef unsigned long  size_type;
			typedef long           difference_type;
			
			typedef const char&  reference;
			typedef const char&  const_reference;
			typedef const char*  pointer;
			typedef const char*  const_pointer;
			typedef const char*  iterator;
			typedef const char*  const_iterator;
			
			enum
			{
				buffer_size_in_longs = 4,  // ptr/len/cap/etc
				buffer_size          = buffer_size_in_longs * sizeof (long),
				max_offset           = buffer_size - 1
			};
		
		private:
			struct alloc_state
			{
				const char*  pointer;
				size_type    length;
				size_type    capacity;
				char         _12;
				char         _13;
				char         _14;
				char         _policy;  // The _ is mnemonic for ~
			};
			
			union
			{
				char         its_small_name[ buffer_size ];
				alloc_state  its_alloc;
				long         its_longs[ buffer_size_in_longs ];
			};
			
			// The _ is mnemonic for ~
			char  _policy() const  { return its_small_name[ max_offset ]; }
			char& _policy()        { return its_small_name[ max_offset ]; }
		
		protected:
			char* reallocate( size_type length );
			
			void set_length( size_type length );
			
			char* copy_on_write( bool tainting );
		
		public:
			static const size_type npos = size_type( -1 );
			
			static const string null;
			
			string()
			{
				its_small_name[ 0          ] = '\0';
				its_small_name[ max_offset ] = max_offset;
			}
			
			string( const char* p, size_type length, delete_policy policy, size_type capacity = 0 );
			
			string( const char* p, size_type length );
			
			string( const char* p, const char* q );
			
			string( const char* s );
			
			string( size_type n, char c );
			
			~string();
			
			string( const string& other, size_type pos, size_type n = npos );
			
			string           ( const string& other );
			string& operator=( const string& other );
			
			static size_type max_size()
			{
				// 32 or 64
				const int n_bits = sizeof (size_type) * 8;
				
				// 0x7fffffff[ffffffff]
				const size_type max = (size_type( 1 ) << n_bits - 1) - 1;
				
				return max;
			}
			
			bool empty() const  { return size() == 0; }
			
			size_type size() const;
			
			size_type length() const  { return size(); }
			
			size_type capacity() const;
			
			const char* data() const;
			
			const char* c_str() const  { return data(); }
			
			const char* begin() const  { return data(); }
			
			const char* end() const;
			
			const char& front() const  { return begin()[  0 ]; }
			const char& back () const  { return end  ()[ -1 ]; }
			
			const char& operator[]( size_type i ) const  { return data()[ i ]; }
			
			string& assign( const char*    p,
			                size_type      length,
			                delete_policy  policy,
			                size_type      capacity = 0 );
			
			string& assign( const char* p, size_type length, size_type capacity = 0 );
			
			string& assign( const char* p, const char* q )
			{
				return assign( p, q - p );
			}
			
			string& assign( const char* s );
			
			string& assign( size_type n, char c );
			
			string& assign( const string& other, size_type pos, size_type n = npos );
			string& assign( const string& other );
			
			char* reset( size_type n = 0 )  { return reallocate( n ); }
			
			string& operator=( const char* s )  { return assign( s ); }
			
			string& operator=( char c )  { return assign( 1, c ); }
			
			void swap( string& other );
			
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
			
			string substr( size_type pos = 0, size_type n = npos ) const
			{
				return string( *this, pos, n );
			}
			
			int compare( const string& b ) const;
			int compare( size_type a_pos, size_type a_n, const string& b ) const;
			int compare( size_type a_pos, size_type a_n, const string& b, size_type b_pos, size_type b_n ) const;
			
			int compare( const char* s ) const;
			int compare( size_type a_pos, size_type a_n, const char* b, size_type b_n = npos ) const;
	};
	
	
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
	
	
	string concat( const char*  a, string::size_type  a_size,
	               const char*  b, string::size_type  b_size );
	
	template < class A >
	string concat( const A&     a,
	               const char*  b, string::size_type  b_size )
	{
		return concat( iota::get_string_data( a ),
		               iota::get_string_size( a ),
		               b,
		               b_size );
	}
	
	template < class B >
	string concat( const char*  a, string::size_type a_size,
	               const B&     b )
	{
		return concat( a,
		               a_size,
		               iota::get_string_data( b ),
		               iota::get_string_size( b ) );
	}
	
	template < class A, class B >
	string concat( const A&  a,
	               const B&  b )
	{
		return concat( iota::get_string_data( a ),
		               iota::get_string_size( a ),
		               iota::get_string_data( b ),
		               iota::get_string_size( b ) );
	}
	
	string operator+( const string& a, const string& b );
	
	string operator+( const string& a, const char* b );
	
	string operator+( const char* a, const string& b );
	
	
	inline void swap( string& a, string& b )
	{
		a.swap( b );
	}
	
}

#endif

