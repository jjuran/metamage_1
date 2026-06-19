/*
	mac-generic.cc
	--------------
*/

#include "vfs/node/types/mac.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// Standard C
#include <string.h>

// iota
#include "iota/class.hh"
#include "iota/endian.hh"

// gear
#include "gear/find.hh"

// d1904z
#include "d1904z.hh"

// ax2
#include "ax2/ax2.hh"

// plus
#include "plus/string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/types/ax2.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node.hh"
#include "vfs/node/types/namedfork.hh"


namespace vfs
{

using namespace ax2;

namespace p7 = poseven;

using iota::big_u16;
using iota::big_u32;
using iota::u16_from_big;
using iota::u32_from_big;


static inline
size_t round_up_to_x512( size_t x )
{
	return (x + 511) & ~511;
}

class scoped_fd
{
	NON_COPYABLE( scoped_fd )
	
	private:
		int its_fd;
	
	public:
		scoped_fd( const vxo::datum_storage& datum, int flags, mode_t mode );
		
		~scoped_fd();
		
		operator int() const  { return its_fd; }
		
		void commit()  { its_fd = -1; }
};

scoped_fd::scoped_fd( const vxo::datum_storage&  datum,
                      int                        flags,
                      mode_t                     mode = 0666 )
{
	const plus::string& s = *(const plus::string*) &datum;
	
	its_fd = open( s.c_str(), flags, mode );
}

scoped_fd::~scoped_fd()
{
	close( its_fd );  // close( -1 ) is harmless
}

struct macinfo_extra_generic
{
	vxo::datum_storage  ax2_path;
	struct stat         st;
	special_info        type;
};

typedef macinfo_extra_generic macinfo_extra;

static
void macinfo_stat( const node*   that,
                   struct stat&  st )
{
	macinfo_extra& extra = *(macinfo_extra*) that->extra();
	
	memset( &st, '\0', sizeof st );
	
	st.st_mode = extra.st.st_mode;
	
	st.st_size = info_sizes[ extra.type ];
}

static
plus::string macinfo_slurp( const node* that )
{
	macinfo_extra& extra = *(macinfo_extra*) that->extra();
	
	if ( extra.type == Info_SetFInfo )
	{
		p7::throw_errno( EPERM );
	}
	
	scoped_fd fd( extra.ax2_path, O_RDONLY );
	
	plus::string result;
	
	const size_t size = info_sizes[ extra.type ];
	
	char* begin = result.reset( size );
	
	if ( extra.type == Info_GetFInfo )
	{
		*begin++ = 0;  // ioFlAttrib -- FIXME
		*begin++ = 0;  // ioFlVersNum
	}
	
	uint32_t* p4 = (uint32_t*) begin;
	
	*p4++ = '*' * 0x01010101;  // type
	*p4++ = '?' * 0x01010101;  // creator
	
	uint16_t* p2 = (uint16_t*) p4;
	
	if ( extra.type >= Info_FInfo )
	{
		memset( p2, '\0', 4 * sizeof (uint16_t) );
		
		p2 += 4;
	}
	
	get_FInfo( fd, begin, size );
	
	if ( extra.type == Info_GetFInfo )
	{
		uint32_t rsrc_log_eof = 0;
		uint32_t rsrc_phy_eof = 0;
		
		off_t eof = geteof_rsrc( fd );
		
		if ( eof > 0 )
		{
			rsrc_log_eof = eof;
			rsrc_phy_eof = round_up_to_x512( eof );
		}
		
		p4 = (uint32_t*) p2;
		
		*p4++ = big_u32( extra.st.st_ino );  // Host information leak?
		
		p2 = (uint16_t*) p4;
		
		*p2++ = 0;  // ioFlStBlk
		
		p4 = (uint32_t*) p2;
		
		*p4++ = big_u32( extra.st.st_size );
		*p4++ = big_u32( extra.st.st_blocks * 512 );
		
		p2 = (uint16_t*) p4;
		
		*p2++ = 0;  // ioFlRStBlk
		
		p4 = (uint32_t*) p2;
		
		*p4++ = rsrc_log_eof;
		*p4++ = rsrc_phy_eof;
		*p4++ = 0;
		*p4++ = big_u32( d1904_from_time( extra.st.st_mtime ) );
	}
	
	return result;
}

static
void macinfo_splat( const node* that, const char* data, size_t size )
{
	macinfo_extra& extra = *(macinfo_extra*) that->extra();
	
	if ( extra.type == Info_GetFInfo )
	{
		p7::throw_errno( EPERM );
	}
	
	const size_t expected_size = info_sizes[ extra.type ];
	
	if ( size != expected_size )
	{
		p7::throw_errno( EINVAL );
	}
	
	scoped_fd fd( extra.ax2_path, O_RDWR | O_CREAT );
	
	if ( size > 16 )
	{
		size = 16;  // disregard SetFInfo, only set FInfo portion
	}
	
	ssize_t n_written = set_FInfo( fd, data, size );
	
	p7::throw_posix_result( n_written );
}

static
void macrsrc_stat( const node*   that,
                   struct stat&  st )
{
	macinfo_extra& extra = *(macinfo_extra*) that->extra();
	
	memset( &st, '\0', sizeof st );
	
	scoped_fd fd( extra.ax2_path, O_RDONLY );
	
