/*
	Changed.cc
	----------
*/

#include "Changed.hh"

// Mac OS
#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "master_pointer.hh"

// ams-rsrc
#include "RsrcMap.hh"
#include "rsrc_fork.hh"


#pragma exceptions off


enum
{
	resProtected = 8,
	resChanged   = 2,
};


short MemErr : 0x0220;
short CurMap : 0x0A5A;
short ResErr : 0x0A60;


static
void adjust_name_offsets( rsrc_map_header& map, long name_offset, int delta )
{
	/*
		We're adding one to all name offsets before comparing
		so that we don't have to explicitly check for 0xFFFF.
	*/
	
	name_offset += 1;
	
	const type_list& types = *(type_list*) ((Ptr) &map + map.offset_to_types);
	
	UInt16 n_types = types.count_1 + 1;
	
	const type_header* it = types.list;
	
	while ( n_types-- > 0 )
	{
		UInt16 n_rsrcs_1 = it->count_1;
		UInt16 offset    = it->offset;
		
		rsrc_header* rsrc = (rsrc_header*) ((Ptr) &types + offset);
		
		do
		{
			UInt16 name_offset_1 = rsrc->name_offset + 1;
			
			if ( name_offset_1 > name_offset )
			{
				rsrc->name_offset += delta;
			}
		}
		while ( ++rsrc, n_rsrcs_1-- > 0 );
		
		++it;
	}
}

static
rsrc_header*
set_resource_name( RsrcMapHandle rsrc_map, rsrc_header* rsrc, const Byte* name )
{
	long new_name_size = 1 + name[ 0 ];  // includes length byte
	
	if ( name[ 0 ] == 0 )
	{
		new_name_size = 0;
	}
	
	rsrc_map_header& map = **rsrc_map;
	
	UInt32 offset_to_names = map.offset_to_names;
	
	Handle rsrc_map_h = (Handle) rsrc_map;
	
	Size length_of_map = mac::glue::GetHandleSize_raw( rsrc_map_h );
	
	long munge_index = length_of_map;
	
	long old_name_size = 0;
	
	if ( rsrc->name_offset != 0xFFFF )
	{
		munge_index = offset_to_names + rsrc->name_offset;
		
		Byte* old_name = (Byte*) &map + munge_index;
		
		old_name_size = 1 + old_name[ 0 ];
	}
	
	Size rsrc_offset = (Ptr) rsrc - *rsrc_map_h;
	
	Munger( rsrc_map_h, munge_index, NULL, old_name_size, name, new_name_size );
	
	if ( (ResErr = MemErr) )
	{
		return NULL;
	}
	
	rsrc = (rsrc_header*) (*rsrc_map_h + rsrc_offset);
	
	long name_offset = munge_index - offset_to_names;
	
	if ( old_name_size == 0 )
	{
		rsrc->name_offset = name_offset;
	}
	else if ( new_name_size == 0 )
	{
		rsrc->name_offset = 0xFFFF;
	}
	
	if ( int delta = new_name_size - old_name_size )
	{
		adjust_name_offsets( **rsrc_map, name_offset, delta );
	}
	
	return rsrc;
}

static
void SetResInfo_handler( Handle       resource : __A0,
                         short        id       : __D0,
                         const Byte*  name     : __A1 )
{
	if ( rsrc_header* rsrc = recover_rsrc_header( resource ) )
	{
		if ( ! (rsrc->attrs & resProtected) )
		{
			if ( name != NULL )
			{
				RsrcMapHandle rsrc_map = home_rsrc_map( resource );
				
				rsrc = set_resource_name( rsrc_map, rsrc, name );
			}
			
			if ( rsrc )
			{
				rsrc->id = id;
			}
		}
	}
}

asm
pascal void SetResInfo_patch( Handle resource, short id, const Byte* name )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVEA.L  (A2)+,A1
	MOVE.W   (A2)+,D0
	MOVEA.L  (A2)+,A0
	
	JSR      SetResInfo_handler
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDA.W   #10,SP
	
	JMP      (A0)
}

static
void ChangedResource_handler( Handle resource : __A0 )
{
	if ( rsrc_header* rsrc = recover_rsrc_header( resource ) )
	{
		if ( rsrc->attrs & resProtected )
		{
			// noErr
		}
		else
		{
			ERROR = "ChangedResource is unimplemented";
			
			ResErr = paramErr;
		}
	}
}

