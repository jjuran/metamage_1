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

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"


#ifndef O_EXEC
#define O_EXEC  0
#endif


namespace Genie
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
	
	static inline mode_t permmode_from_openflags( OpenFlags flags )
	{
		// Not portable, but works in MacRelix
		const bool reading = flags & O_RDONLY;
		const bool writing = flags & O_WRONLY;
		const bool execing = flags & O_EXEC;
		
		return reading * 0400 | writing * 0200 | execing * 0100;
	}
	
	
	static IOPtr anonymous_open( const FSTree* node, int flags, mode_t mode );
	
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
		IOHandle* handle;
	};
	
	static void dispose_handle( const FSTree* node )
	{
		handle_extra& extra = *(handle_extra*) node->extra();
		
		intrusive_ptr_release( extra.handle );
	}
	
	
	static IOPtr anonymous_open( const FSTree* node, int flags, mode_t mode )
	{
		handle_extra& extra = *(handle_extra*) node->extra();
		
		return extra.handle;
	}
	
	
	IOHandle::IOHandle( OpenFlags flags ) : itsOpenFlags( flags )
	{
	}
	
	IOHandle::~IOHandle()
	{
	}
	
	IOHandle* IOHandle::GetBaseForCast( Test test )
	{
		if ( (this->*test)() )
		{
			return this;
		}
		
		if ( IOHandle* next = Next() )
		{
			return next->GetBaseForCast( test );
		}
		
		return NULL;
	}
	
	IOPtr IOHandle::Clone()
	{
		return this;
	}
	
	void IOHandle::Attach( const IOPtr& target )
	{
		p7::throw_errno( EINVAL );
	}
	
	FSTreePtr IOHandle::GetFile()
	{
		FSTree* result = new FSTree( NULL,
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
	
	void IOHandle::IOCtl( unsigned long request, int* argp )
	{
		if ( IOHandle* next = Next() )
		{
			return next->IOCtl( request, argp );
		}
		
		p7::throw_errno( EINVAL );
	}
	
	boost::intrusive_ptr< memory_mapping >
	//
	IOHandle::Map( size_t length, off_t offset )
	{
		p7::throw_errno( ENODEV );
		
		return boost::intrusive_ptr< memory_mapping >();
	}
	
	void Check_IOHandle_Cast( const void* cast, int errorToThrow )
	{
		if ( cast == NULL )
		{
			p7::throw_errno( errorToThrow );
		}
	}
	
}

