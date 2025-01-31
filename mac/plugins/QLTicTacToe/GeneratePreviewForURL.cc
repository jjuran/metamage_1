/*
	GeneratePreviewForURL.cc
	------------------------
*/

// qlgenerator
#include "Preview.h"

// QLTicTacToe
#include "DrawGameFromURLIntoContext.hh"


OSStatus GeneratePreviewForURL( void*                interface,
                                QLPreviewRequestRef  request,
                                CFURLRef             url,
                                CFStringRef          contentTypeUTI,
                                CFDictionaryRef      options)
{
	CGFloat width  = 512;
	CGFloat height = 512;
	
	CGSize size = { width, height };
	
	CFDictionaryRef properties = NULL;
	
	CGContextRef context = QLPreviewRequestCreateContext( request,
	                                                      size,
	                                                      false,
	                                                      properties );
	
	DrawGameFromURLIntoContext( url, context, height );
	
	QLPreviewRequestFlushContext( request, context );
	CGContextRelease( context );
	
	return noErr;
}

void CancelPreviewGeneration( void* interface, QLPreviewRequestRef request )
{
}
