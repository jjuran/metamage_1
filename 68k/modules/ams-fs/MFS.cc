/*
	MFS.cc
	------
*/

#include "MFS.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __STRINGCOMPARE__
#include <StringCompare.h>
#endif

// ams-common
#include "callouts.hh"
#include "FCB.hh"
#include "scoped_zone.hh"


struct logical_block
{
	uint8_t bytes[ 512 ];
};

struct fork_spec
{
	uint16_t  forkStBlk;
	uint32_t  forkLgLen;
	uint32_t  forkPyLen;
};

static inline
const fork_spec& get_fork( const mfs::file_directory_entry* entry, Byte rsrc )
{
	return (const fork_spec&) (rsrc ? entry->flRStBlk : entry->flStBlk);
}

static
const mfs::file_directory_entry* MFS_iterate( VCB* vcb, const mfs::_fde* prev )
{
	if ( prev )
	{
		const uint8_t* next = prev->flNam + (1 + prev->flNam[ 0 ] + 1 & 0xFE);
		
		const mfs::_fde* result = (const mfs::_fde*) next;
		
		if ( result->flAttrib < 0 )
		{
			return result;
		}
		
		return result->flAttrib < 0 ? result : NULL;
	}
	
	const logical_block* all_blocks = (logical_block*) vcb->vcbBufAdr;
	
	const logical_block* master_directory_block = all_blocks + 2;
	
	const logical_block* file_directory = all_blocks + vcb->vcbVBMSt;  // drDirSt
	
	return (const mfs::file_directory_entry*) file_directory;
}

const mfs::file_directory_entry* MFS_get_nth( VCB* vcb, short n )
{
	const mfs::file_directory_entry* entry = NULL;
	
	do
	{
		entry = MFS_iterate( vcb, entry );
	}
	while ( entry != NULL  &&  --n );
	
	return entry;
}

const mfs::file_directory_entry* MFS_lookup( VCB* vcb, const uint8_t* name )
{
	if ( name == NULL  ||  name[ 0 ] == '\0' )
	{
		return NULL;
	}
	
	const mfs::file_directory_entry* it = NULL;
	
	while (( it = MFS_iterate( vcb, it ) ))
	{
		if ( EqualString_sans_case( it->flNam, name ) )
		{
			return it;
		}
	}
	
	return NULL;
}

static
int16_t get_next( const uint8_t* block_map, int16_t curBlk )
{
	if ( curBlk == 1 )
	{
		return 0;
	}
	
	curBlk -= 2;
	
	if ( curBlk < 0 )
	{
		return -1;
	}
	
	block_map += 3 * curBlk / 2u;
	
	if ( curBlk & 1 )
	{
		return (block_map[ 0 ] & 0xF) << 8 | block_map[ 1 ];
	}
	else
	{
		return block_map[ 0 ] << 4 | block_map[ 1 ] >> 4;
	}
}

void MFS_load( VCB* vcb, uint16_t stBlk, Ptr buffer, int16_t n )
{
	Ptr p = buffer;
	
	const logical_block* all_blocks = (logical_block*) vcb->vcbBufAdr;
	
	const logical_block* master_directory_block = all_blocks + 2;
	
	const uint8_t* block_map = (const uint8_t*) master_directory_block + 64;
	
	const char* alloc_blocks = (const char*) (all_blocks + vcb->vcbAlBlSt);
	
	const uint32_t k = vcb->vcbAlBlkSiz;
	
	while ( stBlk > 1  &&  n-- > 0 )
	{
		p = (Ptr) fast_mempcpy( p, alloc_blocks + (stBlk - 2) * k, k );
		
		stBlk = get_next( block_map, stBlk );
	}
}

OSErr MFS_open_fork( short trap_word, FCB* fcb, const mfs::_fde* entry )
{
	scoped_zone null_zone;
	
	const Byte is_rsrc = trap_word;  // Open is A000, OpenRF is A00A
	
	const fork_spec& fork = get_fork( entry, is_rsrc );
	
	const size_t len = fork.forkPyLen;
	
	Ptr buffer = NewPtr( len );
	
	if ( buffer == NULL )
	{
		return memFullErr;
	}
	
	MFS_load( fcb->fcbVPtr, fork.forkStBlk, buffer, len / 512 );
	
	fcb->fcbFlNum  = entry->flNum;
	
	fcb->fcbTypByt = entry->flVersNum;
	fcb->fcbSBlk   = fork.forkStBlk;
	fcb->fcbEOF    = fork.forkLgLen;
	fcb->fcbPLen   = fork.forkPyLen;
	
	fcb->fcbBfAdr  = buffer;
	
	return noErr;
}

OSErr MFS_GetFileInfo( HFileParam* pb, const mfs::_fde* entry )
{
	const size_t n = offsetof( mfs::file_directory_entry, flNam );
	
	fast_memcpy( &pb->ioFlAttrib, entry, n );
	
	if ( pb->ioNamePtr )
	{
		ConstStr255Param name = entry->flNam;
		
		fast_memcpy( pb->ioNamePtr, name, 1 + name[ 0 ] );
	}
	
	return noErr;
}
