/*
	GetMacPathname.cc
	-----------------
*/

#include "GetMacPathname.hh"

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

// plus
#include "plus/mac_utf8.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


struct path_component
{
	const path_component*  next;
	const Byte*            name;
	size_t                 size;
};

static
plus::string GetMacPathname( const FSSpec&          file,
                             const path_component*  link,
                             bool                   utf8 )
{
	path_component node = { link, file.name, file.name[ 0 ] };
	
	if ( link )
	{
		node.size += link->size + 1;
	}
	
	if ( file.parID != fsRtParID )
	{
		mac::types::VRefNum_DirID dir = mac::file::parent_directory( file );
		
		FSSpec dir_spec;
		
		Mac::ThrowOSStatus( mac::file::make_FSSpec( dir_spec, dir ) );
		
		return GetMacPathname( dir_spec, &node, utf8 );
	}
	
	plus::string result;
	
	char* p = result.reset( node.size );
	
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
	
	if ( utf8 )
	{
		return plus::utf8_from_mac( result );
	}
	
	return result;
}

plus::string GetMacPathname( const FSSpec& file, bool utf8 )
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
	
	return GetMacPathname( file, link, utf8 );
}
