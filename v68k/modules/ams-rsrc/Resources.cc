/*
	Resources.cc
	------------
*/

#include "Resources.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif
#ifndef __STRINGCOMPARE__
#include <StringCompare.h>
#endif

// Standard C
#include <stddef.h>

// Standard C++
#include <new>

// gear
#include "gear/hexadecimal.hh"

// log-of-war
#include "logofwar/report.hh"

// plus
#include "plus/var_string.hh"

// freemount-client
#include "freemount/synced.hh"

// ams-common
#include "callouts.hh"
#include "FCB.hh"
#include "master_pointer.hh"
#include "module_A4.hh"

// ams-rsrc
#include "rsrc_fork.hh"


#define STRLEN( s )  (sizeof "" s - 1)
#define STR_LEN( s )  "" s, (sizeof s - 1)


short MemErr      : 0x0220;
Str31 CurApName   : 0x0910;
Handle TopMapHndl : 0x0A50;
short CurMap      : 0x0A5A;
short ResErr      : 0x0A60;


typedef rsrc_data_element  rsrc_data;
typedef rsrc_map_header**  RsrcMapHandle;


static inline
asm Handle PtrToHand( const void* p : __A0, long size : __D0 )
{
	DC.W     0xA9E3  // _PtrToHand
}

static inline
rsrc_header* recover_rsrc_header( Handle resource )
{
	const master_pointer& mp = *(const master_pointer*) resource;
	
	if ( mp.flags & kHandleIsResourceMask )
	{
		return (rsrc_header*) (*(char**) mp.base + mp.offset);
	}
	
	return NULL;
}

static
RsrcMapHandle find_rsrc_map( short refnum )
{
	RsrcMapHandle rsrc_map = (RsrcMapHandle) TopMapHndl;
	
	while ( rsrc_map  &&  rsrc_map[0]->refnum != refnum )
	{
		rsrc_map = (RsrcMapHandle) rsrc_map[0]->next_map;
	}
	
	return rsrc_map;
}

static
uint16_t count_rsrcs( const rsrc_map_header& map, ResType type )
{
	const type_list& types = *(type_list*) ((Ptr) &map + map.offset_to_types);
	
	uint16_t n_types_1 = types.count_1;
	
	const type_header* it = types.list;
	
	do
	{
		if ( it->type == type )
		{
			return it->count_1 + 1;
		}
	}
	while ( ++it, n_types_1-- > 0 );
	
	return 0;
}

static
rsrc_header* get_nth_rsrc( const rsrc_map_header& map, ResType type, short i )
{
	const type_list& types = *(type_list*) ((Ptr) &map + map.offset_to_types);
	
	uint16_t n_types_1 = types.count_1;
	
	const type_header* it = types.list;
	
	do
	{
		if ( it->type != type )  continue;
		
		if ( --i > it->count_1 )
		{
			return NULL;
		}
		
		rsrc_header* rsrc = (rsrc_header*) ((Ptr) &types + it->offset);
		
		while ( i-- > 0 )
		{
			++rsrc;
		}
		
		return rsrc;
	}
	while ( ++it, n_types_1-- > 0 );
	
	return NULL;
}

static
rsrc_header* find_rsrc( const rsrc_map_header& map, ResType type, short id )
{
	const type_list& types = *(type_list*) ((Ptr) &map + map.offset_to_types);
	
	uint16_t n_types_1 = types.count_1;
	
	const type_header* it = types.list;
	
	do
	{
		if ( it->type != type )  continue;
		
		uint16_t n_rsrcs_1 = it->count_1;
		uint16_t offset    = it->offset;
		
		rsrc_header* rsrc = (rsrc_header*) ((Ptr) &types + offset);
		
		do
		{
			if ( rsrc->id == id )
			{
				return rsrc;
			}
		}
		while ( ++rsrc, n_rsrcs_1-- > 0 );
	}
	while ( ++it, n_types_1-- > 0 );
	
	return NULL;
}

static inline
ConstStr255Param get_name( const rsrc_map_header& map, const rsrc_header& rsrc )
{
	if ( rsrc.name_offset == 0xFFFF )
	{
		return NULL;
	}
	
	ConstStr255Param names = (const unsigned char*) &map + map.offset_to_names;
	
	return names + rsrc.name_offset;
}

