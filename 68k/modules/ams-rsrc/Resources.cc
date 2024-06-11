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

// mac-glue-utils
#include "mac_glue/Memory.hh"

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"
#include "FCB.hh"
#include "master_pointer.hh"

// ams-rsrc
#include "rsrc_fork.hh"


#define STRLEN( s )  (sizeof "" s - 1)
#define STR_LEN( s )  "" s, (sizeof s - 1)


enum
{
	resPurgeable = 32,
	resLocked    = 16,
	resProtected = 8,
	resChanged   = 2,
};


Ptr   MemTop      : 0x0108;
short MemErr      : 0x0220;
Ptr   ApplZone    : 0x02AA;
short CurApRefNum : 0x0900;
Str31 CurApName   : 0x0910;
Handle TopMapHndl : 0x0A50;
short SysMap      : 0x0A58;
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
FCB* get_FCB( unsigned short refNum )
{
	if ( refNum > 0 )
	{
		return (FCB*) ((char*) FCBSPtr + refNum);
	}
	
	return NULL;
}

static inline
rsrc_header* recover_rsrc_header( Handle resource )
{
	// Technically we shouldn't deref before null test, but it works in MWC68K
	
	const master_pointer& mp = *(const master_pointer*) resource;
	
	if ( resource  &&  mp.flags & kHandleIsResourceMask )
	{
		return (rsrc_header*) (*(char**) mp.base + mp.offset);
	}
	
	return NULL;
}

static
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
	
	return *rsrc_map;
}

static
UInt16 count_rsrcs( const rsrc_map_header& map, ResType type )
{
	const type_list& types = *(type_list*) ((Ptr) &map + map.offset_to_types);
	
	UInt16 n_types = types.count_1 + 1;
	
	const type_header* it = types.list;
	
	while ( n_types-- > 0 )
	{
		if ( it->type == type )
		{
			return it->count_1 + 1;
		}
		
		++it;
	}
	
	return 0;
}

static
rsrc_header* get_nth_rsrc( const rsrc_map_header& map, ResType type, short i )
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
	
	return NULL;
}

static
rsrc_header* find_rsrc( const rsrc_map_header& map, ResType type, short id )
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
			if ( rsrc->id == id )
			{
				return rsrc;
			}
		}
		while ( ++rsrc, n_rsrcs_1-- > 0 );
		
		++it;
	}
	
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
			if ( ConstStr255Param found_name = get_name( map, *rsrc ) )
			{
				if ( EqualString_sans_case( name, found_name ) )
				{
					return rsrc;
				}
			}
		}
		while ( ++rsrc, n_rsrcs_1-- > 0 );
		
		++it;
	}
	
	return NULL;
}

static
const rsrc_data* get_data( const rsrc_map_header& map, const rsrc_header& rsrc )
{
	FCB* fcb = get_FCB( map.refnum );
	
	Ptr p = fcb->fcbBfAdr;
	
	UInt32 offset = rsrc.offset_high_byte << 16
	              | rsrc.offset_low_word;
	
	offset += map.fork_header.offset_to_data;
	
	p += offset;
	
	return (const rsrc_data*) p;
}

static
void CreateResFile_handler( ConstStr255Param name : __A0, short vRefNum : __D0 )
{
	OSErr err;
	
	ERROR = "CreateResFile is unimplemented";
	
	err = wrgVolTypErr;
	
	ResErr = err;
}

asm
pascal void CreateResFile_patch( const unsigned char* name )
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
	
	UInt16 n_types = types.count_1 + 1;
	
	type_header* type = types.list;
	
	while ( n_types-- > 0 )
	{
		UInt16 n_rsrcs_1 = type->count_1;
		UInt16 offset    = type->offset;
		
		rsrc_header* rsrc = (rsrc_header*) ((Ptr) &types + offset);
		
		do
		{
			rsrc->handle = NULL;
			
			++rsrc;
		}
		while ( n_rsrcs_1-- > 0 );
		
		++type;
	}
	
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

