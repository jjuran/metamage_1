/*
	FSGetCatalogInfo.cc
	-------------------
*/

#include "mac_file/FSGetCatalogInfo.hh"


#pragma exceptions off


namespace mac  {
namespace file {

#if ! __LP64__
#if TARGET_API_MAC_CARBON

OSErr FSGetCatalogInfo_spec( const FSRef*         ref,
                             FSCatalogInfoBitmap  bits,
                             FSCatalogInfo*       info,
                             HFSUniStr255*        name,
                             FSSpec*              spec,
                             FSRef*               parent )
{
	if ( spec == NULL )
	{
		return ::FSGetCatalogInfo( ref, bits, info, name, spec, parent );
	}
	
	OSErr err;
	
	HFSUniStr255 temp_name;
	
	if ( name == NULL )
	{
		name = &temp_name;
	}
	
	err = ::FSGetCatalogInfo( ref, bits, info, name, spec, parent );
	
	/*
		Since we're dealing only with characters in MacRoman,
		they're all in Unicode's Basic Multilingual Plane, so
		there won't be any surrogate pairs in the HFSUniStr255.
		
		Therefore, the UTF-16 name and the MacRoman name
		should have the same number of units.
	*/
	
	if ( err == noErr  &&  spec->name[ 0 ] > name->length )
	{
		/*
			Observed in Mac OS X 10.3 through at least 10.9:
			
			When a filename contains an em dash (U+2014), the FSSpec
			returned by FSGetCatalogInfo() has a corrupted name.
			
			In MacRoman, the em dash is $"d1", but in its place
			we're getting $"81 5c".  (In UTF-8, it's $"e2 80 94".)
			
			The other 127 non-ASCII MacRoman characters work just fine.
		*/
		
		CFStringRef s = CFStringCreateWithCharacters( NULL, name->unicode,
		                                                    name->length );
		
		err = memFullErr;
		
		if ( s )
		{
			/*
				Since we're shrinking the name to its proper length,
				there will be detritus in the unused bytes immediately
				following in the name field (one byte per em dash).
				These bytes will duplicate the end of the name that
				we're returning anyway, so no information is leaked.
				It's up to the caller to clear the excess if desired.
			*/
			
			Boolean got = CFStringGetPascalString( s,
			                                       spec->name,
			                                       sizeof spec->name,  // 64
			                                       kCFStringEncodingMacRoman );
			
			err = got ? noErr : bdNamErr;
			
			CFRelease( s );
		}
	}
	
	return err;
}

#endif  // #if TARGET_API_MAC_CARBON
#else

int dummy;

#endif  // #if ! __LP64__

}
}
