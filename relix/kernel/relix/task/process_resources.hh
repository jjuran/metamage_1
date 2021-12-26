/*
	process_resources.hh
	--------------------
*/

#ifndef RELIX_TASK_PROCESSRESOURCES_HH
#define RELIX_TASK_PROCESSRESOURCES_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// vxo
#include "vxo/ref_count.hh"


namespace relix
{
	
	class fd_map;
	class fs_info;
	
}

namespace relix
{
	
	class process_resources : public vxo::ref_count< process_resources >
	{
		private:
			boost::intrusive_ptr< fs_info > its_fs_info;
			boost::intrusive_ptr< fd_map  > its_fd_map;
		
		public:
			process_resources();
			
			process_resources( const process_resources& that );
			
			~process_resources();
			
			fs_info& get_fs_info() const;
			fd_map&  get_fd_map () const;
			
			void unshare_fs_info();
			void unshare_fd_map();
	};
	
}

#endif
