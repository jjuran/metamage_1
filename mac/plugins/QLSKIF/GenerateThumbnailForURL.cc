/*
	GenerateThumbnailForURL.cc
	--------------------------
*/

#include <QuickLook/QuickLook.h>

// QLSKIF
#include "CGImageFromURL.hh"


extern "C"
OSStatus GenerateThumbnailForURL( void*                  interface,
                                  QLThumbnailRequestRef  request,
                                  CFURLRef               url,
                                  CFStringRef            contentTypeUTI,
                                  CFDictionaryRef        options,
                                  CGSize                 maxSize )
{
	CGImageRef image = CGSKIFCreateImageFromURL( url );
	
	CFDictionaryRef properties = NULL;
	
	QLThumbnailRequestSetImage( request, image, properties );
	
	CGImageRelease( image );
	
	return noErr;
}

extern "C"
void CancelThumbnailGeneration( void* interface, QLThumbnailRequestRef request )
{
}
