/*
	GeneratePreviewForURL.cc
	------------------------
*/

#include <QuickLook/QuickLook.h>

// QLSKIF
#include "CGImageFromURL.hh"


extern "C"
OSStatus GeneratePreviewForURL( void*                interface,
                                QLPreviewRequestRef  request,
                                CFURLRef             url,
                                CFStringRef          contentTypeUTI,
                                CFDictionaryRef      options)
{
	CGImageRef image = CGSKIFCreateImageFromURL( url );
	
	size_t width  = CGImageGetWidth ( image );
	size_t height = CGImageGetHeight( image );
	
	CGSize size = { width, height };
	CGRect rect = CGRectMake( 0, 0, width, height );
	
	CFDictionaryRef properties = NULL;
	
	CGContextRef context = QLPreviewRequestCreateContext( request,
	                                                      size,
	                                                      true,
	                                                      properties );
	
	CGContextDrawImage( context, rect, image );
	
	QLPreviewRequestFlushContext( request, context );
	CGContextRelease( context );
	
	CGImageRelease( image );
	
	return noErr;
}

extern "C"
void CancelPreviewGeneration( void* interface, QLPreviewRequestRef request )
{
}