	off_t eof = geteof_rsrc( fd );
	
	if ( eof < 0 )
	{
		eof = 0;
	}
	
	st.st_mode = extra.st.st_mode;
	st.st_size = eof;
}

static
filehandle_ptr macrsrc_open( const node* that, int flags, mode_t mode )
{
	macinfo_extra& extra = *(macinfo_extra*) that->extra();
	
	const bool truncating = flags & O_TRUNC;
	const int  accmode    = flags & O_ACCMODE;
	
	if ( accmode == O_WRONLY )
	{
		flags += O_RDWR - O_WRONLY;
	}
	
	if ( accmode != O_RDONLY )
	{
		flags |= O_CREAT;
	}
	
	flags &= ~O_TRUNC;
	
	scoped_fd fd( extra.ax2_path, flags, mode );
	
	p7::throw_posix_result( fd );
	
	if ( truncating )
	{
		int nok = seteof_rsrc( fd, 0 );
		
		p7::throw_posix_result( nok );
	}
	
	filehandle_ptr fh = new_ax2_rsrc_access( flags, fd );
	
	fd.commit();
	
	return fh;
}

static
plus::string macrsrc_slurp( const node* that )
{
	macinfo_extra& extra = *(macinfo_extra*) that->extra();
	
	scoped_fd fd( extra.ax2_path, O_RDONLY );
	
	plus::string result;
	
	off_t eof = geteof_rsrc( fd );
	
	if ( eof > 0 )
	{
		char* p = result.reset( eof );
		
		ssize_t n_read = slurp_rsrc( fd, p, eof );
		
		if ( (size_t) n_read < eof )
		{
			errno = ENOSPC;
			
			n_read = -1;
		}
		
		p7::throw_posix_result( n_read );
	}
	
	return result;
}

static
void macrsrc_splat( const node* that, const char* data, size_t size )
{
	macinfo_extra& extra = *(macinfo_extra*) that->extra();
	
	scoped_fd fd( extra.ax2_path, O_RDWR | O_CREAT );
	
	ssize_t n_written = splat_rsrc( fd, data, size );
	
	p7::throw_posix_result( n_written );
}

static const item_method_set macinfo_item_methods =
{
	&macinfo_stat,
};

static const data_method_set macinfo_data_methods =
{
	NULL,
	NULL,
	NULL,
	&macinfo_slurp,
	&macinfo_splat,
};

static const node_method_set macinfo_methods =
{
	&macinfo_item_methods,
	&macinfo_data_methods,
};

static const item_method_set macrsrc_item_methods =
{
	&macrsrc_stat,
};

static const data_method_set macrsrc_data_methods =
{
	&macrsrc_open,
	NULL,
	NULL,
	&macrsrc_slurp,
	&macrsrc_splat,
};

static const node_method_set macrsrc_methods =
{
	&macrsrc_item_methods,
	&macrsrc_data_methods,
};

static
node_ptr new_info( special_info  type,
                   const char*   path,
                   const node*   parent,
                   uid_t         user )
{
	struct stat st;
	
	int nok = stat( path, &st );
	
	if ( nok )
	{
		return node_ptr();  // A call ancestor will deal with it
	}
	
	mode_t mode = st.st_mode;
	
	const node_method_set* methods = type ? &macinfo_methods
	                                      : &macrsrc_methods;
	
	node* result = new node( parent,
	                         "<info>",
	                         mode,
	                         user,
	                         methods,
	                         sizeof (macinfo_extra) );
	
	macinfo_extra& extra = *(macinfo_extra*) result->extra();
	
	plus::string ax2_path;
	
	size_t len = strlen( path );
	
	char* p = ax2_path.reset( len + 2 );  // add space for "._"
	
	const char* q = path;
	
	const char* slash = gear::find_last_match( path, len, '/' );
	
	if ( slash )
	{
		q = slash + 1;
		
		size_t n = q - path;
		
		memcpy( p, path, n );
		
		p += n;
		
		len -= n;
	}
	
	*p++ = '.';
	*p++ = '_';
	
	memcpy( p, q, len );
	
	vxo::construct_from_move( extra.ax2_path, ax2_path.move() );
	
	extra.st   = st;
	extra.type = type;
	
	return result;
}

#ifdef __APPLE__
#define mac_lookup_info  mac_lookup_info_generic
#endif

node_ptr mac_lookup_info( const char*  path,
                          const char*  fork_name,
                          const node*  parent,
                          uid_t        user )
{
	special_info type = Info_null;
	
	/*
		We're overloading Info_null to mean ..namedfork/rsrc.
	*/
	
	if ( strcmp( fork_name, "rsrc" ) != 0 )
	{
		if ( strcmp( fork_name, "PkgInfo" ) == 0 )
		{
			type = Info_PkgInfo;
		}
		else if ( strcmp( fork_name, "FInfo" ) == 0 )
		{
			type = Info_FInfo;
		}
		else if ( strcmp( fork_name, "GetFInfo" ) == 0 )
		{
			type = Info_GetFInfo;
		}
		else if ( strcmp( fork_name, "SetFInfo" ) == 0 )
		{
			type = Info_SetFInfo;
		}
		else
		{
			return node_ptr();
		}
	}
	
	return new_info( type, path, parent, user );
}

}
