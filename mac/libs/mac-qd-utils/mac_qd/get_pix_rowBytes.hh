/*
	get_pix_rowBytes.hh
	-------------------
*/

#ifndef MACQD_GETPIXROWBYTES_HH
#define MACQD_GETPIXROWBYTES_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef MAC_OS_X_VERSION_10_7
#ifndef __QDOFFSCREEN__
#include <QDOffscreen.h>
#endif
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QDOFFSCREEN_H
#include "missing/QDOffscreen.h"
#endif
#endif


namespace mac {
namespace qd  {
	
#if TARGET_API_MAC_CARBON
	
	inline
	long get_pix_rowBytes( PixMapHandle pix )
	{
		return GetPixRowBytes( pix );
	}
	
#else
	
	inline
	short get_pix_rowBytes( PixMapHandle pix )
	{
		return pix[0]->rowBytes & 0x3FFF;
	}
	
#endif
	
}
}

#endif
