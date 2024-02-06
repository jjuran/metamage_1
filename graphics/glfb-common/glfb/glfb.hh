/*
	glfb.hh
	-------
*/

#ifndef GLFB_GLFB_HH
#define GLFB_GLFB_HH


namespace glfb
{
	
	extern bool overlay_enabled;
	
	extern bool cursor_enabled;
	extern bool cursor_visible;
	
	// Logical location of cursor on screen
	extern int cursor_x;
	extern int cursor_y;
	
	// Delta between top-left of cursor image and hotspot
	extern int hotspot_dx;
	extern int hotspot_dy;
	
	void initialize();
	
	void set_dimensions( int width, int height, int depth = 1 );
	
	void set_screen_image( const void* src_addr );
	void set_cursor_image( const void* src_addr );
	
	inline
	void set_cursor_hotspot( int dx, int dy )
	{
		hotspot_dx = dx;
		hotspot_dy = dy;
	}
	
	inline
	void set_cursor_location( int x, int y )
	{
		cursor_x = x;
		cursor_y = y;
	}
	
	inline
	void set_cursor_visibility( bool visible )
	{
		cursor_visible = visible;
	}
	
	void render();
	
	void terminate();
	
}

#endif
