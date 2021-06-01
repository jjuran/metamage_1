/*
	PackBits.cc
	-----------
*/

#include "varyx/mac/PackBits.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
	#if __LP64__  ||  MAC_OS_X_VERSION_10_7
	extern "C" void PackBits( Ptr* src, Ptr* dst, short srcBytes );
	extern "C" void UnpackBits( Ptr* src, Ptr* dst, short dstBytes );
	#endif
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Standard C
#include <stdlib.h>

// vlib
#include "vlib/proc_info.hh"
#include "vlib/types/packed.hh"
#include "vlib/types/type.hh"


namespace varyx
{
namespace mac
{

using namespace vlib;

static inline
size_t minimum( size_t size )
{
	/*
		  1 - 127: n + 1
		128 - 254: n + 2
		255 - 381: n + 3
		...
	*/
	
	return 1 + size + (size - 1) / 127;
}

static inline
size_t adequate( size_t size )
{
	/*
		  1 -  63: n + 1
		 64 - 127: n + 2
		128 - 191: n + 3
		192 - 255: n + 4
		...
	*/
	
	return 1 + size + size / 64u;
}

static
Value v_PackBits( const Value& v )
{
	const plus::string& s = v.string();
	
	const char* src = s.data();
	size_t size     = s.size();
	
	if ( size == 0  ||  size > 32767 )
	{
		/*
			PackBits() is defined as taking a short argument.
			If overflow occurs, it doesn't advance the dst pointer in OS X.
		*/
		return Packed( plus::string::null );
	}
	
	enum
	{
		small_buffer_size = 2 * 128,
		max_small_input   = 2 * 127,
	};
	
	char small_buffer[ small_buffer_size ];
	
	char* buffer = size <= max_small_input ? small_buffer
	                                       : (char*) malloc( adequate( size ) );
	char* dst = buffer;
	
	PackBits( (Ptr*) &src, &dst, size );
	
	plus::string result( buffer, dst );
	
	if ( buffer != small_buffer )
	{
		free( buffer );
	}
	
	return Packed( result );
}

static
int sizeof_unpacked_bits( uint8_t const* src, unsigned n_src )
{
	int len = 0;
	
	uint8_t const* end = src + n_src;
	
	while ( src < end )
	{
		signed char c = *src++;
		
		if ( c >= 0 )
		{
			size_t n = c + 1;
			
			if ( n > end - src )
			{
				return -1;
			}
			
			len += n;
			src += n;
		}
		else if ( c == (signed char) 0x80 )
		{
			/*
				The behavior of QuickDraw's UnpackBits() in response to $80
				is undocumented and varies between versions of Mac OS.
				Reject it by default.
			*/
			return -1;
		}
		else
		{
			size_t n = 1 - c;
			
			if ( src == end )
			{
				return -1;
			}
			
			src++;
			
			len += n;
		}
		
		if ( len > 32767 )
		{
			/*
				UnpackBits() is defined as taking a short argument for the
				/destination/ length.
			*/
			return -1;
		}
	}
	
	return len;
}

static
Value v_UnpackBits( const Value& v )
{
	const plus::string& s = v.string();
	
	const char* src = s.data();
	size_t    n_src = s.size();
	
	int n_dst = sizeof_unpacked_bits( (const uint8_t*) src, n_src );
	
	if ( n_dst <= 0 )
	{
		return Packed( plus::string::null );
	}
	
	enum
	{
		small_buffer_size = 2 * 128,
		max_small_input   = 2 * 127,
	};
	
	char small_buffer[ small_buffer_size ];
	
	char* buffer = n_dst <= max_small_input ? small_buffer
	                                        : (char*) malloc( n_dst );
	char* dst = buffer;
	
	UnpackBits( (Ptr*) &src, &dst, n_dst );
	
	plus::string result( buffer, dst );
	
	if ( buffer != small_buffer )
	{
		free( buffer );
	}
	
	return Packed( result );
}

static const Type packed = packed_vtype;

const proc_info proc_PackBits   = { "PackBits",   &v_PackBits,   &packed };
const proc_info proc_UnpackBits = { "UnpackBits", &v_UnpackBits, &packed };

}
}
