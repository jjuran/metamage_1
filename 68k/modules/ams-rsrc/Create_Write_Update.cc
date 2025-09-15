/*
	Create_Write_Update.cc
	----------------------
*/

#include "Create_Write_Update.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif

// Standard C++
#include <algorithm>

// mac-glue-utils
#include "mac_glue/Memory.hh"

// ams-common
#include "callouts.hh"

// ams-rsrc
#include "RsrcMap.hh"
#include "rsrc_fork.hh"


#pragma exceptions off


enum
{
	resProtected = 8,
	resChanged   = 2,
};

enum
{
	mapReadOnly = 128,
	mapCompact  =  64,
	mapChanged  =  32,
};


short MemErr : 0x0220;
short ResErr : 0x0A60;


struct empty_resource_map : rsrc_map_header
{
	UInt16 empty_type_list;
};

static
void CreateResFile_handler( const Byte* name : __A0, short vRefNum : __D0 )
{
	OSErr err;
	short refNum;
	
	err = Create( name, vRefNum, 'RSED', 'rsrc' );
	
	if ( err != noErr  &&  err != dupFNErr )
	{
		goto bail;
	}
	
	err = OpenRF( name, vRefNum, &refNum );
	
	if ( err != noErr )
	{
		goto bail;
	}
	
	SInt32 rsrc_fork_size;
	
	err = GetEOF( refNum, &rsrc_fork_size );
	
	if ( err == noErr  &&  rsrc_fork_size > 0 )
	{
		err = dupFNErr;
	}
	
	if ( err != noErr )
	{
		goto close_and_bail;
	}
	
	enum
	{
		offset_to_data = sizeof (rsrc_fork_superheader),
		length_of_data = 0,
		
		offset_to_map = offset_to_data + length_of_data,
		length_of_map = sizeof (empty_resource_map),
		
		offset_to_types = sizeof (rsrc_map_header),
		offset_to_names = sizeof (empty_resource_map),
		empty_type_list = -1,
	};
	
	empty_resource_map empty_map;
	
	fast_memset( &empty_map, '\0', sizeof empty_map );
	
	rsrc_fork_header& fork_header = empty_map.fork_header;
	
	fork_header.offset_to_data = offset_to_data;  // 256
	fork_header.offset_to_map  = offset_to_map;   // 256
//	fork_header.length_of_data = length_of_data;  //   0
	fork_header.length_of_map  = length_of_map;   //  30
	
//	empty_map.next_map = NULL;
//	empty_map.refnum   = 0;
//	empty_map.attrs    = 0;
	empty_map.offset_to_types = offset_to_types;  // 28 bytes
	empty_map.offset_to_names = offset_to_names;  // 30 bytes
	empty_map.empty_type_list = empty_type_list;  //  0 types
	
	Size fork_header_size = sizeof (rsrc_fork_header);
	Size size_of_rsrc_map = length_of_map;
	
	(err = SetEOF ( refNum, 286 ))                                    ||
	(err = FSWrite( refNum, &fork_header_size, (Ptr) &fork_header ))  ||
	(err = SetFPos( refNum, fsFromStart, 256 ))                       ||
	(err = FSWrite( refNum, &size_of_rsrc_map, (Ptr) &empty_map   ));
	
	if ( err != noErr )
	{
		goto close_and_bail;
	}
	
close_and_bail:
	FSClose( refNum );
	
bail:
	ResErr = err;
}

asm
pascal void CreateResFile_patch( const Byte* name )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVEA.L  (A2)+,A0
	MOVEQ.L  #0,D0    // vRefNum = 0
	
	JSR      CreateResFile_handler
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #4,SP
	
	JMP      (A0)
}

static
OSErr write_resource_data( rsrc_header* rsrc )
{
	if ( (rsrc->attrs & (resProtected | resChanged)) != resChanged )
	{
		return noErr;
	}
	
	rsrc_map_header& map = **home_rsrc_map( rsrc->handle );
	
	if ( map.attrs & mapReadOnly )
	{
		return noErr;
	}
	
	OSErr err;
	
	long& attrs_offset = *(long*) &rsrc->attrs;
	
	Size data_offset = attrs_offset & 0x00FFFFFF;
	
	Size new_data_size = mac::glue::GetHandleSize( rsrc->handle );
	Size old_data_size = -1;
	
	Size data_size_size = sizeof old_data_size;
	
	short refnum = map.refnum;
	
	if ( (SInt8) rsrc->offset_high_byte >= 0 )
	{
		(err = SetFPos( refnum, fsFromStart, 256 + data_offset  ))  ||
		(err = FSRead ( refnum, &data_size_size, &old_data_size ));
		
		if ( err )
		{
			return err;
		}
	}
	
	/*
		Write the resource data.
	*/
	
	(err = SetFPos( refnum, fsFromStart, 256 + data_offset  ))  ||
	(err = FSWrite( refnum, &data_size_size, &new_data_size ))  ||
	(err = FSWrite( refnum, &new_data_size, *rsrc->handle   ));
	
	if ( err == noErr )
	{
		rsrc->attrs &= ~resChanged;
	}
	
	return err;
}

static
OSErr write_all_resources( rsrc_map_header& map )
{
	OSErr err = noErr;
	
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
			if ( rsrc->attrs & resChanged )
			{
				err = write_resource_data( rsrc );
				
				if ( err < 0 )
				{
					return err;
				}
			}
		}
		while ( ++rsrc, n_rsrcs_1-- > 0 );
		
		++it;
	}
	
	return err;
}

