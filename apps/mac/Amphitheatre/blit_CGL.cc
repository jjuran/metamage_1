/*
	blit_CGL.cc
	-----------
*/

#include "blit_CGL.hh"

// Mac OS X
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#endif

// OpenGL
#include <OpenGL/gl.h>

// transcodex
#include "transcode/8x_1bpp_to_8bpp.hh"

// Amphitheatre
#include "bitmapped_texture_check.hh"


static int image_width;
static int image_height;

static int tex_width;
static int tex_height;

#ifdef GL_TEXTURE_RECTANGLE_ARB

const GLenum texture_target = GL_TEXTURE_RECTANGLE_ARB;

#define glTexCoord_ glTexCoord2i

static int tex_x1;
static int tex_y1;

#else

const GLenum texture_target = GL_TEXTURE_2D;

#define glTexCoord_ glTexCoord2f

static float tex_x1;
static float tex_y1;

#endif

static bool bitmaps_make_textures;

static GLenum texture_format = GL_LUMINANCE;
static GLenum texture_type   = GL_UNSIGNED_BYTE;

static uint8_t* texture_data;

static CGLContextObj context;
static GLuint        texture;

static inline
unsigned binary_power_up( unsigned x )
{
	--x;
	
	int power = 1;
	
	while ( x >>= 1 )
	{
		++power;
	}
	
	return 1 << power;
}

CGL_blitter::CGL_blitter( CGDirectDisplayID id, CGRect bounds )
{
	CGLError err;
	
	CGOpenGLDisplayMask display_mask = CGDisplayIDToOpenGLDisplayMask( id );
	
	const CGLPixelFormatAttribute attrs[] =
	{
	#ifndef MAC_OS_X_VERSION_10_7
		// Without this, CGLSetFullScreen() returns 10004 in 10.6.
		kCGLPFAFullScreen,
	#endif
		kCGLPFADoubleBuffer,
		kCGLPFADisplayMask,
		(CGLPixelFormatAttribute) display_mask,
		(CGLPixelFormatAttribute) 0  // "NULL" terminator
	};
	
	CGLPixelFormatObj format;
	GLint             nvirt;
	
	if ( (err = CGLChoosePixelFormat( attrs, &format, &nvirt )) )
	{
		return;
	}
	
	err = CGLCreateContext( format, 0, &context );  // NULL
	
	CGLDestroyPixelFormat( format );
	
	if ( err )
	{
		return;
	}
	
#ifdef MAC_OS_X_VERSION_10_7
	
	/*
		According to the Internet, 10.5 lacks CGLSetFullScreenOnDisplay(),
		even though Apple's headers claim otherwise.
		
		In addition, CGLSetFullScreenOnDisplay() is returning 10012 in 10.6.
	*/
	
	err = CGLSetFullScreenOnDisplay( context, display_mask );
	
#else
	
	err = CGLSetFullScreen( context );
	
#endif
	
	if ( err )
	{
		return;
	}
	
	CGLSetCurrentContext( context );
	
#ifdef GL_TEXTURE_RECTANGLE_ARB
	
	/*
		Don't even check for bitmap support if we don't have rectangle
		textures.  It's always supported with 2D textures, but not in the
		older OpenGL hardware -- and falling back to software rendering
		kills performance.
		
		It's true that has_bitmapped_textures() returns false in this case,
		but the extra check here lets us omit some dead code.
	*/
	
	if ( (bitmaps_make_textures = has_bitmapped_textures()) )
	{
		texture_format = GL_COLOR_INDEX;
		texture_type   = GL_BITMAP;
		
		const float palette[] = { 1.0, 0.0 };  // white = 0, black = 1
		
		glPixelMapfv( GL_PIXEL_MAP_I_TO_R, 2, palette );
		
		/*
			GL_UNPACK_ALIGNMENT must be a factor of the stride, which is also
			known as rowBytes.  In practice, screens tend to be a multiple of
			128px wide, which at 1bpp is 16 bytes -- more than enough to cover
			OpenGL's default alignment of 4 bytes or even its max of 8 bytes.
			(An exception is 800px (rowBytes of 100), which is only a multiple
			of 4 bytes.)
			
			Note that color graphics devices in classic Mac OS tend to have
			extra, unused words at the end of each row.  This is fine when
			you have qd.screenBits.rowBytes, but OpenGL doesn't have a means
			to skip words that aren't alignment gaps.  Care must be taken when
			creating raster files to avoid creating a "gutter" that poses a
			nuisance for OpenGL, and ideally, choose an image width with no
			wasted bytes that works regardless of alignment.
			
			To ensure compatibility with QuickDraw, the alignment must be at
			least 2 bytes.  For example, an image width of 520px should have
			a rowBytes of 66, not 65, and you'd have to call
			
				glPixelStorei( GL_UNPACK_ALIGNMENT, 2 );
			
			for correct unpacking in OpenGL.
		*/
	}
	
#endif
	
	glGenTextures( 1, &texture );
	
	glBindTexture( texture_target, texture );
	
	/*
		Leave GL_TEXTURE_MAG_FILTER set to its default of GL_LINEAR for now.
		Perhaps in the future we'll have a switch to use GL_NEAREST instead.
		
		We don't have any cases of minification currently, so it doesn't
		matter what GL_TEXTURE_MIN_FILTER is set to.
	*/
	
#ifndef GL_TEXTURE_RECTANGLE_ARB
	
	// Leave GL_TEXTURE_BASE_LEVEL set to its default of zero.
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
	
#endif
	
	glViewport( (int) bounds.origin.x,
	            (int) bounds.origin.y,
	            (int) bounds.size.width,
	            (int) bounds.size.height );
}