static
void CloseResFile_core( short refnum : __D0 )
{
	UpdateResFile_patch( refnum );
	
	RsrcMapHandle& rsrc_map = find_rsrc_map( refnum );
	
	if ( rsrc_map == NULL )
	{
		ResErr = resFNotFound;
		return;
	}
	
	rsrc_map_header& map = **rsrc_map;
	
	type_list& types = *(type_list*) ((Ptr) &map + map.offset_to_types);
	
	UInt16 n_types = types.count_1 + 1;
	
	type_header* type = types.list;
	
	while ( n_types-- > 0 )
	{
		UInt16 n_rsrcs_1 = type->count_1;
		UInt16 offset    = type->offset;
		
		rsrc_header* rsrc = (rsrc_header*) ((Ptr) &types + offset);
		
		do
		{
			if ( Handle h = rsrc->handle )
			{
				master_pointer& mp = *(master_pointer*) h;
				
				mp.flags -= kHandleIsResourceMask;
				
				DisposeHandle( h );
			}
			
			++rsrc;
		}
		while ( n_rsrcs_1-- > 0 );
		
		++type;
	}
	
	Handle h = (Handle) rsrc_map;
	
	rsrc_map = (RsrcMapHandle) map.next_map;
	
	if ( CurMap == refnum )
	{
		CurMap = rsrc_map[0]->refnum;
	}
	
	DisposeHandle( h );
	
	FSClose( refnum );
}

static inline
short map_refnum( Handle h )
{
	rsrc_map_header& map = **(RsrcMapHandle) h;
	
	return map.refnum;
}

static
void CloseResFile_handler( short refnum : __D0 )
{
	if ( refnum == CurApRefNum )
	{
		/*
			If we're asked to close the application's resource file,
			then close all resource files above it first.
		*/
		
		while ( (refnum = map_refnum( TopMapHndl )) != CurApRefNum )
		{
			CloseResFile_core( refnum );
		}
	}
	
	CloseResFile_core( refnum );
}

asm
pascal void CloseResFile_patch( short refnum )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVE.W   (A2)+,D0
	
	JSR      CloseResFile_handler
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #2,SP
	
	JMP      (A0)
}

static asm
void run_INIT( ProcPtr proc : __A0 )
{
	LINK     A6,#0
	MOVEM.L  D3-D7/A2-A4,-(SP)
	
	JSR      (A0)
	
	MOVEM.L  (SP)+,D3-D7/A2-A4
	UNLK     A6
	RTS
}

pascal short InitResources_patch()
{
	SysMap = OpenResFile_handler( "\p" "AMS Resources", 0 );
	
	short index = 0;
	
	while ( Handle h = GetIndResource_patch( 'INIT', ++index ) )
	{
		ProcPtr proc = (ProcPtr) *h;
		
		run_INIT( proc );
	}
	
	return SysMap;
}

static inline
bool handle_in_ApplZone( Handle h )
{
	/*
		Compare addresses against MemTop to distinguish those in the ApplZone
		heap from those in persistent private memory above ROM.
		
		Don't use HeapEnd, because the heap has already been shrunk to its
		original size by the time we're called to clear out resources from
		the system resource file that could have been allocated higher in the
		application heap.
		
		Don't use ApplLimit, just in case an application sets it to a value
		less than HeapEnd (which is harmless, or at least mostly harmless).
	*/
	
	return h  &&  *h > ApplZone  &&  *h < MemTop;
	
	/*
		We're actually checking the data block address above.  We can't use
		the simpler logic below to check the master pointer address, because
		master pointer blocks are always allocated out of private memory.
	*/
	
	return (Ptr) h > ApplZone  &&  (Ptr) h < MemTop;
}

static
void release_ApplZone_resources()
{
	if ( MemTop == NULL )
	{
		return;
	}
	
	rsrc_map_header& map = **(RsrcMapHandle) TopMapHndl;
	
	type_list& types = *(type_list*) ((Ptr) &map + map.offset_to_types);
	
	UInt16 n_types = types.count_1 + 1;
	
	type_header* type = types.list;
	
	while ( n_types-- > 0 )
	{
		UInt16 n_rsrcs_1 = type->count_1;
		UInt16 offset    = type->offset;
		
		rsrc_header* rsrc = (rsrc_header*) ((Ptr) &types + offset);
		
		do
		{
			if ( handle_in_ApplZone( rsrc->handle ) )
			{
				rsrc->handle = NULL;
			}
			
			++rsrc;
		}
		while ( n_rsrcs_1-- > 0 );
		
		++type;
	}
}

pascal void RsrcZoneInit_patch()
{
	release_ApplZone_resources();
	
	CurApRefNum = OpenResFile_handler( CurApName, 0 );
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
	ResErr = noErr;
	
	CurMap = refnum;
}