static
long enumerate_resources( rsrc_map_header& map, Handle list )
{
	UInt32 length = 0;
	
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
			/*
				Append the resource entry's address (not its contents).
			*/
			
			if ( (SInt8) rsrc->offset_high_byte >= 0 )
			{
				Munger( list, length, NULL, 0, &rsrc, sizeof rsrc );
				
				if ( OSErr err = MemErr )
				{
					return err;
				}
				
				length += sizeof rsrc;
			}
		}
		while ( ++rsrc, n_rsrcs_1-- > 0 );
		
		++it;
	}
	
	return length;
}

struct rsrc_header_less
{
	bool operator()( const rsrc_header* a, const rsrc_header* b )
	{
		long a_offset = *(long*) &a->attrs & 0x00FFFFFF;
		long b_offset = *(long*) &b->attrs & 0x00FFFFFF;
		
		return a_offset < b_offset;
	}
};

static
OSErr compact_resources( RsrcMapHandle rsrc_map )
{
	OSErr  err;
	SInt32 file_size;
	
	rsrc_map_header& map = **rsrc_map;
	
	short refnum = map.refnum;
	
	(err = GetEOF ( refnum, &file_size ))  ||
	(err = SetFPos( refnum, fsFromStart, 256 ));
	
	if ( err )
	{
		return err;
	}
	
	/*
		length_of_data below includes the resource map,
		because there might be newly added resource data
		*following* the resource map in the resource fork.
	*/
	
	Size length_of_map  = mac::glue::GetHandleSize_raw( (Handle) rsrc_map );
	Size length_of_data = file_size - 256;
	
	Handle data = NewHandle( length_of_data );
	
	if ( ! data )
	{
		return MemErr;
	}
	
	Handle list = NewHandle( 0 );
	
	if ( ! list )
	{
		err = MemErr;
		
		goto bail_but_this_should_never_happen;
	}
	
	long len = enumerate_resources( map, list );
	
	if ( len < 0 )
	{
		goto bail;
	}
	
	(err = FSRead( refnum, &length_of_data, *data ))  ||
	(err = SetFPos( refnum, fsFromStart, 256 ));
	
	if ( err )
	{
		goto bail;
	}
	
	typedef rsrc_header** Iter;
	
	Iter begin = (Iter)  *list;
	Iter end   = (Iter) (*list + len);
	
	std::sort( begin, end, rsrc_header_less() );
	
	Size data_size_size = sizeof (Size);
	
	long new_data_offset = 0;
	
	Iter it = begin;
	
	while ( it < end )
	{
		rsrc_header* rsrc = *it++;
		
		long& attrs_offset = *(long*) &rsrc->attrs;
		
		Size old_data_offset = attrs_offset & 0x00FFFFFF;
		
		Size data_size = 0;
		
		fast_memcpy( &data_size, *data + old_data_offset, sizeof (Size) );
		
		data_size += sizeof (Size);
		
		fast_memmove( *data + new_data_offset,
		              *data + old_data_offset,
		              data_size );
		
		attrs_offset = (attrs_offset & 0xFF000000) | new_data_offset;
		
		new_data_offset += data_size;
	}
	
	DisposeHandle( list );
	
	length_of_data = new_data_offset;
	
	err = FSWrite( refnum, &length_of_data, *data );
	
	DisposeHandle( data );
	
	rsrc_fork_header& fork = map.fork_header;
	
	fork.offset_to_map  = 256 + length_of_data;
	fork.length_of_data = length_of_data;
	fork.length_of_map  = length_of_map;
	
	map.attrs &= ~mapCompact;
	
	return err;
	
bail:
	
	DisposeHandle( list );
	
bail_but_this_should_never_happen:
	
	DisposeHandle( data );
	
	return err;
}

static inline
OSErr write_resource_map( short refnum, const rsrc_map_header& map )
{
	OSErr err;
	
	const rsrc_fork_header& fork = map.fork_header;
	
	SInt32 fork_header_size = sizeof fork;
	
	SInt32 offset_to_map = fork.offset_to_map;
	SInt32 length_of_map = fork.length_of_map;
	
	(err = SetFPos( refnum, fsFromStart, offset_to_map ))  ||
	(err = FSWrite( refnum, &length_of_map,    &map    ))  ||
	(err = SetFPos( refnum, fsFromStart, 0             ))  ||
	(err = FSWrite( refnum, &fork_header_size, &fork   ));
	
	SetEOF( refnum, offset_to_map + length_of_map );
	
	return err;
}

static
void UpdateResFile_handler( short refnum : __D0 )
{
	if ( RsrcMapHandle rsrc_map = find_rsrc_map( refnum ) )
	{
		rsrc_map_header& map = **rsrc_map;
		
		if ( (map.attrs & (mapChanged | mapReadOnly)) == mapChanged )
		{
			OSErr err;
			
			err = write_all_resources( map );
			
			if ( err == noErr  &&  map.attrs & mapCompact )
			{
				err = compact_resources( rsrc_map );
			}
			
			if ( err == noErr )
			{
				map.attrs &= ~mapChanged;
				
				err = write_resource_map( refnum, map );
			}
			
			ResErr = err;
		}
	}
}

asm
pascal void UpdateResFile_patch( short refnum )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVE.W   (A2)+,D0
	
	JSR      UpdateResFile_handler
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #2,SP
	
	JMP      (A0)
}

static
void WriteResource_handler( Handle resource : __A0 )
{
	if ( rsrc_header* rsrc = recover_rsrc_header( resource ) )
	{
		ResErr = write_resource_data( rsrc );
	}
}

asm
pascal void WriteResource_patch( Handle resource )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVEA.L  (A2)+,A0
	
	JSR      WriteResource_handler
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #4,SP
	
	JMP      (A0)
}
