/*
	Preview.h
	---------
*/

#pragma once

// Mac OS X
#include <CoreFoundation/CoreFoundation.h>
#include <QuickLook/QuickLook.h>


#ifdef __cplusplus
extern "C" {
#endif

OSStatus
GeneratePreviewForURL( void*                interface,
                       QLPreviewRequestRef  request,
                       CFURLRef             url,
                       CFStringRef          contentTypeUTI,
                       CFDictionaryRef      options);

void
CancelPreviewGeneration( void* interface, QLPreviewRequestRef request );

#ifdef __cplusplus
}
#endif
