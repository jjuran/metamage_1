/*
	HFS_pathname.cc
	---------------
*/

#include "mac_file/HFS_pathname.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// more-libc
#include "more/string.h"

// mac-file-utils
#include "mac_file/make_FSSpec.hh"
#include "mac_file/parent_directory.hh"


namespace mac  {
namespace file {

typedef struct path_component Node;

struct path_component
{
	const Node*  next;
	const Byte*  name;
	size_t       size;
};

#if ! __LP64__

static
long HFS_pathname( char* p, unsigned len, const FSSpec& file, const Node* link )
{
	using mac::types::VRefNum_DirID;
	
	path_component node = { link, file.name, file.name[ 0 ] };
	
	if ( link )
	{
		node.size += link->size + 1;
	}
	
	if ( file.parID != fsRtParID )
	{
		VRefNum_DirID dir = parent_directory( file );
		
		FSSpec dir_spec;
		
		if ( OSErr err = make_FSSpec( dir_spec, dir ) )
		{
			return err;
		}
		
		return HFS_pathname( p, len, dir_spec, &node );
	}
	
	if ( node.size > len )
	{
		/*
			The buffer isn't big enough to fit the entire path.
			Return the length of the path, but with the high bit set.
			For example, if the path length were 64 bytes, we'd return
			0x80000040.  This pattern doesn't conflict with OSStatus
			error space until the length exceeds 65536, which is
			pathologically long for a pathname and will only
			occur in the presence of a fuzzing attempt.
			
			This code will only be used in 32-bit environments due to
			lack of 64-bit FSSpec support, but nonetheless the next line
			is correct for 64-bit `long` (returning e.g. 0xFFFFFFFF80000040).
			
			Any negative result is an error.  To distinguish between a
			buffer overrun and File Manager error:
			
				long got = HFS_pathname( ... );
				
				if ( got < 0 )
				{
					// error
					
					if ( (SInt32) got <= (SInt32) 0x80008000 )
					{
						// buffer overrun
					}
					else
					{
						// OSErr
					}
				}
				else
				{
					// success, pathname occupies `got` bytes of buffer
				}
		*/
		
		return ~0x7FFFFFFFL | node.size;
	}
	
	link = &node;
	
	goto start;
	
	do
	{
		*p++ = ':';
		
	start:
		
		const Byte* q = link->name;
		
		p = (char*) mempcpy( p, q + 1, q[ 0 ] );
		
		link = link->next;
	}
	while ( link != NULL );
	
	return node.size;
}

long HFS_pathname( char* buffer, unsigned len, const FSSpec& file )
{
	path_component* link = NULL;
	
	path_component node;
	
	if ( file.parID == fsRtParID )
	{
		// This gets us a trailing colon for a volume pathname.
		
		node.next = NULL;
		node.name = "\p";
		node.size = 0;
		
		link = &node;
	}
	
	return HFS_pathname( buffer, len, file, link );
}

#else

void dummy()
{
}

#endif  // #if ! __LP64__

}
}
