/*
	fs_info.hh
	----------
*/

#ifndef GENIE_TASK_FSINFO_HH
#define GENIE_TASK_FSINFO_HH

// plus
#include "plus/ref_count.hh"

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	class IOHandle;
	
	
	class fs_info;
	
	void destroy( const fs_info* x );
	
	
	class fs_info : public plus::ref_count< fs_info >
	{
		private:
			// Private, undefined assignment operator prevents std::swap()
			fs_info& operator=( const fs_info& );
		
		protected:
			// Protected destructor prevents slicing
			~fs_info()  {}
		
		public:
			void chdir( const boost::shared_ptr< IOHandle >& dir );
			
			boost::shared_ptr< IOHandle > getcwd() const;
			
			void swap( fs_info& other );
			
			static fs_info* create( const boost::shared_ptr< IOHandle >& dir );
	};
	
	inline void swap( fs_info& a, fs_info& b )
	{
		a.swap( b );
	}
	
	fs_info* duplicate( const fs_info& one );
	
}

#endif

