/*
	string.cc
	---------
	
	Some of the find-related member functions were written using 
	boost/shmem/containers/string.hpp as a guide.
*/

#include "plus/string.hh"

// Standard C++
#include <algorithm>
#include <stdexcept>

// Standard C
#include <stdlib.h>
#include <string.h>

// more-libc
#include "more/string.h"

// gear
#include "gear/compare.hh"

// debug
#include "debug/assert.hh"

// plus
#include "plus/string_details.hh"


#if defined(__MWERKS__)  &&  __MWERKS__ < 0x2300
	#define THROW( type, s )  throw std::exception()
#else
	#define THROW( type, s )  throw std::type( s )
#endif


namespace plus
{
	
	using vxo::destroy;  // disambiguate from std::destroy() for CW Pro 4
	
	const string string::null;
	
	
	static inline
	cmp_t string_compare( const char* a, size_t a_size,
	                      const char* b, size_t b_size )
	{
		using gear::compare_bytes;
		using gear::byte_t;
		
		return compare_bytes( (const byte_t*) a, a_size,
		                      (const byte_t*) b, b_size );
	}
	
	static
	bool string_equal( const char* a, size_t a_size,
	                   const char* b, size_t b_size )
	{
		return a_size == b_size  &&  string_compare( a, a_size,
		                                             b, b_size ) == 0;
	}
	
	
	class matches_any
	{
		private:
			const char*  begin;
			const char*  end;
		
		public:
			matches_any( const char* a, const char* z ) : begin( a ), end( z )
			{
			}
			
			bool operator()( char c ) const
			{
				return std::find( begin, end, c ) != end;
			}
	};
	
	class matches_none
	{
		private:
			const char*  begin;
			const char*  end;
		
		public:
			matches_none( const char* a, const char* z ) : begin( a ), end( z )
			{
			}
			
			bool operator()( char c ) const
			{
				return std::find( begin, end, c ) == end;
			}
	};
	
	
	template < class Iter, class F >
	static Iter find_last_if( Iter begin, Iter end, F f )
	{
		ASSERT( begin <= end );
		
		Iter it = end;
		
		while ( --it >= begin )
		{
			if ( f( *it ) )
			{
				return it;
			}
		}
		
		return end;
	}
	
	
	string::string( const char*    p,
	                size_type      length,
	                delete_policy  policy,
	                size_type      capacity )
	{
		/*
			Sanity-check length and capacity against max_size().
			
			We were throwing exceptions on failure, which requires a
			dependency on the C++ exception runtime.  In practice, an
			out-of-bounds value will never be provided by correct code.
			
			The maximum value is half the size of the address space.
			This is not an allocation request like malloc(), where the
			caller can conceivably ask for anything representable by the
			integer type.  Rather, the caller is providing the length of
			an allocation that already exists.
			
			While a plus::string may contain arbitrary byte values, and
			a plus::string constructed using a delete_never policy can
			refer to arbitrary memory, it's out of scope for plus::string
			to refer to vast swaths of memory that don't correspond to a
			single object.  The delete_never policy is meant for members
			of argv and statically allocated storage that will persist for
			the life of the process (or, on rare occasion, system-owned
			memory at a fixed address, like the machine name string whose
			address is returned when calling Gestalt() with a selector of
			'mnam').
			
			In classic Mac OS, malloc() is provided by MSL and backed by
			NewPtr(), which is limited by the application's heap size.
			In Mac OS X, it appears that NewPtr() tops out near 1 GiB.
			I'm not sure what other OSes like Linux and BSD do on 32-bit
			machines, but the prospect of tending multi-gigabyte workloads
			on them is vanishingly small.
			
			On a 64-bit system, there won't be enough /disk space/ to even
			come close to the max size -- half the address space is 8 EiB.
		*/
		
		ASSERT( length   <= max_size() );
		ASSERT( capacity <= max_size() );
		
	#ifndef __LP64__
	#ifndef __APPLE__
	#ifndef __MACOS__
		
		check_size( length   );
		check_size( capacity );
		
	#endif
	#endif
	#endif
		
		store.alloc.pointer  = p;
		store.alloc.length   = length;
		store.alloc.capacity = capacity;  // may be zero
		
		_policy( ~policy );
	}
	