static
rsrc_header* find_rsrc( const rsrc_map_header&  map,
                        ResType                 type,
                        ConstStr255Param        name )
{
	const type_list& types = *(type_list*) ((Ptr) &map + map.offset_to_types);
	
	uint16_t n_types_1 = types.count_1;
	
	const type_header* it = types.list;
	
	do
	{
		if ( it->type != type )  continue;
		
		uint16_t n_rsrcs_1 = it->count_1;
		uint16_t offset    = it->offset;
		
		rsrc_header* rsrc = (rsrc_header*) ((Ptr) &types + offset);
		
		do
		{
			if ( ConstStr255Param found_name = get_name( map, *rsrc ) )
			{
				if ( EqualString( name, found_name, false, true ) )
				{
					return rsrc;
				}
			}
		}
		while ( ++rsrc, n_rsrcs_1-- > 0 );
	}
	while ( ++it, n_types_1-- > 0 );
	
	return NULL;
}

static
const rsrc_data* get_data( const rsrc_map_header& map, const rsrc_header& rsrc )
{
	FCB* fcb = get_FCB( map.refnum );
	
	Ptr p = fcb->fcbBfAdr;
	
	uint32_t offset = rsrc.offset_high_byte << 16
	                | rsrc.offset_low_word;
	
	offset += map.fork_header.offset_to_data;
	
	p += offset;
	
	return (const rsrc_data*) p;
}

static
short OpenResFile_handler( ConstStr255Param name : __A0, short vRefNum : __D0 )
{
	OSErr err;
	short refNum;
	
	err = OpenRF( name, vRefNum, &refNum );
	
	if ( err != noErr )
	{
		goto bail;
	}
	
	FCB* fcb = get_FCB( refNum );
	
	Ptr fork_buffer = fcb->fcbBfAdr;
	
	if ( fcb->fcbEOF < 256 )  // TODO
	{
		err = eofErr;
		goto close_and_bail;
	}
	
	rsrc_fork_header& header = *(rsrc_fork_header*) fork_buffer;
	
	Ptr start_of_map = fork_buffer + header.offset_to_map;
	
	Handle h = PtrToHand( start_of_map, header.length_of_map );
	
	if ( h == NULL )
	{
		err = MemErr;
		goto close_and_bail;
	}
	
	RsrcMapHandle rsrc_map = (RsrcMapHandle) h;
	
	rsrc_map_header& map = **rsrc_map;
	
	map.fork_header = header;
	map.next_map    = TopMapHndl;
	map.refnum      = refNum;
	
	type_list& types = *(type_list*) (*h + map.offset_to_types);
	
	uint16_t n_types_1 = types.count_1;
	
	type_header* type = types.list;
	
	do
	{
		uint16_t n_rsrcs_1 = type->count_1;
		uint16_t offset    = type->offset;
		
		rsrc_header* rsrc = (rsrc_header*) ((Ptr) &types + offset);
		
		do
		{
			rsrc->handle = NULL;
			
			++rsrc;
		}
		while ( n_rsrcs_1-- > 0 );
		
		++type;
	}
	while ( n_types_1-- > 0 );
	
	TopMapHndl = h;
	
	ResErr = noErr;
	
	return CurMap = refNum;
	
close_and_bail:
	FSClose( refNum );
	
bail:
	ResErr = err;
	return -1;
}

asm
pascal short OpenResFile_patch( ConstStr255Param name )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVEA.L  (A2)+,A0
	MOVEQ.L  #0,D0    // vRefNum = 0
	
	JSR      OpenResFile_handler
	MOVE.W   D0,(A2)
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #4,SP
	
	JMP      (A0)
}

pascal void RsrcZoneInit_patch()
{
	FCBSPtr->fcbs[ 0 ].fcbFlNum = -1;  // claim for System resource fork
	
	// CurApRefNum is automatically fixed at 2
	
	OpenResFile_handler( CurApName, 0 );
}

pascal short ResError_patch()
{
	return ResErr;
}

pascal short CurResFile_patch()
{
	return CurMap;
}

static
short HomeResFile_handler( Handle resource : __A0 )
{
	const master_pointer& mp = *(const master_pointer*) resource;
	
	if ( mp.flags & kHandleIsResourceMask )
	{
		ResErr = noErr;
		
		RsrcMapHandle map = (RsrcMapHandle) mp.base;
		
		return map[0]->refnum;
	}
	
	ResErr = resNotFound;
	
	return -1;
}

