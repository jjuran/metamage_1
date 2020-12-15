/*
	argv.hh
	-------
*/

#ifndef RELIX_TASK_ARGV_HH
#define RELIX_TASK_ARGV_HH

// plus
#include "plus/var_string.hh"

// vxo
#include "vxo/ptrvec.hh"


namespace relix
{
	
	class argv
	{
		private:
			plus::var_string      its_string;
			vxo::UPtrVec< char >  its_vector;
		
		public:
			argv();
			
			argv( const plus::string& s );
			
			argv( const char *const *args );
			
			argv( const argv& that );
			
			argv& operator=( const argv& that );
			
			void swap( argv& that );
			
			int get_argc() const  { return its_vector.size() - 1; }
			
			char* const* get_argv() const  { return &its_vector[ 0 ]; }
			
			const plus::string& get_string() const  { return its_string; }
			
			argv& assign( const plus::string& s );
			
			argv& assign( const char *const *args );
	};
	
	inline
	void swap( argv& a, argv& b )
	{
		a.swap( b );
	}
	
}

#endif
