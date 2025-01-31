/*
	Thumbnail.h
	-----------
*/

#pragma once

// Mac OS X
#include <CoreFoundation/CoreFoundation.h>
#include <QuickLook/QuickLook.h>


#ifdef __cplusplus
extern "C" {
#endif

OSStatus
GenerateThumbnailForURL( void*                  interface,
                         QLThumbnailRequestRef  request,
                         CFURLRef               url,
                         CFStringRef            contentTypeUTI,
                         CFDictionaryRef        options,
                         CGSize                 maxSize );

void
CancelThumbnailGeneration( void* interface, QLThumbnailRequestRef request );

#ifdef __cplusplus
}
#endif
