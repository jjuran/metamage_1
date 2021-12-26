/*
	fs_info.hh
	----------
*/

#ifndef RELIX_TASK_FSINFO_HH
#define RELIX_TASK_FSINFO_HH

// vxo
#include "vxo/ref_count.hh"

// vfs
#include "vfs/filehandle_ptr.hh"


namespace relix
{
	
	class fs_info;
	
	void destroy( const fs_info* x );
	
	
	class fs_info : public vxo::ref_count< fs_info >
	{
		private:
			// Private, undefined assignment operator prevents std::swap()
			fs_info& operator=( const fs_info& );
		
		protected:
			// Protected destructor prevents slicing
			~fs_info()  {}
		
		public:
			void chdir( const vfs::filehandle_ptr& dir );
			
			vfs::filehandle_ptr getcwd() const;
			
			void swap( fs_info& other );
			
			static fs_info* create( const vfs::filehandle_ptr& dir );
	};
	
	inline void swap( fs_info& a, fs_info& b )
	{
		a.swap( b );
	}
	
	fs_info* duplicate( const fs_info& one );
	
}

#endif
