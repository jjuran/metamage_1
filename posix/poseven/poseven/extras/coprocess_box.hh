/*
	coprocess_box.hh
	----------------
*/

#ifndef POSEVEN_EXTRAS_COPROCESSBOX_HH
#define POSEVEN_EXTRAS_COPROCESSBOX_HH

// POSIX
#include <sys/types.h>


namespace poseven
{
	
	class coprocess;
	
	class coprocess_box
	{
		private:
			coprocess* its_coprocess;
		
		public:
			coprocess_box() : its_coprocess()
			{
			}
			
			~coprocess_box();
			
			coprocess_box           ( const coprocess_box& );
			coprocess_box& operator=( const coprocess_box& );
			
			void swap( coprocess_box& that );
			
			coprocess_box( ::pid_t pid );
			
			coprocess* get() const  { return its_coprocess; }
	};
	
	inline void swap( coprocess_box& a, coprocess_box& b )
	{
		a.swap( b );
	}
	
}

#endif