asm
pascal short HomeResFile_patch( Handle resource )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVEA.L  (A2)+,A0
	
	JSR      HomeResFile_handler
	MOVE.W   D0,(A2)
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #4,SP
	
	JMP      (A0)
}

pascal void UseResFile_patch( short refnum )
{
	CurMap = refnum;
}

pascal void SetResLoad_patch( unsigned char load )
{
}

static
bool try_to_get( const char* begin, const char* end, plus::var_string& data )
{
	plus::string path( begin, end - begin, plus::delete_never );
	
	try
	{
		namespace F = freemount;
		
		const int in  = 6;
		const int out = 7;
		
		data = F::synced_get( in, out, path ).move();
		
		return true;
	}
	catch ( const std::bad_alloc& )
	{
		ResErr = memFullErr;
	}
	catch ( ... )
	{
		ResErr = resNotFound;
	}
	
	return false;
}

static
bool try_to_get( char*              insert_end,
                 char*              end,
                 const char*        name,
                 size_t             len,
                 plus::var_string&  result )
{
	char* begin = insert_end - len;
	
	fast_memcpy( begin, name, len );
	
	return try_to_get( begin, end, result );
}

static
Handle new_res_handle( RsrcMapHandle rsrc_map, rsrc_header& rsrc, ResType type )
{
	if ( rsrc.handle == NULL )
	{
		const rsrc_data* data = get_data( **rsrc_map, rsrc );
		
		// data may be misaligned
		
		Size data_length;
		fast_memcpy( &data_length, &data->length, sizeof (Size) );
		
		if ( Handle h = PtrToHand( data->bytes, data_length ) )
		{
			master_pointer& mp = *(master_pointer*) h;
			
			mp.flags  = kHandleIsResourceMask;
			mp.offset = (char*) &rsrc - (char*) *rsrc_map;
			mp.type   = type;
			mp.base   = rsrc_map;
			
			rsrc.handle = h;
		}
		else
		{
			ResErr = memFullErr;
			return NULL;
		}
	}
	
	ResErr = noErr;
	
	return rsrc.handle;
}

static
Handle GetResource_core( ResType type : __D0, short id : __D1 )
{
	RsrcMapHandle rsrc_map = find_rsrc_map( CurMap );
	
	while ( rsrc_map != NULL )
	{
		if ( rsrc_header* rsrc = find_rsrc( **rsrc_map, type, id ) )
		{
			return new_res_handle( rsrc_map, *rsrc, type );
		}
		
		rsrc_map = (RsrcMapHandle) rsrc_map[0]->next_map;
	}
	
	/*
		The app name can only be 31 bytes (not 32), but with the extra byte,
		"TYPE" is word-aligned.
	*/
	
	char tmp_path[] = "1234567890" "1234567890" "1234567890" "12/r/1234.TYPE";
	
	char* const end      = tmp_path + sizeof tmp_path - 1;
	char* const name_end = tmp_path + 32;
	
	char* p = end - STRLEN( "1234.TYPE" );
	
	gear::encode_16_bit_hex( id, name_end + STRLEN( "/r/" ) );
	
	p += 5;
	
	*(ResType*) p = type;
	
	plus::var_string rsrc;
	
	Handle result = 0;  // NULL
	
	bool got = false;
	
	if ( CurApName[ 0 ] != '\0' )
	{
		const size_t len = CurApName[ 0 ];
		
		got = try_to_get( name_end, end, (char*) CurApName + 1, len, rsrc );
	}
	
	if ( ! got )
	{
		got = try_to_get( name_end, end, STR_LEN( "System" ), rsrc );
	}
	
	if ( ! got )
	{
		got = try_to_get( name_end, end, STR_LEN( "AMS Resources" ), rsrc );
	}
	
	if ( ! got )
	{
		// ResErr is already set.
		return result;
	}
	
	result = PtrToHand( rsrc.data(), rsrc.size() );
	
	ResErr = MemErr;
	
	return result;
}

static
short CountResources_handler( ResType type : __D0 )
{
	uint16_t count = 0;
	
	RsrcMapHandle rsrc_map = find_rsrc_map( CurMap );
	
	while ( rsrc_map != NULL )
	{
		count += count_rsrcs( **rsrc_map, type );
		
		rsrc_map = (RsrcMapHandle) rsrc_map[0]->next_map;
	}
	
	return count;
}

asm
pascal short CountResources_patch( ResType type )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVE.L   (A2)+,D0
	
	JSR      CountResources_handler
	MOVE.W   D0,(A2)
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #4,SP
	
	JMP      (A0)
}

