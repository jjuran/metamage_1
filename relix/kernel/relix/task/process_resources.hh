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

// plus
#include "plus/ref_count.hh"


namespace relix
{
	
	class fs_info;
	
}

namespace relix
{
	
	class process_resources : public plus::ref_count< process_resources >
	{
		private:
			boost::intrusive_ptr< fs_info > its_fs_info;
		
		public:
			process_resources();
			
			process_resources( const process_resources& that );
			
			~process_resources();
			
			fs_info& get_fs_info() const;
			
			void unshare_fs_info();
	};
	
}

#endif

