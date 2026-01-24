/*
	FSGetCatalogInfo.hh
	-------------------
*/

#ifndef MACFILE_FSGETCATALOGINFO_HH
#define MACFILE_FSGETCATALOGINFO_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif


namespace mac  {
namespace file {
	
	OSErr Str63_from_HFSUniStr255( Str63 out, const HFSUniStr255& in );
	
#if TARGET_API_MAC_CARBON
	
	OSErr FSGetCatalogInfo_spec( const FSRef*         ref,
	                             FSCatalogInfoBitmap  bits,
	                             FSCatalogInfo*       info,
	                             HFSUniStr255*        name,
	                             FSSpec*              spec,
	                             FSRef*               parent );
	
#else
	
	inline
	OSErr FSGetCatalogInfo_spec( const FSRef*         ref,
	                             FSCatalogInfoBitmap  bits,
	                             FSCatalogInfo*       info,
	                             HFSUniStr255*        name,
	                             FSSpec*              spec,
	                             FSRef*               parent )
	{
		return ::FSGetCatalogInfo( ref, bits, info, name, spec, parent );
	}
	
#endif
	
	inline
	OSErr FSGetCatalogInfo_spec( const FSRef& ref, FSSpec& spec )
	{
		return FSGetCatalogInfo_spec( &ref, 0, 0, 0, &spec, 0 );
	}
	
}
}

#endif
