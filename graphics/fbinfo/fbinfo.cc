/*
	fbinfo.cc
	---------
*/

// Android
#ifdef __ANDROID__
#include <sys/system_properties.h>
#endif

// Standard C
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// fbinfo
#include "fb.hh"


#define PROGRAM  "fbinfo"

#ifdef ANDROID
#define DEFAULT_FB_PATH  "/dev/graphics/fb0"
#else
#define DEFAULT_FB_PATH  "/dev/fb0"
#endif


static
void print_fb_bitfield( const char* label,
                        const char* space,
                        const fb_bitfield& bf )
{
	if ( bf.length )
	{
		fprintf( stderr,
		         "%s:%s offset = %c%c, length = %u, msb_right = %u\n",
		         label,
		         space,
		         bf.offset > 9 ? bf.offset / 10 + '0' : ' ',
		         bf.offset % 10 + '0',
		         bf.length,
		         bf.msb_right );
	}
}

static inline
bool is_rgba( const fb_var_screeninfo& var_info )
{
	return var_info.red.offset == 24;
}

static
void print_fbdev_info( const char* path )
{
	fb::handle fbh( path );
	
	fb_var_screeninfo var = get_var_screeninfo( fbh );
	fb_fix_screeninfo fix = get_fix_screeninfo( fbh );
	
	const unsigned width  = var.xres;
	const unsigned height = var.yres;
	const unsigned stride = fix.line_length;
	const unsigned weight = var.bits_per_pixel;
	
	const unsigned vwidth  = var.xres_virtual;
	const unsigned vheight = var.yres_virtual;
	const unsigned xoffset = var.xoffset;
	const unsigned yoffset = var.yoffset;
	
	fprintf( stderr, "ID: %s\n", fix.id );
	fprintf( stderr, "type, visual = %d, %d\n", fix.type, fix.visual );
	
	fprintf( stderr, "%ux%u*%u (%u)\n", vwidth, vheight, weight, stride );
	
	if ( vwidth != width  ||  vheight != height )
	{
		fprintf( stderr, "%ux%u@%u,%u\n", width, height, xoffset, yoffset );
	}
	
	int xmm = var.width;
	int ymm = var.height;
	
	if ( xmm >= 0  &&  ymm >= 0 )
	{
		int xdpi = 254 * width  / xmm / 10;
		int ydpi = 254 * height / ymm / 10;
		
		fprintf( stderr, "%umm x %umm (%dx%d dpi)\n", xmm, ymm, xdpi, ydpi );
	}
	
	fprintf( stderr, "grayscale = %d\n", var.grayscale );
	
	print_fb_bitfield( "red", "   ", var.red    );
	print_fb_bitfield( "green", " ", var.green  );
	print_fb_bitfield( "blue", "  ", var.blue   );
	print_fb_bitfield( "transp", "", var.transp );
	
	if ( var.sync     ) fprintf( stderr, "sync: %u\n", var.sync );
	if ( var.vmode    ) fprintf( stderr, "vmode: %u\n", var.vmode );
	if ( var.rotate   ) fprintf( stderr, "rotate: %u\n", var.rotate );
	if ( var.nonstd   ) fprintf( stderr, "nonstd: %u\n", var.nonstd );
	if ( var.activate ) fprintf( stderr, "activate: %u\n", var.activate );
	
	bool rgba = is_rgba( var );
	
	if ( rgba )
	{
		fprintf( stderr, "RGBA detected via fb_var_screeninfo.\n" );
	}
	
#ifdef __ANDROID__
	
	/*
		An HP tablet running CyanogenMod (identifying as Android 4.3.1) reports
		var_info offsets of (red: 16, green: 8, blue: 0) -- i.e. it's claiming
		ARGB, but actually it's lying and is really RGBA (like all Android 4+,
		to my knowledge).
	*/
	
	if ( ! rgba )
	{
		char buffer[ PROP_VALUE_MAX + 1 ];
		int len = __system_property_get( "ro.build.version.release", buffer );
		
		fprintf( stderr, "ro.build.version.release: %s\n", buffer );
		
		if ( len >= 2  &&  (buffer[ 0 ] >= '4'  ||  buffer[ 1 ] != '.') )
		{
			rgba = true;
			fprintf( stderr, "RGBA detected via ro.build.version.release.\n" );
		}
	}
	
#endif
	
}

int main( int argc, char** argv )
{
	const char* filename = DEFAULT_FB_PATH;
	
	if ( argc >= 3  &&  strcmp( argv[ 1 ], "-d" ) == 0 )
	{
		filename = argv[ 2 ];
	}
	
	print_fbdev_info( filename );
	
	return 0;
}