pascal void SetResLoad_patch( unsigned char load )
{
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
			
			if ( rsrc.attrs & resLocked )
			{
				mp.flags |= kHandleLockedMask;
			}
			
			if ( rsrc.attrs & resPurgeable )
			{
				mp.flags |= kHandlePurgeableMask;
			}
			
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
	
	ResErr = resNotFound;
	
	return NULL;
}

static
short CountResources_handler( ResType type : __D0 )
{
	UInt16 count = 0;
	
	RsrcMapHandle rsrc_map = find_rsrc_map( CurMap );
	
	while ( rsrc_map != NULL )
	{
		count += count_rsrcs( **rsrc_map, type );
		
		rsrc_map = (RsrcMapHandle) rsrc_map[0]->next_map;
	}
	
	ResErr = noErr;
	
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
		UInt16 count = count_rsrcs( **rsrc_map, type );
		
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
Handle GetNamedResource_handler( ResType type : __D0, const Byte* name : __A0 )
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
	ResErr = noErr;
}

static
void ReleaseResource_handler( Handle resource : __A0 )
{
	ResErr = noErr;
	
	DetachResource_patch( resource );
	
	/*
		TODO:  Check ResErr.
		
		For now we call DisposeHandle() unconditionally, so that we
		don't leak handles owned by non-map resources.
	*/
	
	DisposeHandle( resource );
}

asm
pascal void ReleaseResource_patch( Handle resource )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVEA.L  (A2)+,A0
	
	JSR      ReleaseResource_handler
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #4,SP
	
	JMP      (A0)
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
	
	ResErr = noErr;
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

static
long SizeRsrc_handler( Handle resource : __A0 )
{
	if ( const master_pointer* mp = (const master_pointer*) resource )
	{
		if ( mp->flags & kHandleIsResourceMask )
		{
			ResErr = noErr;
			
			return mac::glue::GetHandleSize_raw( resource );
		}
	}
	
	ResErr = resNotFound;
	
	return -1;
}

asm
pascal long SizeRsrc_patch( Handle resource )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVEA.L  (A2)+,A0
	
	JSR      SizeRsrc_handler
	MOVE.L   D0,(A2)
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #4,SP
	
	JMP      (A0)
}

static
void SetResAttrs_handler( Handle resource : __A0, short attrs : __D0 )
{
	OSErr err = resNotFound;
	
	if ( rsrc_header* rsrc = recover_rsrc_header( resource ) )
	{
		rsrc->attrs = attrs;
		
		err = noErr;
	}
	
	ResErr = err;
}

asm
pascal void SetResAttrs_patch( Handle resource, short attrs )
{
	MOVEM.L  D1-D2/A1-A2,-(SP)
	
	LEA      20(SP),A2
	MOVE.W   (A2)+,D0
	MOVEA.L  (A2)+,A0
	
	JSR      SetResAttrs_handler
	
	MOVEM.L  (SP)+,D1-D2/A1-A2
	
	MOVEA.L  (SP)+,A0
	
	ADDQ.L   #6,SP
	
	JMP      (A0)
}

static
void ChangedResource_handler( Handle resource : __A0 )
{
	OSErr err = resNotFound;
	
	if ( rsrc_header* rsrc = recover_rsrc_header( resource ) )
	{
		if ( rsrc->attrs & resProtected )
		{
			err = noErr;
		}
		else
		{
			ERROR = "ChangedResource is unimplemented";
			err = paramErr;
		}
	}
	
	ResErr = err;
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
void AddResource_handler( Handle            data : __A0,
                          ResType           type : __D0,
                          short             id   : __D1,
                          ConstStr255Param  name : __A1 )
{
	ERROR = "AddResource is unimplemented";
	
	ResErr = wrgVolTypErr;
}

asm
pascal void AddResource_patch( Handle            data,
                               ResType           type,
                               short             id,
                               ConstStr255Param  name )
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

static
void UpdateResFile_handler( short refnum : __D0 )
{
	ERROR = "UpdateResFile is unimplemented";
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
	OSErr err = resNotFound;
	
	if ( rsrc_header* rsrc = recover_rsrc_header( resource ) )
	{
		if ( (rsrc->attrs & (resProtected | resChanged)) != resChanged )
		{
			err = noErr;
		}
		else
		{
			ERROR = "WriteResource is unimplemented";
			err = paramErr;
		}
	}
	
	ResErr = err;
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

pascal void SetResPurge_patch( unsigned char install )
{
}

static
short Count1Resources_handler( ResType type : __D0 )
{
	ResErr = noErr;
	
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