	string::string( const char* p, size_type length )
	{
		ASSERT( p != NULL );
		
		ASSERT( p + length >= p );
		
		check_size( length );
		
		allocate_data( store, p, length );
	}
	
	/*
		Regarding the next three constructors:
		
		An uninitialized 15-char string is acceptable because we're just
		going to assign over it anyway, and empties aren't faster here.
	*/
	
	string::string( const char* p, const char* q )
	{
		store.small[ max_offset ] = 0;
		
		assign( p, q );
	}
	
	string::string( const char* s )
	{
		store.small[ max_offset ] = 0;
		
		assign( s );
	}
	
	string::string( size_type n, char c )
	{
		store.small[ max_offset ] = 0;
		
		assign( n, c );
	}
	
	string::string( const string& other, size_type pos, size_type n )
	{
		construct_from_move( store, other.substr( pos, n ).move() );
	}
	
	const char* string::c_str() const
	{
		using vxo::datum_storage;
		
		datum_storage& u = const_cast< datum_storage& >( store );
		
		char* last = u.small + max_offset;
		
		if ( (int8_t) *last >= 0 )
		{
			char* tail = last - *last;
			
			*tail = '\0';
			
			return store.small;  // always terminated
		}
		
		const char* begin = store.alloc.pointer + alloc_substr_offset( store );
		
		if ( begin[ store.alloc.length ] == '\0' )
		{
			return begin;
		}
		
		datum_storage temp;
		
		char* p = allocate_data( temp, begin, store.alloc.length );
		
		destroy( u );
		
		u = temp;
		
		ASSERT( !is_small() );
		
		return p;
	}
	
	const char* string::end() const
	{
		return begin() + size();
	}
	
	string& string::assign( const char* p, size_type length, delete_policy policy, size_type capacity )
	{
		check_size( length   );
		check_size( capacity );
		
		if ( length )
		{
			ASSERT( p != NULL );
			
			ASSERT( p + length >= p );
		}
		
		destroy( store );
		
		store.alloc.pointer  = p;
		store.alloc.length   = length;
		store.alloc.capacity = capacity;  // may be zero
		
		_policy( ~policy );
		
		return *this;
	}
	
	char* string::reset_nothrow( size_type length )
	{
		if ( length > max_size() )
		{
			return NULL;
		}
		
		return reallocate_nothrow( store, length );
	}
	
	char* string::reset( size_type length )
	{
		check_size( length );
		
		return reallocate( store, length );
	}
	
	string& string::assign( const char* p, size_type length )
	{
		return assign( string( p, length ).move() );
	}
	
	string& string::assign( const char* s )
	{
		ASSERT( s != NULL );
		
		const size_type length = strlen( s );
		
		return assign( s, length );
	}
	
	string& string::assign( size_type n, char c )
	{
		// reset() will throw if n exceeds max_size()
		
		char* pointer = reset( n );
		
		memset( pointer, c, n );
		
		return *this;
	}
	
	string& string::assign( datum_movable& m )
	{
		if ( &m != &store )
		{
			destroy( store );
			
			construct_from_move_untaint( store, m );
		}
		
		return *this;
	}
	
	string& string::assign( const string& other, size_type pos, size_type n )
	{
		return assign( other.substr( pos, n ).move() );
	}
	
	
	string::size_type string::copy( char* buffer, size_type n, size_type pos ) const
	{
		const size_t size = length();
		
		if ( pos > size )
		{
			THROW( out_of_range, "string::copy" );
		}
		
		n = std::min( n, size - pos );
		
		mempcpy( buffer, data() + pos, n );
		
		return n;
	}
	
	
	string::size_type string::find( const string& s, size_type pos ) const
	{
		return find( s.data(), pos, s.size() );
	}
	
	string::size_type string::find( const char* s, size_type pos ) const
	{
		return find( s, pos, strlen( s ) );
	}
	
