/*
	Resources.cc
	------------
*/

#include "Resources.hh"

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// Standard C
#include <string.h>

// Standard C++
#include <new>

// gear
#include "gear/hexadecimal.hh"

// freemount-client
#include "freemount/synced.hh"


#define STRLEN( s )  (sizeof "" s - 1)


short MemErr : 0x0220;
short ResErr : 0x0A60;

const short memFullErr  = -108;
const short resNotFound = -192;

unsigned long module_A4;

static inline
asm uint32_t swap_A4( uint32_t a4 : __D0 ) : __D0
{
	EXG      D0,A4
}

class temp_A4
{
	private:
		uint32_t saved_A4;
		
		// non-copyable
		temp_A4           ( const temp_A4& );
		temp_A4& operator=( const temp_A4& );
	
	public:
		temp_A4() : saved_A4( swap_A4( module_A4 ) )
		{
		}
		
		~temp_A4()
		{
			swap_A4( saved_A4 );
		}
};

pascal char** GetResource_patch( unsigned long type, short id )
{
	temp_A4 a4;
	
	char path[] = "System/r/1234.TYPE";
	
	char* p = path + STRLEN( "System/r/" );
	
	gear::encode_16_bit_hex( id, p );
	
	p += 5;
	
	*(unsigned long*) p = type;
	
	char** result = 0;  // NULL
	
	try
	{
		namespace F = freemount;
		
		const int in  = 6;
		const int out = 7;
		
		plus::string got = F::synced_get( in, out, path );
		
		const unsigned long size = got.size();
		
		result = NewHandle( size );
		
		if ( result == 0 )  // NULL
		{
			ResErr = MemErr;
			return result;
		}
		
		memcpy( *result, got.data(), size );
	}
	catch ( const std::bad_alloc& )
	{
		ResErr = memFullErr;
	}
	catch ( ... )
	{
		ResErr = resNotFound;
	}
	
	return result;
}

pascal void ReleaseResource_patch( char** resource )
{
	DisposeHandle( resource );
}
