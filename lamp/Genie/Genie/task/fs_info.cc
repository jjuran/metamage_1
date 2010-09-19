/*
	fs_info.cc
	----------
*/

#include "Genie/task/fs_info.hh"


namespace Genie
{
	
	class fs_info_impl : public fs_info
	{
		public:
			boost::shared_ptr< IOHandle > its_cwd;
		
		public:
			fs_info_impl()
			{
			}
			
			fs_info_impl( const boost::shared_ptr< IOHandle >& cwd ) : its_cwd( cwd )
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
	
	
	static inline const boost::shared_ptr< IOHandle >& get_cwd( const fs_info* that )
	{
		return static_cast< const fs_info_impl* >( that )->its_cwd;
	}
	
	static inline boost::shared_ptr< IOHandle >& get_cwd( fs_info* that )
	{
		return static_cast< fs_info_impl* >( that )->its_cwd;
	}
	
	void fs_info::chdir( const boost::shared_ptr< IOHandle >& dir )
	{
		get_cwd( this ) = dir;
	}
	
	boost::shared_ptr< IOHandle > fs_info::getcwd() const
	{
		return get_cwd( this );
	}
	
	void fs_info::swap( fs_info& other )
	{
		Genie::swap( static_cast< fs_info_impl& >( *this ),
		             static_cast< fs_info_impl& >( other ) );
	}
	
	fs_info* fs_info::create( const boost::shared_ptr< IOHandle >& dir )
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

