/*
	filehandle.cc
	-------------
*/

#include "vfs/filehandle.hh"

// POSIX
#include <fcntl.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/memory_mapping.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"


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
	
	
	void filehandle::Attach( vfs::filehandle* target )
	{
		p7::throw_errno( EINVAL );
	}
	
	void filehandle::set_file( const node& file )
	{
		its_file = &file;
	}
	
}

