/*
	iterate_resources.cc
	--------------------
*/

#include "MacVFS/util/iterate_resources.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-sys-utils
#include "mac_sys/res_error.hh"

// gear
#include "gear/hexadecimal.hh"

// MacScribe
#include "quad/safe.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"


namespace vfs
{

using MacScribe::is_safe_quad;

OSErr iterate_resources( vfs::dir_contents& cache )
{
	plus::string name;
	
	const short n_types = Count1Types();
	
	for ( short i = 1;  i <= n_types;  ++i )
	{
		ResType type;
		
		Get1IndType( &type, i );
		
		const bool safe = is_safe_quad( type );
		
		const size_t size = safe ? 4 + 1 + 4
		                         : 4 + 1 + 8;
		
		const short n_rsrcs = Count1Resources( type );
		
		for ( short j = 1;  j <= n_rsrcs;  ++j )
		{
			const Handle r = Get1IndResource( type, j );
			
			if ( ! r )
			{
				OSErr err = mac::sys::res_error();
				
				return err ? err : memFullErr;
			}
			
			short    id;
			ResType  type;
			Str255   res_name;
			
			GetResInfo( r, &id, &type, res_name );
			
			char* p = name.reset_nothrow( size );  // can't fail
			
			p = gear::encode_16_bit_hex( id, p );
			
			*p++ = '.';
			
			if ( safe )
			{
				const char* q = (const char*) &type;
				
				*p++ = *q++;
				*p++ = *q++;
				*p++ = *q++;
				*p++ = *q++;
			}
			else
			{
				gear::encode_32_bit_hex( type, p );
			}
			
			const vfs::dir_entry node( id, name );
			
			cache.push_back( node );
		}
	}
	
	return noErr;
}

}
