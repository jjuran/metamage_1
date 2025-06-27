/*
	argv.hh
	-------
*/

#ifndef RELIX_TASK_ARGV_HH
#define RELIX_TASK_ARGV_HH

// iota
#include "iota/class.hh"

// plus
#include "plus/var_string.hh"

// vxo
#include "vxo/ptrvec.hh"


namespace relix
{
	
	class argv
	{
		NON_COPYABLE( argv )
		NO_NEW_DELETE
		
		private:
			plus::var_string      its_string;
			vxo::UPtrVec< char >  its_vector;
		
		public:
			argv();
			
			void swap( argv& that );
			
			int get_argc() const  { return its_vector.size() - 1; }
			
			char* const* get_argv() const  { return &its_vector[ 0 ]; }
			
			const plus::string& get_string() const  { return its_string; }
			
			argv& assign( const char *const *args );
	};
	
	inline
	void swap( argv& a, argv& b )
	{
		a.swap( b );
	}
	
}

#endif
