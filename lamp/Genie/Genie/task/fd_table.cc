/*
	fd_table.cc
	-----------
	
	Copyright 2010, Joshua Juran
*/

#include "Genie/task/fd_table.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	bool fd_table::contains( int fd ) const
	{
		return unsigned( fd ) < its_fds.size()  &&  its_fds[ fd ].handle.get() != NULL;
	}
	
	FileDescriptor& fd_table::at( int fd )
	{
		if ( !contains( fd ) )
		{
			p7::throw_errno( EBADF );
		}
		
		return its_fds[ fd ];
	}
	
	FileDescriptor& fd_table::operator[]( int fd )
	{
		if ( fd < 0 )
		{
			p7::throw_errno( EBADF );
		}
		
		if ( fd >= its_fds.size() )
		{
			its_fds.resize( fd + 1 );
		}
		
		return its_fds[ fd ];
	}
	
	int fd_table::first_unused( int minimum )
	{
		const std::size_t n = its_fds.size();
		
		for ( int fd = minimum;  fd < n;  ++fd )
		{
			if ( its_fds[ fd ].handle.get() == NULL )
			{
				return fd;
			}
		}
		
		its_fds.resize( n + 1 );
		
		return n;
	}
	
	void fd_table::close( int fd )
	{
		FileDescriptor().swap( its_fds.at( fd ) );
	}
	
	void fd_table::for_each( void (*f)( void*, int, const FileDescriptor& ), void* param ) const
	{
		const std::size_t n = its_fds.size();
		
		for ( int fd = 0;  fd < n;  ++fd )
		{
			if ( its_fds[ fd ].handle.get() != NULL )
			{
				f( param, fd, its_fds[ fd ] );
			}
		}
		
	}
	
	void fd_table::for_each( void (*f)( void*, int, FileDescriptor& ), void* param )
	{
		const std::size_t n = its_fds.size();
		
		for ( int fd = 0;  fd < n;  ++fd )
		{
			if ( its_fds[ fd ].handle.get() != NULL )
			{
				f( param, fd, its_fds[ fd ] );
			}
		}
		
	}
	
	void fd_table::clear()
	{
		its_fds.clear();
	}
	
}

