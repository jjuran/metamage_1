/*
	qdscreencap.cc
	--------------
*/

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <stdlib.h>

// mac-qd-utils
#include "mac_qd/is_monochrome.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// raster
#include "raster/raster.hh"
#include "raster/skif.hh"


#define PROGRAM  "qdscreencap"

#define MISSING_OUTPUT  "stdout is a tty, either redirect or use -o"
#define NULL_BASE_ADDR  "null base address, aborting"
#define GRAYSCALE_ONLY  "non-grayscale palettes aren't fully supported yet"

#define STR_LEN( s )  "" s, (sizeof s - 1)


using mac::qd::is_monochrome;
using mac::sys::gestalt;

using namespace raster;


struct raster_info
{
	raster_desc  desc;
	void*        baseAddr;
};

#if TARGET_CPU_68K  &&  !__MC68020__
	const bool definitely_not_a_68000 = false;
#else
	const bool definitely_not_a_68000 = true;
#endif

static inline
bool has_Color_QuickDraw()
{
	if ( definitely_not_a_68000 )
	{
		return true;
	}
	
	enum
	{
		gestaltQuickdrawVersion = 'qd  ',
		gestaltOriginalQD       = 0x0000,
	};
	
	return mac::sys::gestalt( gestaltQuickdrawVersion ) != gestaltOriginalQD;
}

static
PixMapHandle get_main_pixmap()
{
#ifndef MAC_OS_X_VERSION_10_7
	
	GDHandle gdh = GetMainDevice();
	
	PixMapHandle pmh = gdh[0]->gdPMap;
	
	const PixMap& pm = **pmh;
	
	if ( ! TARGET_RT_MAC_MACHO  ||  TARGET_CPU_PPC  ||  pm.baseAddr != NULL )
	{
		return pmh;
	}
	
#endif
	
	return NULL;
}

static
raster_info classic_Macintosh_screen_info()
{
	raster_info info =
	{
		{
			kSKIFFileType,
			0,
			512,
			342,
			64,
			1,
		},
		
	#if TARGET_CPU_68K
		
		LMGetScrnBase(),
		
	#endif
	};
	
	return info;
}

static
raster_info desktop_raster_info()
{
	if ( ! has_Color_QuickDraw() )
	{
		return classic_Macintosh_screen_info();
	}
	
	PixMapHandle pmh = get_main_pixmap();
	
	if ( pmh == NULL )
	{
		write( STDERR_FILENO, STR_LEN( PROGRAM ": " NULL_BASE_ADDR "\n" ) );
		
		exit( 10 );
	}
	
	raster_model model;
	pixel_layout layout = {};
	
	const PixMap& pm = **pmh;
	
	if ( pm.pixelType != 0 )  // chunky
	{
		model = Model_RGB;  // 16-bit or 32-bit
		
		if ( pm.pixelSize == 16 )
		{
			layout.red   = 0x5a;
			layout.green = 0x55;
			layout.blue  = 0x50;
		}
		else
		{
			layout.per_pixel = raster::xRGB;
		}
	}
	else if ( is_monochrome( pm ) )
	{
		model = Model_monochrome_paint;
	}
	else
	{
		model = Model_palette;
		
		write( STDERR_FILENO, STR_LEN( PROGRAM ": " GRAYSCALE_ONLY "\n" ) );
	}
	
	const short width  = pm.bounds.right - pm.bounds.left;
	const short height = pm.bounds.bottom - pm.bounds.top;
	const short stride = pm.rowBytes & 0x3FFF;
	const short weight = pm.pixelSize;
	
	const uint32_t image_size = height * stride;
	
	raster_info info =
	{
		{
			raster::kSKIFFileType,
			0,
			width,
			height,
			stride,
			weight,
			model,
			0,
			0,
			layout,
		},
		pm.baseAddr,
	};
	
	return info;
}

static
void save_desktop_screenshot( const char* path )
{
	raster_info  info = desktop_raster_info();
	raster_desc& desc = info.desc;
	
	const uint32_t image_size = desc.height * desc.stride;
	
	int fd = open( path, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
	
	ssize_t n_written = write( fd, info.baseAddr, image_size );
	
	n_written = write( fd, &desc, sizeof desc );
	
	uint32_t footer_size = sizeof desc + sizeof (uint32_t) * 2;
	
	const uint32_t disk_block_size = TARGET_API_MAC_CARBON ? 4096 : 512;
	const uint32_t k               = disk_block_size - 1;
	
	// Round up the total file size to a multiple of the disk block size.
	const uint32_t total_size = (image_size + footer_size + k) & ~k;
	
	footer_size = total_size - image_size;
	
	const off_t footer_addr = total_size - sizeof footer_size;
	
	const size_t code_len = sizeof (uint32_t);
	
	n_written = pwrite( fd, "SKIF", code_len, footer_addr - code_len );
	n_written = pwrite( fd, &footer_size, sizeof footer_size, footer_addr );
	
	close( fd );
}

int main( int argc, char** argv )
{
	const char* filename = "/dev/fd/1";
	
	if ( argc >= 3  &&  strcmp( argv[ 1 ], "-o" ) == 0 )
	{
		filename = argv[ 2 ];
	}
	else if ( isatty( STDOUT_FILENO ) )
	{
		write( STDERR_FILENO, STR_LEN( PROGRAM ": " MISSING_OUTPUT "\n" ) );
		return 2;
	}
	
	save_desktop_screenshot( filename );
	
	return 0;
}
