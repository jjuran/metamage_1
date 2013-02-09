/*
	fs_info.cc
	----------
*/

#include "Genie/task/fs_info.hh"

// vfs
#include "vfs/filehandle.hh"


namespace relix
{
	
	class fs_info_impl : public fs_info
	{
		public:
			vfs::filehandle_ptr its_cwd;
		
		public:
			fs_info_impl()
			{
			}
			
			fs_info_impl( const vfs::filehandle_ptr& cwd ) : its_cwd( cwd )
			{
			}
			
			void swap( fs_info_impl& other );
	};
	
	void fs_info_impl::swap( fs_info_impl& other )
	{
		its_cwd.swap( other.its_cwd );
	}
	
	inline void swap( fs_info_impl& a, fs_info_impl& b )
	{
		a.swap( b );
	}
	
	
	static inline const vfs::filehandle_ptr& get_cwd( const fs_info* that )
	{
		return static_cast< const fs_info_impl* >( that )->its_cwd;
	}
	
	static inline vfs::filehandle_ptr& get_cwd( fs_info* that )
	{
		return static_cast< fs_info_impl* >( that )->its_cwd;
	}
	
	void fs_info::chdir( const vfs::filehandle_ptr& dir )
	{
		get_cwd( this ) = dir;
	}
	
	vfs::filehandle_ptr fs_info::getcwd() const
	{
		return get_cwd( this );
	}
	
	void fs_info::swap( fs_info& other )
	{
		relix::swap( static_cast< fs_info_impl& >( *this ),
		             static_cast< fs_info_impl& >( other ) );
	}
	
	fs_info* fs_info::create( const vfs::filehandle_ptr& dir )
	{
		return new fs_info_impl( dir );
	}
	
	fs_info* duplicate( const fs_info& one )
	{
		return new fs_info_impl( get_cwd( &one ) );
	}
	
	void destroy( const fs_info* x )
	{
		delete static_cast< const fs_info_impl* >( x );
	}
	
}

