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

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


struct path_component
{
	const path_component*  next;
	const Byte*            name;
	size_t                 size;
};

static inline
bool is_error( const FSSpec& file )
{
	return file.vRefNum == 0;
}

static inline
long error( const FSSpec& file )
{
	return file.parID;
}

static
plus::string GetMacPathname( const FSSpec& file, const path_component* link )
{
	// make_FSSpec() may return errors, so check here.
	
	if ( is_error( file ) )
	{
		Mac::ThrowOSStatus( error( file ) );
	}
	
	path_component node = { link, file.name, file.name[ 0 ] };
	
	if ( link )
	{
		node.size += link->size + 1;
	}
	
	if ( file.parID != fsRtParID )
	{
		mac::types::VRefNum_DirID dir = mac::file::parent_directory( file );
		
		return GetMacPathname( mac::file::make_FSSpec( dir ), &node );
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
	
	return result;
}

plus::string GetMacPathname( const mac::types::VRefNum_DirID& dir )
{
	return GetMacPathname( mac::file::make_FSSpec( dir ), NULL );
}

plus::string GetMacPathname( const FSSpec& file )
{
	bool needsTrailingColon = file.parID == fsRtParID;
	
	const path_component node = {};
	
	const path_component* link = needsTrailingColon ? &node : NULL;
	
	return GetMacPathname( file, link );
}
