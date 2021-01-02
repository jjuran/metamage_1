/*
	fb.hh
	-----
*/

#ifndef FB_HH
#define FB_HH

// Linux
#include <linux/fb.h>


namespace fb
{
	
	class handle
	{
		private:
			int fd;
			
			// non-copyable
			handle           ( const handle& );
			handle& operator=( const handle& );
		
		public:
			handle( const char* path );
			
			~handle();
			
			int get_fd() const  { return fd; }
	};
	
	
	fb_fix_screeninfo get_fix_screeninfo( const handle& h );
	fb_var_screeninfo get_var_screeninfo( const handle& h );
	
}

#endif
