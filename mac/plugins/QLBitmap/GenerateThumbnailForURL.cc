/*
	GenerateThumbnailForURL.cc
	--------------------------
*/

// qlgenerator
#include "Thumbnail.h"

// QLBitmap
#include "CreateImage.hh"


OSStatus GenerateThumbnailForURL( void*                  interface,
                                  QLThumbnailRequestRef  request,
                                  CFURLRef               url,
                                  CFStringRef            contentTypeUTI,
                                  CFDictionaryRef        options,
                                  CGSize                 maxSize )
{
	CGImageRef image = CreateImageFromURL( url );
	
	CFDictionaryRef properties = NULL;
	
	QLThumbnailRequestSetImage( request, image, properties );
	
	CGImageRelease( image );
	
	return noErr;
}

void CancelThumbnailGeneration( void* interface, QLThumbnailRequestRef request )
{
}
