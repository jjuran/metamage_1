/*
	fd_map.cc
	---------
*/

#include "relix/task/fd_map.hh"

// Standard C++
#include <vector>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/file_descriptor.hh"


namespace relix
{
	
	namespace p7 = poseven;
	
	using vfs::file_descriptor;
	
	
	class fd_map_impl : public fd_map
	{
		public:
			std::vector< file_descriptor > its_fds;
		
		public:
			fd_map_impl()
			{
			}
			
			fd_map_impl( const std::vector< file_descriptor >& fds ) : its_fds( fds )
			{
			}
			
			void swap( fd_map_impl& other );
	};
	
	void fd_map_impl::swap( fd_map_impl& other )
	{
		its_fds.swap( other.its_fds );
	}
	
	inline void swap( fd_map_impl& a, fd_map_impl& b )
	{
		a.swap( b );
	}
	
	
	static inline const std::vector< file_descriptor >& get_fds( const fd_map* that )
	{
		return static_cast< const fd_map_impl* >( that )->its_fds;
	}
	
	static inline std::vector< file_descriptor >& get_fds( fd_map* that )
	{
		return static_cast< fd_map_impl* >( that )->its_fds;
	}
	
	bool fd_map::contains( int fd ) const
	{
		const std::vector< file_descriptor >& fds = get_fds( this );
		
		return unsigned( fd ) < fds.size()  &&  fds[ fd ].handle.get() != NULL;
	}
	
	file_descriptor& fd_map::at( int fd )
	{
		if ( !contains( fd ) )
		{
			p7::throw_errno( EBADF );
		}
		
		return get_fds( this )[ fd ];
	}
	
	file_descriptor& fd_map::operator[]( int fd )
	{
		if ( fd < 0 )
		{
			p7::throw_errno( EBADF );
		}
		
		std::vector< file_descriptor >& fds = get_fds( this );
		
		if ( fd >= fds.size() )
		{
			fds.resize( fd + 1 );
		}
		
		return fds[ fd ];
	}
	
	int fd_map::first_unused( int minimum )
	{
		std::vector< file_descriptor >& fds = get_fds( this );
		
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
	
	void fd_map::close( int fd )
	{
		file_descriptor().swap( get_fds( this ).at( fd ) );
	}
	
	void fd_map::for_each( void (*f)( void*, int, const file_descriptor& ), void* param ) const
	{
		const std::vector< file_descriptor >& fds = get_fds( this );
		
		const std::size_t n = fds.size();
		
		for ( int fd = 0;  fd < n;  ++fd )
		{
			if ( fds[ fd ].handle.get() != NULL )
			{
				f( param, fd, fds[ fd ] );
			}
		}
		
	}
	
	void fd_map::for_each( void (*f)( void*, int, file_descriptor& ), void* param )
	{
		std::vector< file_descriptor >& fds = get_fds( this );
		
		const std::size_t n = fds.size();
		
		for ( int fd = 0;  fd < n;  ++fd )
		{
			if ( fds[ fd ].handle.get() != NULL )
			{
				f( param, fd, fds[ fd ] );
			}
		}
		
	}
	
	void fd_map::clear()
	{
		get_fds( this ).clear();
	}
	
	void fd_map::swap( fd_map& other )
	{
		relix::swap( static_cast< fd_map_impl& >( *this ),
		             static_cast< fd_map_impl& >( other ) );
	}
	
	fd_map* fd_map::create()
	{
		return new fd_map_impl();
	}
	
	fd_map* duplicate( const fd_map& one )
	{
		return new fd_map_impl( get_fds( &one ) );
	}
	
	void destroy( const fd_map* x )
	{
		delete static_cast< const fd_map_impl* >( x );
	}
	
}

