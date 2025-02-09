/*
	GenerateThumbnailForURL.cc
	--------------------------
*/

// qlgenerator
#include "Thumbnail.h"

// QLSKIF
#include "CGImageFromURL.hh"


OSStatus GenerateThumbnailForURL( void*                  interface,
                                  QLThumbnailRequestRef  request,
                                  CFURLRef               url,
                                  CFStringRef            contentTypeUTI,
                                  CFDictionaryRef        options,
                                  CGSize                 maxSize )
{
	CGImageRef image = CGImageCreateFromURL( url );
	
	CFDictionaryRef properties = NULL;
	
	QLThumbnailRequestSetImage( request, image, properties );
	
	CGImageRelease( image );
	
	return noErr;
}

void CancelThumbnailGeneration( void* interface, QLThumbnailRequestRef request )
{
}
