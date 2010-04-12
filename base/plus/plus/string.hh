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
		delete_none,
		delete_basic
	};
	
	
	class string
	{
		public:
			typedef unsigned long size_type;
			
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
			};
			
			union
			{
				char         its_small_name[ buffer_size ];
				alloc_state  its_alloc;
				long         its_longs[ buffer_size_in_longs ];
			};
		
		public:
			string()
			{
				its_small_name[ max_offset ] = 0;
			}
			
			string( const char* p, size_type length, delete_policy policy );
			
			string( const char* p, size_type length );
			
			string( const char* p, const char* q );
			
			string( const char* s );
			
			~string();
			
			string           ( const string& other );
			string& operator=( const string& other );
			
			bool empty() const  { return length() == 0; }
			
			size_type length() const;
			
			size_type size() const  { return length(); }
			
			const char* data() const;
			
			const char* c_str() const  { return data(); }
			
			operator const char*() const  { return c_str(); }
			
			void assign( const char* p, size_type length, delete_policy policy );
			
			void assign( const char* p, size_type length );
			
			void assign( const char* p, const char* q )  { assign( p, q - p ); }
			
			void assign( const char* s );
			
			void assign( const string& other )  { assign( other.data(), other.size() ); }
			
			void append( const char* p, size_type length );
			
			void append( const char* p, const char* q )  { append( p, q - p ); }
			
			void append( const char* s );
			
			void append( const string& other )  { append( other.data(), other.size() ); }
			
			string& operator=( const char* s )  { assign( s );  return *this; }
			
			string& operator+=( const string& s )  { append( s );  return *this; }
			string& operator+=( const char*   s )  { append( s );  return *this; }
			
			void swap( string& other );
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

