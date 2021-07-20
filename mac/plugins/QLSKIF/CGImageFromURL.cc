/*
	CGImageFromURL.cc
	-----------------
*/

#include "CGImageFromURL.hh"

// rasterlib
#include "raster/load.hh"

// CGSKIF
#include "CGSKIF.hh"


using namespace raster;


static
CGImageRef CGSKIFCreateImageFromPath( const char* path )
{
	int raster_fd = open( path, O_RDONLY );
	
	if ( raster_fd < 0 )
	{
		return NULL;
	}
	
	using namespace raster;
	
	raster_load loaded_raster = load_raster( raster_fd );
	
	if ( loaded_raster.addr == NULL )
	{
		return NULL;
	}
	
	close( raster_fd );
	
	CGImageRef image = CGSKIFCreateImageFromRaster( loaded_raster );
	
	close_raster( loaded_raster );
	
	return image;
}

CGImageRef CGSKIFCreateImageFromURL( CFURLRef url )
{
	const CFStringEncoding encoding = kCFStringEncodingUTF8;
	
	CFStringRef string = CFURLCopyFileSystemPath( url, kCFURLPOSIXPathStyle );
	
	const char* path = CFStringGetCStringPtr( string, encoding );
	
	char buffer[ PATH_MAX ];
	
	if ( path == NULL )
	{
		if ( ! CFStringGetCString( string, buffer, sizeof buffer, encoding ) )
		{
			return NULL;
		}
		
		path = buffer;
	}
	
	CGImageRef image = CGSKIFCreateImageFromPath( path );
	
	CFRelease( string );
	
	return image;
}