static
Handle GetIndResource_handler( unsigned long type : __D0, short index : __D1 )
{
	if ( index <= 0 )
	{
		ResErr = noErr;
		
		return 0;  // NULL
	}
	
	RsrcMapHandle rsrc_map = find_rsrc_map( CurMap );
	
	while ( rsrc_map != NULL )
	{
		uint16_t count = count_rsrcs( **rsrc_map, type );
		
		if ( index <= count )
		{
			if ( rsrc_header* rsrc = get_nth_rsrc( **rsrc_map, type, index ) )
			{
				return new_res_handle( rsrc_map, *rsrc, type );
			}
			
			// Shouldn't get here?
		}
		
		index -= count;
		
		rsrc_map = (RsrcMapHandle) rsrc_map[0]->next_map;
	}
	
	ResErr = resNotFound;
	
	return 0;  // NULL
}

asm
pascal Handle GetIndResource_patch( ResType type, short index )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVE.W   (A2)+,D1
	MOVE.L   (A2)+,D0
	
	JSR      GetIndResource_handler
	MOVE.L   A0,(A2)
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #6,SP
	
	JMP      (A0)
}

static
Handle GetResource_handler( ResType type : __D0, short id : __D1 )
{
	temp_A4 a4;
	
	return GetResource_core( type, id );
}

asm
pascal Handle GetResource_patch( ResType type, short id )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVE.W   (A2)+,D1
	MOVE.L   (A2)+,D0
	
	JSR      GetResource_handler
	MOVE.L   A0,(A2)
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #6,SP
	
	JMP      (A0)
}

static
Handle GetNamedResource_handler( ResType type : __D0, const UInt8* name : __A0 )
{
	RsrcMapHandle rsrc_map = find_rsrc_map( CurMap );
	
	while ( rsrc_map != NULL )
	{
		if ( rsrc_header* rsrc = find_rsrc( **rsrc_map, type, name ) )
		{
			return new_res_handle( rsrc_map, *rsrc, type );
		}
		
		rsrc_map = (RsrcMapHandle) rsrc_map[0]->next_map;
	}
	
	ResErr = resNotFound;
	
	return 0;  // NULL
}

asm
pascal Handle GetNamedResource_patch( ResType type, ConstStr255Param name )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVEA.L  (A2)+,A0
	MOVE.L   (A2)+,D0
	
	JSR      GetNamedResource_handler
	MOVE.L   A0,(A2)
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #8,SP
	
	JMP      (A0)
}

pascal void LoadResource_patch( Handle resource )
{
}

pascal void ReleaseResource_patch( Handle resource )
{
	DetachResource_patch( resource );
	
	/*
		TODO:  Check ResErr.
		
		For now we call DisposeHandle() unconditionally, so that we
		don't leak handles owned by non-map resources.
	*/
	
	DisposeHandle( resource );
}

static
void DetachResource_handler( Handle resource : __A0 )
{
	OSErr err = resNotFound;
	
	if ( rsrc_header* rsrc = recover_rsrc_header( resource ) )
	{
		rsrc->handle = NULL;
		
		master_pointer& mp = *(master_pointer*) resource;
		
		mp.flags -= kHandleIsResourceMask;
		
		err = noErr;
	}
	
	ResErr = err;
}

asm
pascal void DetachResource_patch( Handle resource )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVEA.L  (A2)+,A0
	
	JSR      DetachResource_handler
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #4,SP
	
	JMP      (A0)
}

struct GetResInfo_args
{
	unsigned char*  name;
	unsigned long*  type;
	short*          id;
	Handle          resource;
};

static
void GetResInfo_handler( const GetResInfo_args* args : __A0 )
{
	Handle resource = args->resource;
	
	const master_pointer& mp = *(const master_pointer*) resource;
	
	const rsrc_header* rsrc = recover_rsrc_header( resource );
	
	if ( rsrc == NULL )
	{
		ResErr = resNotFound;
		return;
	}
	
	if ( args->id )
	{
		*args->id = rsrc->id;
	}
	
	if ( args->type )
	{
		*args->type = mp.type;
	}
	
	if ( args->name )
	{
		args->name[ 0 ] = '\0';  // Pessimistically assign empty string
		
		const rsrc_map_header& rsrc_map = **(RsrcMapHandle) mp.base;
		
		if ( ConstStr255Param name = get_name( rsrc_map, *rsrc ) )
		{
			fast_memcpy( args->name, name, 1 + name[ 0 ] );
		}
	}
}

