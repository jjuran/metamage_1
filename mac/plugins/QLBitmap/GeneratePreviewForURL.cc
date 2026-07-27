/*
	GeneratePreviewForURL.cc
	------------------------
*/

// qlgenerator
#include "Preview.h"

// QLBitmap
#include "CreateImage.hh"


OSStatus GeneratePreviewForURL( void*                interface,
                                QLPreviewRequestRef  request,
                                CFURLRef             url,
                                CFStringRef          contentTypeUTI,
                                CFDictionaryRef      options)
{
	CGImageRef image = CreateImageFromURL( url );
	
	size_t width  = CGImageGetWidth ( image );
	size_t height = CGImageGetHeight( image );
	
	width  *= 4;
	height *= 4;
	
	CGSize size = { width, height };
	CGRect rect = CGRectMake( 0, 0, width, height );
	
	CFDictionaryRef properties = NULL;
	
	CGContextRef context = QLPreviewRequestCreateContext( request,
	                                                      size,
	                                                      true,
	                                                      properties );
	
	CGContextClearRect( context, rect );
	
	CGContextDrawImage( context, rect, image );
	
	QLPreviewRequestFlushContext( request, context );
	CGContextRelease( context );
	
	CGImageRelease( image );
	
	return noErr;
}

void CancelPreviewGeneration( void* interface, QLPreviewRequestRef request )
{
}
