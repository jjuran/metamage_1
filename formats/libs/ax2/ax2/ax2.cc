/*
	ax2.cc
	------
*/

#include "ax2/ax2.hh"

// POSIX
#include <sys/stat.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <string.h>

// iota
#include "iota/endian.hh"


namespace ax2
{

using iota::big_u16;
using iota::big_u32;
using iota::u16_from_big;
using iota::u32_from_big;

enum
{
	header_size = 26,
};

struct header
{
	uint32_t  magic;
	uint32_t  version;
	uint32_t  reserved[ 4 ];
	uint16_t  count;
};

struct entry_descriptor
{
	uint32_t  id;
	uint32_t  offset;
	uint32_t  length;
};

struct FileInfo_buffer
{
	uint8_t  finfo [ 16 ];
	uint8_t  fxinfo[ 16 ];
};

static
ssize_t pread_all( int fd, void* buffer, size_t n, off_t offset )
{
	ssize_t n_read = pread( fd, buffer, n, offset );
	
	if ( n_read >= 0  &&  n_read != n )
	{
		errno = ENOSPC;
		
		n_read = -1;
	}
	
	return n_read;
}

static
ssize_t pwrite_all( int fd, const void* buffer, size_t n, off_t offset )
{
	ssize_t n_written = pwrite( fd, buffer, n, offset );
	
	if ( n_written >= 0  &&  n_written != n )
	{
		errno = ENOSPC;
		
		n_written = -1;
	}
	
	return n_written;
}

static
ssize_t write_all( int fd, const void* buffer, size_t n )
{
	ssize_t n_written = write( fd, buffer, n );
	
	if ( n_written >= 0  &&  n_written != n )
	{
		errno = ENOSPC;
		
		n_written = -1;
	}
	
	return n_written;
}

static
ssize_t create_new_if_empty( int fd )
{
	struct stat st;
	
	int nok = fstat( fd, &st );
	
	if ( nok )
	{
		return nok;
	}
	
	if ( st.st_size != 0 )
	{
		return st.st_size;
	}
	
	FileInfo_buffer ffx = {};
	
	entry_descriptor entries[ 2 ];
	
	entries[ 0 ].id     = big_u32( 9 );
	entries[ 0 ].offset = big_u32( header_size + sizeof entries );
	entries[ 0 ].length = big_u32( sizeof ffx );
	
	entries[ 1 ].id     = big_u32( 2 );
	entries[ 1 ].offset = big_u32( header_size + sizeof entries + sizeof ffx );
	entries[ 1 ].length = big_u32( 0 );
	
	header h = {};
	
	h.magic   = big_u32( 0x00051607 );
	h.version = big_u32( 0x00020000 );
	
	h.count = big_u16( 2 );
	
	ssize_t n;
	
	(n = write_all( fd, &h,      header_size    )) < 0  ||
	(n = write_all( fd, entries, sizeof entries )) < 0  ||
	(n = write_all( fd, &ffx,    sizeof ffx     ));
	
	return n < 0 ? n : 0;
}

static inline
ssize_t get_nth_entry( int fd, entry_descriptor& entry, unsigned i )
{
	const off_t offset = header_size + i * sizeof entry;
	
	return pread_all( fd, &entry, sizeof entry, offset );
}

static inline
ssize_t set_entry_length( int fd, const entry_descriptor& entry, unsigned i )
{
	const off_t offset = header_size + i * sizeof entry + 8;
	
	return pwrite_all( fd, &entry.length, sizeof entry.length, offset );
}

static inline
ssize_t get_FileInfo_entry( int fd, entry_descriptor& entry )
{
	return get_nth_entry( fd, entry, 0 );
}

static
ssize_t get_RsrcFork_entry( int fd, entry_descriptor& entry )
{
	return get_nth_entry( fd, entry, 1 );
}

static inline
ssize_t set_RsrcFork_length( int fd, const entry_descriptor& entry )
{
	return set_entry_length( fd, entry, 1 );
}

ssize_t get_FInfo( int fd, void* buffer, size_t length )
{
	entry_descriptor info;
	
	ssize_t n_read = get_FileInfo_entry( fd, info );
	
	if ( n_read < 0 )
	{
		return n_read;
	}
	
	const size_t info_length = u32_from_big( info.length );
	
	if ( length > info_length )
	{
		length = info_length;
	};
	
	return pread_all( fd, buffer, length, u32_from_big( info.offset ) );
}

ssize_t set_FInfo( int fd, const void* buffer, size_t length )
{
	create_new_if_empty( fd );
	
	entry_descriptor info;
	
	ssize_t n_read = get_FileInfo_entry( fd, info );
	
	if ( n_read < 0 )
	{
		return n_read;
	}
	
	if ( length > u32_from_big( info.length ) )
	{
		errno = ENOSPC;
		
		return -1;
	};
	
	return pwrite_all( fd, buffer, length, u32_from_big( info.offset ) );
}

off_t geteof_rsrc( int fd )
{
	entry_descriptor rsrc;
	
	ssize_t n_read = get_RsrcFork_entry( fd, rsrc );
	
	if ( n_read < 0 )
	{
		return n_read;
	}
	
	return u32_from_big( rsrc.length );
}

int seteof_rsrc( int fd, off_t eof )
{
	ssize_t created = create_new_if_empty( fd );
	
	entry_descriptor rsrc;
	
	ssize_t n_read = get_RsrcFork_entry( fd, rsrc );
	
	if ( n_read < 0 )
	{
		return n_read;
	}
	
	rsrc.length = big_u32( eof );
	
	ssize_t set = set_RsrcFork_length( fd, rsrc );
	
	if ( set < 0 )
	{
		return set;
	}
	
	int nok = ftruncate( fd, u32_from_big( rsrc.offset ) + eof );
	
	return nok;
}

ssize_t slurp_rsrc( int fd, void* p, size_t n )
{
	entry_descriptor rsrc;
	
	ssize_t n_read = get_RsrcFork_entry( fd, rsrc );
	
	if ( n_read < 0 )
	{
		return n_read;
	}
	
	const size_t rsrc_length = u32_from_big( rsrc.length );
	
	if ( n >= rsrc_length )
	{
		n = rsrc_length;
	}
	
	if ( n == 0 )
	{
		return 0;
	}
	
	return pread( fd, p, n, u32_from_big( rsrc.offset ) );
}

ssize_t pread_rsrc( int fd, void* p, size_t n, off_t offset )
{
	entry_descriptor rsrc;
	
	ssize_t n_read = get_RsrcFork_entry( fd, rsrc );
	
	if ( n_read < 0 )
	{
		return n_read;
	}
	
	const size_t rsrc_length = u32_from_big( rsrc.length );
	
	if ( offset >= rsrc_length )
	{
		return 0;
	}
	
	if ( n >= rsrc_length - offset )
	{
		n = rsrc_length - offset;
	}
	
	return pread( fd, p, n, u32_from_big( rsrc.offset ) + offset );
}

ssize_t splat_rsrc( int fd, const void* p, size_t n )
{
	create_new_if_empty( fd );
	
	entry_descriptor rsrc;
	
	ssize_t n_read = get_RsrcFork_entry( fd, rsrc );
	
	if ( n_read < 0 )
	{
		return n_read;
	}
	
	ssize_t n_written = pwrite( fd, p, n, u32_from_big( rsrc.offset ) );
	
	if ( n_written > 0 )
	{
		if ( n_written != u32_from_big( rsrc.length ) )
		{
			rsrc.length = big_u32( n_written );
			
			ssize_t set = set_RsrcFork_length( fd, rsrc );
			
			if ( set < 0 )
			{
				return set;
			}
		}
	}
	
	return n_written;
}

ssize_t pwrite_rsrc( int fd, const void* p, size_t n, off_t offset )
{
	create_new_if_empty( fd );
	
	entry_descriptor rsrc;
	
	ssize_t n_read = get_RsrcFork_entry( fd, rsrc );
	
	if ( n_read < 0 )
	{
		return n_read;
	}
	
	ssize_t n_written = pwrite( fd, p, n, u32_from_big( rsrc.offset ) + offset );
	
	if ( n_written > 0 )
	{
		offset += n_written;
		
		if ( offset > rsrc.length )
		{
			rsrc.length = big_u32( offset );
			
			ssize_t set = set_RsrcFork_length( fd, rsrc );
			
			if ( set < 0 )
			{
				return set;
			}
		}
	}
	
	return n_written;
}

}
