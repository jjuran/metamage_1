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
#include "vfs/node/types/anonymous.hh"


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
	
	void filehandle::Attach( vfs::filehandle* target )
	{
		p7::throw_errno( EINVAL );
	}
	
	node_ptr filehandle::GetFile()
	{
		const mode_t mode = S_IFIFO | permmode_from_openflags( GetFlags() );
		
		return vfs::new_anonymous_node( IOName( this, true ), mode, this );
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

