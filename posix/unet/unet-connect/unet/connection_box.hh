/*
	connection_box.hh
	-----------------
*/

#ifndef UNET_CONNECTIONBOX_HH
#define UNET_CONNECTIONBOX_HH


namespace unet
{
	
	class connection;
	
	class connection_box
	{
		private:
			connection* its_connection;
		
		public:
			connection_box() : its_connection()
			{
			}
			
			~connection_box();
			
			connection_box( const connection_box& that );
			
			connection_box& operator=( const connection_box& that );
			
			void swap( connection_box& that );
			
			connection_box( const char* argv[], int in, int out );
			
			connection* get() const  { return its_connection; }
			
			void check() const;
			
			connection& deref() const  { check();  return *get(); }
			
			int get_input () const;
			int get_output() const;
	};
	
	inline void swap( connection_box& a, connection_box& b )
	{
		a.swap( b );
	}
	
	connection_box connect( const char* argv[] );
	
}

#endif
