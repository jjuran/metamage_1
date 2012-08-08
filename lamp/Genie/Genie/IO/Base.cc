/*	=======
 *	Base.cc
 *	=======
 */

#include "Genie/IO/Base.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// iota
#include "iota/strings.hh"

// gear
#include "gear/hexidecimal.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"

// Genie
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"


#ifndef O_EXEC
#define O_EXEC  0
#endif


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	static plus::string IOName( const void* address, bool is_pipe )
	{
		plus::var_string name = is_pipe ? "pipe" : "socket";
		
		const size_t hex_offset = name.size() + STRLEN( ":[" );
		
		name += ":[12345678]";
		
		gear::encode_32_bit_hex( (long) address, &name[ hex_offset ] );
		
		return name;
	}
	
	static inline mode_t permmode_from_openflags( int flags )
	{
		// Not portable, but works in MacRelix
		const bool reading = flags & O_RDONLY;
		const bool writing = flags & O_WRONLY;
		const bool execing = flags & O_EXEC;
		
		return reading * 0400 | writing * 0200 | execing * 0100;
	}
	
	
	static filehandle_ptr anonymous_open( const node* it, int flags, mode_t mode );
	
	static const data_method_set anonymous_data_methods =
	{
		&anonymous_open
	};
	
	static const node_method_set anonymous_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&anonymous_data_methods
	};
	
	
	struct handle_extra
	{
		vfs::filehandle* handle;
	};
	
	static void dispose_handle( const node* it )
	{
		handle_extra& extra = *(handle_extra*) it->extra();
		
		intrusive_ptr_release( extra.handle );
	}
	
	
	static filehandle_ptr anonymous_open( const node* it, int flags, mode_t mode )
	{
		handle_extra& extra = *(handle_extra*) it->extra();
		
		return extra.handle;
	}
	
	
	filehandle::filehandle( int flags ) : itsOpenFlags( flags )
	{
	}
	
	filehandle::~filehandle()
	{
	}
	
	filehandle* filehandle::GetBaseForCast( Test test )
	{
		if ( (this->*test)() )
		{
			return this;
		}
		
		if ( filehandle* next = Next() )
		{
			return next->GetBaseForCast( test );
		}
		
		return NULL;
	}
	
	filehandle_ptr filehandle::Clone()
	{
		return this;
	}
	
	void filehandle::Attach( const filehandle_ptr& target )
	{
		p7::throw_errno( EINVAL );
	}
	
	node_ptr filehandle::GetFile()
	{
		node* result = new node( NULL,
		                         IOName( this, true ),
		                         S_IFIFO | permmode_from_openflags( GetFlags() ),
		                         &anonymous_methods,
		                         sizeof (handle_extra),
		                         &dispose_handle );
		
		handle_extra& extra = *(handle_extra*) result->extra();
		
		intrusive_ptr_add_ref( this );
		
		extra.handle = this;
		
		return result;
	}
	
	void filehandle::IOCtl( unsigned long request, int* argp )
	{
		if ( filehandle* next = Next() )
		{
			return next->IOCtl( request, argp );
		}
		
		p7::throw_errno( EINVAL );
	}
	
	memory_mapping_ptr
	//
	filehandle::Map( size_t length, int prot, int flags, off_t offset )
	{
		p7::throw_errno( ENODEV );
		
		return memory_mapping_ptr();
	}
	
}

