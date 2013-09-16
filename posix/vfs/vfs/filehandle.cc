/*
	filehandle.cc
	-------------
*/

#include "vfs/filehandle.hh"

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
#include "vfs/memory_mapping.hh"
#include "vfs/node.hh"
#include "vfs/node/types/anonymous.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"


#ifndef O_EXEC
#define O_EXEC  0
#endif


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	filehandle::filehandle( int                           flags,
	                        const filehandle_method_set*  methods,
	                        std::size_t                   n_extra,
	                        filehandle_destructor         dtor )
	:
		its_mark   (         ),
		its_flags  ( flags   ),
		its_methods( methods ),
		its_extra  ( n_extra ? ::operator new( n_extra ) : NULL ),
		its_destructor( dtor )
	{
	}
	
	filehandle::filehandle( const node*                   file,
	                        int                           flags,
	                        const filehandle_method_set*  methods,
	                        std::size_t                   n_extra,
	                        filehandle_destructor         dtor )
	:
		its_mark   (         ),
		its_file   ( file    ),
		its_flags  ( flags   ),
		its_methods( methods ),
		its_extra  ( n_extra ? ::operator new( n_extra ) : NULL ),
		its_destructor( dtor )
	{
	}
	
	filehandle::~filehandle()
	{
		if ( its_destructor )
		{
			its_destructor( this );
		}
		
		::operator delete( its_extra );
	}
	
	const bstore_method_set& filehandle::bstore_methods() const
	{
		if ( !its_methods  ||  !its_methods->bstore_methods )
		{
			// This confuses MWCPPC when optimizing:
			//p7::throw_errno( IsDirectory() ? EISDIR : ESPIPE );
			// internal compiler error: File: 'PCodeUtilities.c' Line: 80
			
			const volatile int error = IsDirectory() ? EISDIR : ESPIPE;
			
			p7::throw_errno( error );
		}
		
		return *its_methods->bstore_methods;
	}
	
	const socket_method_set& filehandle::socket_methods() const
	{
		if ( !its_methods  ||  !its_methods->socket_methods )
		{
			p7::throw_errno( ENOTSOCK );
		}
		
		return *its_methods->socket_methods;
	}
	
	const terminal_method_set& filehandle::terminal_methods() const
	{
		if ( !its_methods  ||  !its_methods->terminal_methods )
		{
			p7::throw_errno( ENOTTY );
		}
		
		return *its_methods->terminal_methods;
	}
	
	
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
	
	void filehandle::Attach( vfs::filehandle* target )
	{
		p7::throw_errno( EINVAL );
	}
	
	node_ptr filehandle::GetFile()
	{
		if ( its_file.get() )
		{
			return its_file;
		}
		
		const bool is_pipe = !(its_methods  &&  its_methods->socket_methods);
		
		const mode_t mode = S_IFIFO | permmode_from_openflags( get_flags() );
		
		return vfs::new_anonymous_node( IOName( this, is_pipe ), mode, this );
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

