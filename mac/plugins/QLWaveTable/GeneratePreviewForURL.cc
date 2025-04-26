/*
	GeneratePreviewForURL.cc
	------------------------
*/

// qlgenerator
#include "Preview.h"

// QLSKIF
#include "CGImageFromURL.hh"


OSStatus GeneratePreviewForURL( void*                interface,
                                QLPreviewRequestRef  request,
                                CFURLRef             url,
                                CFStringRef          contentTypeUTI,
                                CFDictionaryRef      options)
{
	CGImageRef image = CGImageCreateFromURL( url );
	
	size_t width  = CGImageGetWidth ( image );
	size_t height = CGImageGetHeight( image );
	
	CGSize size = { width, height };
	CGRect rect = CGRectMake( 0, 0, width, height );
	
	CFDictionaryRef properties = NULL;
	
	CGContextRef context = QLPreviewRequestCreateContext( request,
	                                                      size,
	                                                      false,
	                                                      properties );
	
	CGContextDrawImage( context, rect, image );
	
	QLPreviewRequestFlushContext( request, context );
	CGContextRelease( context );
	
	CGImageRelease( image );
	
	return noErr;
}

void CancelPreviewGeneration( void* interface, QLPreviewRequestRef request )
{
}
