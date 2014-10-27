/*
	connection.hh
	-------------
*/

#ifndef UNET_CONNECTION_HH
#define UNET_CONNECTION_HH

// plus
#include "plus/ref_count.hh"


namespace unet
{
	
	class connection : public plus::ref_count< connection >
	{
		private:
			int its_input;
			int its_output;
			
		private:
			// non-copyable
			connection           ( const connection& );
			connection& operator=( const connection& );
		
		public:
			connection( int in, int out );
			
			~connection();
			
			int get_input () const  { return its_input;  }
			int get_output() const  { return its_output; }
	};
	
	inline connection::connection( int in, int out )
	:
		its_input ( in  ),
		its_output( out )
	{
	}
	
}

#endif