CGL_blitter::~CGL_blitter()
{
	// Clear and flush twice to ensure we get a black screen before exit.
	
	glClear( GL_COLOR_BUFFER_BIT );
	CGLFlushDrawable( context );
	
	glClear( GL_COLOR_BUFFER_BIT );
	CGLFlushDrawable( context );
	
	free( texture_data );
	
	CGLClearDrawable( context );
	CGLDestroyContext( context );
}

void CGL_blitter::prep( int stride, int width, int height )
{
	glMatrixMode( GL_PROJECTION );
	glOrtho( 0, width, 0, height, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	
	image_width  = width;
	image_height = height;
	
#ifdef GL_TEXTURE_RECTANGLE_ARB
	
	tex_width  = image_width;
	tex_height = image_height;
	
	tex_x1 = image_width;
	tex_y1 = image_height;
	
#else
	
	int greater = width > height ? width : height;
	
	tex_width  =
	tex_height = binary_power_up( greater );
	
	tex_x1 = 1.0 * image_width  / tex_width;
	tex_y1 = 1.0 * image_height / tex_height;
	
#endif
	
	glTexImage2D( texture_target,
	              0,
	              GL_LUMINANCE,
	              tex_width,
	              tex_height,
	              0,
	              texture_format,
	              texture_type,
	              0 );  // NULL
	
	if ( ! bitmaps_make_textures )
	{
		texture_data = (uint8_t*) malloc( tex_width * tex_height );
	}
}

void CGL_blitter::blit( const void* src_addr )
{
	if ( ! bitmaps_make_textures )
	{
		const int n_octets = image_width * image_height / 8u;
		
		transcode_8x_1bpp_to_8bpp( src_addr, texture_data, n_octets );
		
		src_addr = texture_data;
	}
	
	glBindTexture( texture_target, texture );
	
	glTexSubImage2D( texture_target,
	                 0,
	                 0,
	                 0,
	                 image_width,
	                 image_height,
	                 texture_format,
	                 texture_type,
	                 src_addr );
	
	glClear( GL_COLOR_BUFFER_BIT );
	
	glBindTexture( texture_target, texture );
	glEnable( texture_target );
	glBegin( GL_QUADS );
	
	const int width  = image_width;
	const int height = image_height;
	
	glTexCoord_( 0,      0      ); glVertex2i( 0,     height );  // top left
	glTexCoord_( tex_x1, 0      ); glVertex2i( width, height );  // top right
	glTexCoord_( tex_x1, tex_y1 ); glVertex2i( width, 0      );  // bottom right
	glTexCoord_( 0,      tex_y1 ); glVertex2i( 0,     0      );  // bottom left
	
	glEnd();
	glDisable( texture_target );
	glBindTexture( texture_target, 0 );
	
	CGLFlushDrawable( context );  // implicitly calls glFlush()
}
