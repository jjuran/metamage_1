/*
	string.hh
	---------
	
	Copyright 2010, Joshua Juran
*/

#ifndef PLUS_STRING_HH
#define PLUS_STRING_HH


namespace plus
{
	
	enum delete_policy
	{
		delete_never,  // propagates, for static storage like argv members
		delete_shared, // Refcounted delete, for everything by default
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
				min_small_string_length = 15,
				buffer_size_in_longs    = min_small_string_length / sizeof (long) + 1,
				buffer_size             = buffer_size_in_longs * sizeof (long),
				max_offset              = buffer_size - 1
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
		
		protected:
			char* reallocate( size_type length );
			
			void set_length( size_type length );
			
			void copy_on_write();
		
		public:
			static const size_type npos = size_type( -1 );
			
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
			
			bool empty() const  { return length() == 0; }
			
			size_type length() const;
			
			size_type size() const  { return length(); }
			
			size_type capacity() const;
			
			const char* data() const;
			
			const char* c_str() const  { return data(); }
			
			operator const char*() const  { return c_str(); }
			
			const char* begin() const  { return data(); }
			
			const char* end() const;
			
			const char& front() const  { return begin()[  0 ]; }
			const char& back () const  { return end  ()[ -1 ]; }
			
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
			
			string& assign( const string& other, size_type pos, size_type n = npos );
			string& assign( const string& other );
			
			void clear()  { reallocate( 0 ); }
			
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
	
	
	string concat( const char* a, string::size_type  a_size,
	               const char* b, string::size_type  b_size );
	
	string operator+( const string& a, const string& b );
	
	string operator+( const string& a, const char* b );
	
	string operator+( const char* a, const string& b );
	
	
	inline void swap( string& a, string& b )
	{
		a.swap( b );
	}
	
}

#endif