asm
pascal void GetResInfo_patch( Handle          resource,
                              short*          id,
                              unsigned long*  type,
                              unsigned char*  name )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A0
	
	JSR      GetResInfo_handler
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDA.L   #16,SP
	
	JMP      (A0)
}

static
short GetResAttrs_handler( Handle resource : __A0 )
{
	if ( const rsrc_header* rsrc = recover_rsrc_header( resource ) )
	{
		ResErr = noErr;
		
		return rsrc->attrs;
	}
	
	ResErr = resNotFound;
	
	return 0;
}

asm
pascal short GetResAttrs_patch( Handle resource )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVEA.L  (A2)+,A0
	
	JSR      GetResAttrs_handler
	MOVE.W   D0,(A2)
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #4,SP
	
	JMP      (A0)
}

pascal long SizeRsrc_patch( Handle resource )
{
	return GetHandleSize( resource );
}

pascal void ChangedResource_patch( Handle resource )
{
	ERROR = "ChangedResource is unimplemented";
}

pascal void SetResPurge_patch( unsigned char install )
{
}

static
short Count1Resources_handler( ResType type : __D0 )
{
	RsrcMapHandle rsrc_map = find_rsrc_map( CurMap );
	
	return count_rsrcs( **rsrc_map, type );
}

asm
pascal short Count1Resources_patch( ResType type )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVE.L   (A2)+,D0
	
	JSR      Count1Resources_handler
	MOVE.W   D0,(A2)
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #4,SP
	
	JMP      (A0)
}

static
Handle Get1IxResource_handler( unsigned long type : __D0, short index : __D1 )
{
	if ( index <= 0 )
	{
		ResErr = noErr;
		
		return 0;  // NULL
	}
	
	RsrcMapHandle rsrc_map = find_rsrc_map( CurMap );
	
	if ( rsrc_header* rsrc = get_nth_rsrc( **rsrc_map, type, index ) )
	{
		return new_res_handle( rsrc_map, *rsrc, type );
	}
	
	ResErr = resNotFound;
	
	return 0;  // NULL
}

asm
pascal Handle Get1IxResource_patch( ResType type, short index )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVE.W   (A2)+,D1
	MOVE.L   (A2)+,D0
	
	JSR      Get1IxResource_handler
	MOVE.L   A0,(A2)
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #6,SP
	
	JMP      (A0)
}

static
Handle Get1Resource_handler( ResType type : __D0, short id : __D1 )
{
	RsrcMapHandle rsrc_map = find_rsrc_map( CurMap );
	
	if ( rsrc_header* rsrc = find_rsrc( **rsrc_map, type, id ) )
	{
		return new_res_handle( rsrc_map, *rsrc, type );
	}
	
	ResErr = resNotFound;
	
	return NULL;
}

asm
pascal Handle Get1Resource_patch( ResType type, short id )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVE.W   (A2)+,D1
	MOVE.L   (A2)+,D0
	
	JSR      Get1Resource_handler
	MOVE.L   A0,(A2)
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #6,SP
	
	JMP      (A0)
}

static
Handle Get1NamedResource_handler( ResType          type : __D0,
                                  ConstStr255Param name : __A0 )
{
	RsrcMapHandle rsrc_map = find_rsrc_map( CurMap );
	
	if ( rsrc_header* rsrc = find_rsrc( **rsrc_map, type, name ) )
	{
		return new_res_handle( rsrc_map, *rsrc, type );
	}
	
	ResErr = resNotFound;
	
	return 0;  // NULL
}

asm
pascal Handle Get1NamedResource_patch( ResType type, ConstStr255Param name )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVEA.L  (A2)+,A0
	MOVE.L   (A2)+,D0
	
	JSR      Get1NamedResource_handler
	MOVE.L   A0,(A2)
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #8,SP
	
	JMP      (A0)
}

asm
pascal short OpenRFPerm_patch( ConstStr255Param name, short vRefNum, char perm )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVE.B   (A2)+,D1  // payload in the high byte
	ADDQ.L   #1,A2     // skip the low byte
	MOVE.W   (A2)+,D0
	MOVEA.L  (A2)+,A0
	
	JSR      OpenResFile_handler
	MOVE.W   D0,(A2)
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #8,SP
	
	JMP      (A0)
}