	string::size_type string::find( const char* s, size_type pos, size_type n ) const
	{
		const size_t size = length();
		
		if ( pos > size  ||  pos + n > size )
		{
			return npos;
		}
		
		if ( n == 0 )
		{
			return pos;
		}
		
		const char* begin = data();
		const char* end   = begin + size;
		
		const char* it = std::search( begin + pos, end, s, s + n );
		
		return it != end ? it - begin : npos;
	}
	
	string::size_type string::find( char c, size_type pos ) const
	{
		return find( &c, pos, 1 );
	}
	
	
	string::size_type string::rfind( const string& s, size_type pos ) const
	{
		return rfind( s.data(), pos, s.size() );
	}
	
	string::size_type string::rfind( const char* s, size_type pos ) const
	{
		return rfind( s, pos, strlen( s ) );
	}
	
	string::size_type string::rfind( const char* s, size_type pos, size_type n ) const
	{
		const size_type size = length();
		
		if ( n > size )
		{
			return npos;
		}
		
		pos = std::min( pos, size - n );
		
		if ( n == 0 )
		{
			return pos;
		}
		
		const char* begin = data();
		const char* end   = begin + pos + n;
		
		const char* it = std::find_end( begin, end, s, s + n );
		
		return it != end ? it - begin : npos;
	}
	
	string::size_type string::rfind( char c, size_type pos ) const
	{
		return rfind( &c, pos, 1 );
	}
	
	
	string::size_type string::find_first_of( const string& s, size_type pos ) const
	{
		return find_first_of( s.data(), pos, s.size() );
	}
	
	string::size_type string::find_first_of( const char* s, size_type pos ) const
	{
		return find_first_of( s, pos, strlen( s ) );
	}
	
	string::size_type string::find_first_of( const char* s, size_type pos, size_type n ) const
	{
		const size_t size = length();
		
		if ( pos >= size )
		{
			return npos;
		}
		
		const char* begin = data();
		const char* end   = begin + size;
		
		const char* it = std::find_first_of( begin + pos, end, s, s + n );
		
		return it != end ? it - begin : npos;
	}
	
	string::size_type string::find_first_of( char c, size_type pos ) const
	{
		return find( c, pos );
	}
	
	
	string::size_type string::find_last_of( const string& s, size_type pos ) const
	{
		return find_last_of( s.data(), pos, s.size() );
	}
	
	string::size_type string::find_last_of( const char* s, size_type pos ) const
	{
		return find_last_of( s, pos, strlen( s ) );
	}
	
	string::size_type string::find_last_of( const char* s, size_type pos, size_type n ) const
	{
		const size_type size = length();
		
		if ( 1 > size )
		{
			return npos;
		}
		
		pos = std::min( pos, size - 1 );
		
		const char* begin = data();
		const char* end   = begin + pos + 1;
		
		const char* it = find_last_if( begin, end, matches_any( s, s + n ) );
		
		return it != end ? it - begin : npos;
	}
	
	string::size_type string::find_last_of( char c, size_type pos ) const
	{
		return find_last_of( &c, pos, 1 );
	}
	
	
	string::size_type string::find_first_not_of( const string& s, size_type pos ) const
	{
		return find_first_not_of( s.data(), pos, s.size() );
	}
	
	string::size_type string::find_first_not_of( const char* s, size_type pos ) const
	{
		return find_first_not_of( s, pos, strlen( s ) );
	}
	
	string::size_type string::find_first_not_of( const char* s, size_type pos, size_type n ) const
	{
		const size_t size = length();
		
		if ( pos >= size )
		{
			return npos;
		}
		
		const char* begin = data();
		const char* end   = begin + size;
		
		const char* it = std::find_if( begin + pos, end, matches_none( s, s + n ) );
		
		return it != end ? it - begin : npos;
	}
	
	string::size_type string::find_first_not_of( char c, size_type pos ) const
	{
		return find_first_not_of( &c, pos, 1 );
	}
	
	
	string::size_type string::find_last_not_of( const string& s, size_type pos ) const
	{
		return find_last_not_of( s.data(), pos, s.size() );
	}
	
