/*
	GenerateThumbnailForURL.cc
	--------------------------
*/

// qlgenerator
#include "Thumbnail.h"

// QLTicTacToe
#include "DrawGameFromURLIntoContext.hh"


OSStatus GenerateThumbnailForURL( void*                  interface,
                                  QLThumbnailRequestRef  request,
                                  CFURLRef               url,
                                  CFStringRef            contentTypeUTI,
                                  CFDictionaryRef        options,
                                  CGSize                 maxSize )
{
	CGSize size = maxSize;
	CGRect rect = { {0, 0}, size };
	
	size_t bitsPerComponent = 8;
	size_t bytesPerRow      = size.width;
	
	CFStringRef colorSpaceName = kCGColorSpaceGenericGray;
	CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName( colorSpaceName );
	
	CGContextRef context = CGBitmapContextCreate( NULL,
	                                              size.width,
	                                              size.height,
	                                              bitsPerComponent,
	                                              bytesPerRow,
	                                              colorSpace,
	                                              kCGImageAlphaNone );
	
	CGColorSpaceRelease( colorSpace );
	
	CGContextSetGrayFillColor( context, 1, 1 );  // white
	
	CGContextFillRect( context, rect );
	
	DrawGameFromURLIntoContext( url, context, size.height );
	
	CGImageRef image = CGBitmapContextCreateImage( context );
	
	CGContextRelease( context );
	
	CFDictionaryRef properties = NULL;
	
	QLThumbnailRequestSetImage( request, image, properties );
	
	CGImageRelease( image );
	
	return noErr;
}

void CancelThumbnailGeneration( void* interface, QLThumbnailRequestRef request )
{
}
