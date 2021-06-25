/*
	filehandle.cc
	-------------
*/

#include "vfs/filehandle.hh"

// POSIX
#include <fcntl.h>

// Extended API Set, part 2
#include "extended-api-set/part-2.h"

// Debug
#include "debug/boost_assert.hh"

// boost
#include <boost/intrusive_ptr.hpp>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/memory_mapping.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	filehandle::filehandle( const node*                   file,
	                        int                           flags,
	                        const filehandle_method_set*  methods,
	                        size_t                        n_extra,
	                        filehandle_destructor         dtor )
	:
		its_mark   (         ),
		its_file   ( file    ),
		its_flags  ( flags   ),
		its_methods( methods ),
		its_extra  ( n_extra ? ::operator new( n_extra ) : NULL ),
		its_destructor( dtor )
	{
		if ( its_file )
		{
			intrusive_ptr_add_ref( its_file );
		}
	}
	
	filehandle::~filehandle()
	{
		if ( its_destructor )
		{
			its_destructor( this );
		}
		
		if ( its_file )
		{
			intrusive_ptr_release( its_file );
		}
		
		::operator delete( its_extra );
	}
	
	const bstore_method_set& filehandle::bstore_methods() const
	{
		if ( !its_methods  ||  !its_methods->bstore_methods )
		{
			const bool is_directory = its_flags & O_DIRECTORY;
			
			p7::throw_errno( is_directory ? EISDIR : ESPIPE );
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
	
	
	void filehandle::set_file( const node& file )
	{
		intrusive_ptr_add_ref( &file );
		
		if ( its_file )
		{
			intrusive_ptr_release( its_file );
		}
		
		its_file = &file;
	}
	
}
