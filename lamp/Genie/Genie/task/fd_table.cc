/*
	fd_table.cc
	-----------
	
	Copyright 2010, Joshua Juran
*/

#include "Genie/task/fd_table.hh"

// Standard C++
#include <vector>

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FileDescriptor.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	class fd_table_impl : public fd_table
	{
		public:
			std::vector< FileDescriptor > its_fds;
		
		public:
			fd_table_impl()
			{
			}
			
			fd_table_impl( const std::vector< FileDescriptor >& fds ) : its_fds( fds )
			{
			}
			
			void swap( fd_table_impl& other );
	};
	
	void fd_table_impl::swap( fd_table_impl& other )
	{
		its_fds.swap( other.its_fds );
	}
	
	inline void swap( fd_table_impl& a, fd_table_impl& b )
	{
		a.swap( b );
	}
	
	
	static inline const std::vector< FileDescriptor >& get_fds( const fd_table* that )
	{
		return static_cast< const fd_table_impl* >( that )->its_fds;
	}
	
	static inline std::vector< FileDescriptor >& get_fds( fd_table* that )
	{
		return static_cast< fd_table_impl* >( that )->its_fds;
	}
	
	bool fd_table::contains( int fd ) const
	{
		const std::vector< FileDescriptor >& fds = get_fds( this );
		
		return unsigned( fd ) < fds.size()  &&  fds[ fd ].handle.get() != NULL;
	}
	
	FileDescriptor& fd_table::at( int fd )
	{
		if ( !contains( fd ) )
		{
			p7::throw_errno( EBADF );
		}
		
		return get_fds( this )[ fd ];
	}
	
	FileDescriptor& fd_table::operator[]( int fd )
	{
		if ( fd < 0 )
		{
			p7::throw_errno( EBADF );
		}
		
		std::vector< FileDescriptor >& fds = get_fds( this );
		
		if ( fd >= fds.size() )
		{
			fds.resize( fd + 1 );
		}
		
		return fds[ fd ];
	}
	
	int fd_table::first_unused( int minimum )
	{
		std::vector< FileDescriptor >& fds = get_fds( this );
		
		const std::size_t n = fds.size();
		
		for ( int fd = minimum;  fd < n;  ++fd )
		{
			if ( fds[ fd ].handle.get() == NULL )
			{
				return fd;
			}
		}
		
		fds.resize( n + 1 );
		
		return n;
	}
	
	void fd_table::close( int fd )
	{
		FileDescriptor().swap( get_fds( this ).at( fd ) );
	}
	
	void fd_table::for_each( void (*f)( void*, int, const FileDescriptor& ), void* param ) const
	{
		const std::vector< FileDescriptor >& fds = get_fds( this );
		
		const std::size_t n = fds.size();
		
		for ( int fd = 0;  fd < n;  ++fd )
		{
			if ( fds[ fd ].handle.get() != NULL )
			{
				f( param, fd, fds[ fd ] );
			}
		}
		
	}
	
	void fd_table::for_each( void (*f)( void*, int, FileDescriptor& ), void* param )
	{
		std::vector< FileDescriptor >& fds = get_fds( this );
		
		const std::size_t n = fds.size();
		
		for ( int fd = 0;  fd < n;  ++fd )
		{
			if ( fds[ fd ].handle.get() != NULL )
			{
				f( param, fd, fds[ fd ] );
			}
		}
		
	}
	
	void fd_table::clear()
	{
		get_fds( this ).clear();
	}
	
	void fd_table::swap( fd_table& other )
	{
		Genie::swap( static_cast< fd_table_impl& >( *this ),
		             static_cast< fd_table_impl& >( other ) );
	}
	
	fd_table* fd_table::create()
	{
		return new fd_table_impl();
	}
	
	fd_table* duplicate( const fd_table& one )
	{
		return new fd_table_impl( get_fds( &one ) );
	}
	
	void destroy( const fd_table* x )
	{
		delete static_cast< const fd_table_impl* >( x );
	}
	
}

