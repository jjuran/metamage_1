/*
	fb.hh
	-----
*/

#ifndef FB_HH
#define FB_HH

// Linux
#include <linux/fb.h>

// Standard C
#include <stddef.h>


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
			handle();
			
			~handle();
			
			int get_fd() const  { return fd; }
	};
	
	class map
	{
		private:
			void*   base;
			size_t  size;
			size_t  stride;
			
			// non-copyable
			map           ( const map& );
			map& operator=( const map& );
		
		public:
			map( const handle& h );
			
			~map();
			
			void*  get_base() const  { return base; }
			size_t get_size() const  { return size; }
			size_t get_stride() const  { return stride; }
	};
	
	
	fb_fix_screeninfo get_fix_screeninfo( const handle& h );
	fb_var_screeninfo get_var_screeninfo( const handle& h );
	
}

#endif
