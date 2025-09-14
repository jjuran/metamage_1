/*
	RsrcMap.cc
	----------
*/

#include "RsrcMap.hh"

// Mac OS
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// ams-common
#include "master_pointer.hh"

// ams-rsrc
#include "rsrc_fork.hh"


#pragma exceptions off


Handle TopMapHndl : 0x0A50;
short SysMap      : 0x0A58;
short ResErr      : 0x0A60;


static
rsrc_header* find_rsrc( const rsrc_map_header& map, ResType type, Handle h )
{
	const type_list& types = *(type_list*) ((Ptr) &map + map.offset_to_types);
	
	UInt16 n_types = types.count_1 + 1;
	
	const type_header* it = types.list;
	
	while ( n_types-- > 0 )
	{
		if ( it->type != type )
		{
			++it;
			continue;
		}
		
		UInt16 n_rsrcs_1 = it->count_1;
		UInt16 offset    = it->offset;
		
		rsrc_header* rsrc = (rsrc_header*) ((Ptr) &types + offset);
		
		do
		{
			if ( rsrc->handle == h )
			{
				return rsrc;
			}
		}
		while ( ++rsrc, n_rsrcs_1-- > 0 );
		
		++it;
	}
	
	return NULL;
}

rsrc_header* recover_rsrc_header( Handle resource )
{
	OSErr err = resNotFound;
	
	rsrc_header* header = NULL;
	
	// Technically we shouldn't deref before null test, but it works in MWC68K
	
	const master_pointer& mp = *(const master_pointer*) resource;
	
	if ( resource  &&  mp.flags & kHandleIsResourceMask )
	{
		RsrcMapHandle rsrc_map = (RsrcMapHandle) mp.base;
		
		if ( (header = find_rsrc( **rsrc_map, mp.type, resource )) )
		{
			err = noErr;
		}
	}
	
	ResErr = err;
	
	return header;
}

RsrcMapHandle& find_rsrc_map( short refnum )
{
	if ( refnum == 0 )
	{
		refnum = SysMap;
	}
	
	RsrcMapHandle* rsrc_map = (RsrcMapHandle*) &TopMapHndl;
	
	while ( *rsrc_map  &&  rsrc_map[0][0]->refnum != refnum )
	{
		rsrc_map = (RsrcMapHandle*) &rsrc_map[0][0]->next_map;
	}
	
	ResErr = noErr;
	
	if ( *rsrc_map == NULL )
	{
		ResErr = resFNotFound;
	}
	
	return *rsrc_map;
}

RsrcMapHandle home_rsrc_map( Handle resource : __A0 )
{
	const master_pointer& mp = *(const master_pointer*) resource;
	
	if ( mp.flags & kHandleIsResourceMask )
	{
		RsrcMapHandle map = (RsrcMapHandle) mp.base;
		
		return map;
	}
	
	return NULL;
}