	string::size_type string::find_last_not_of( const char* s, size_type pos ) const
	{
		return find_last_not_of( s, pos, strlen( s ) );
	}
	
	string::size_type string::find_last_not_of( const char* s, size_type pos, size_type n ) const
	{
		const size_type size = length();
		
		if ( 1 > size )
		{
			return npos;
		}
		
		pos = std::min( pos, size - 1 );
		
		const char* begin = data();
		const char* end   = begin + pos + 1;
		
		const char* it = find_last_if( begin, end, matches_none( s, s + n ) );
		
		return it != end ? it - begin : npos;
	}
	
	string::size_type string::find_last_not_of( char c, size_type pos ) const
	{
		return find_last_not_of( &c, pos, 1 );
	}
	
	
	string string::substr( size_type pos, size_type n ) const
	{
		const size_type len = size();
		
		if ( pos > len )
		{
			THROW( out_of_range, "string::substr" );
		}
		
		if ( len - pos < n )
		{
			n = len - pos;
		}
		
		if ( n > vxo::datum_max_offset  &&  _policy() >= ~vxo::delete_shared )
		{
			plus::string temp = *this;
			
			const long offset = alloc_substr_offset( store );
			
			temp.store.alloc.length   = n;
			temp.store.alloc.capacity = -(offset + pos);
			
			return temp.move();
		}
		
		return string( data() + pos, n );
	}
	
	
	int string::compare( const string& b ) const
	{
		return string_compare( data(), size(), b.data(), b.size() );
	}
	
	int string::compare( size_type a_pos, size_type a_n, const string& b ) const
	{
		const size_type a_size = size();
		
		if ( a_pos > a_size )
		{
			THROW( out_of_range, "string::compare" );
		}
		
		a_n = std::min( a_n, a_size - a_pos );
		
		return string_compare( data() + a_pos, a_n, b.data(), b.size() );
	}
	
	int string::compare( size_type a_pos, size_type a_n, const string& b, size_type b_pos, size_type b_n ) const
	{
		const size_type a_size = size();
		
		if ( a_pos > a_size )
		{
			THROW( out_of_range, "string::compare" );
		}
		
		a_n = std::min( a_n, a_size - a_pos );
		
		const size_type b_size = b.size();
		
		if ( b_pos > b_size )
		{
			THROW( out_of_range, "string::compare" );
		}
		
		b_n = std::min( b_n, b_size - b_pos );
		
		return string_compare( data() + a_pos, a_n, b.data() + b_pos, b_n );
	}
	
	int string::compare( const char* b ) const
	{
		return string_compare( data(), size(), b, strlen( b ) );
	}
	
	int string::compare( size_type a_pos, size_type a_n, const char* b, size_type b_n ) const
	{
		const size_type a_size = size();
		
		if ( a_pos > a_size )
		{
			THROW( out_of_range, "string::compare" );
		}
		
		a_n = std::min( a_n, a_size - a_pos );
		
		b_n = std::min< size_type >( b_n, strlen( b ) );
		
		return string_compare( data(), size(), b, b_n );
	}
	
	
	cmp_t compare( const string& a, const string& b )
	{
		return string_compare( a.data(), a.size(), b.data(), b.size() );
	}
	
	bool operator==( const string& a, const string& b )
	{
		return string_equal( a.data(), a.size(), b.data(), b.size() );
	}
	
	bool operator==( const string& a, const char* b )
	{
		return string_equal( a.data(), a.size(), b, strlen( b ) );
	}
	
	
	bool operator<( const string& a, const string& b )
	{
		return string_compare( a.data(), a.size(), b.data(), b.size() ) < 0;
	}
	
	bool operator<( const string& a, const char* b )
	{
		return string_compare( a.data(), a.size(), b, strlen( b ) ) < 0;
	}
	
	bool operator<( const char* a, const string& b )
	{
		return string_compare( a, strlen( a ), b.data(), b.size() ) < 0;
	}
	
}