asm
pascal void ChangedResource_patch( Handle resource )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVEA.L  (A2)+,A0
	
	JSR      ChangedResource_handler
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #4,SP
	
	JMP      (A0)
}

static
void AddResource_handler( Handle       data : __A0,
                          ResType      type : __D0,
                          short        id   : __D1,
                          const Byte*  name : __A1 )
{
	Size data_size = mac::glue::GetHandleSize_raw( data );
	
	if ( data_size < 0 )
	{
		ResErr = data_size;
		
		return;
	}
	
	Size name_size = (name  &&  name[ 0 ]) ? 1 + name[ 0 ] : 0;
	
	Size map_increase = sizeof (rsrc_header) + sizeof (type_header) + name_size;
	
	RsrcMapHandle rsrc_map = find_rsrc_map( CurMap );
	
	Size length_of_map = mac::glue::GetHandleSize_raw( (Handle) rsrc_map );
	
	SetHandleSize( (Handle) rsrc_map, length_of_map + map_increase );
	
	if ( (ResErr = MemErr) )
	{
		return;
	}
	
	/*
		Now that we've successfully expanded the handle, we can be sure
		that future expansions up to the new size (after reducing to the
		original size) will (a) also succeed, and (b) not move memory.
	*/
	
	SetHandleSize( (Handle) rsrc_map, length_of_map );
	
	rsrc_map_header& map = **rsrc_map;
	
	rsrc_header new_rsrc;
	
	new_rsrc.id               = id;
	new_rsrc.name_offset      = -1;
	new_rsrc.attrs            = resChanged;
	new_rsrc.offset_high_byte = -1;
	new_rsrc.offset_low_word  = -1;
	new_rsrc.handle           = data;
	
	if ( name_size )
	{
		new_rsrc.name_offset = length_of_map - map.offset_to_names;
		
		Munger( (Handle) rsrc_map, length_of_map, NULL, 0, name, name_size );
	}
	
	type_list& types = *(type_list*) ((Ptr) *rsrc_map + map.offset_to_types);
	
	SInt16 n_types = types.count_1 + 1;
	
	type_header* it = types.list;
	
	while ( n_types-- > 0  &&  it->type != type )
	{
		++it;
	}
	
	if ( n_types < 0 )
	{
		n_types = types.count_1 += 1;
		
		for ( int i = 0;  i < n_types;  ++i )
		{
			types.list[ i ].offset += sizeof (type_header);
		}
		
		type_header th;
		
		UInt16 type_offset = (Ptr) it - (Ptr) &map;
		
		map.offset_to_names += sizeof th;
		
		Munger( (Handle) rsrc_map, type_offset, NULL, 0, &th, sizeof th );
		
		it->type    = type;
		it->count_1 = -1;
		it->offset  = map.offset_to_names - map.offset_to_types;
	}
	else
	{
		/*
			n_types is the count of types following the one that
			matched; it's at least zero and at most types.count_1.
			
			Those are the types whose offsets must be adjusted.
		*/
		
		for ( int i = 1;  i <= n_types;  ++i )
		{
			it[ i ].offset += sizeof (rsrc_header);
		}
		
		map_increase -= sizeof (type_header);
	}
	
	it->count_1 += 1;
	
	Size offset = map.offset_to_types + it->offset;
	
	Munger( (Handle) rsrc_map, offset, NULL, 0, &new_rsrc, sizeof new_rsrc );
	
	map.offset_to_names += sizeof (rsrc_header);
	
	master_pointer& mp = *(master_pointer*) data;
	
	mp.flags |= kHandleIsResourceMask;
	mp.type  = type;
	mp.base  = rsrc_map;
	
	ResErr = noErr;
}

asm
pascal void AddResource_patch( Handle       data,
                               ResType      type,
                               short        id,
                               const Byte*  name )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVEA.L  (A2)+,A1
	MOVE.W   (A2)+,D1
	MOVE.L   (A2)+,D0
	MOVEA.L  (A2)+,A0
	
	JSR      AddResource_handler
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	LEA      14(SP),SP
	
	JMP      (A0)
}
