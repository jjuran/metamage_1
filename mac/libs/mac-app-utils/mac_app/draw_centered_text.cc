/*
	draw_centered_text.cc
	---------------------
*/

#include "mac_app/draw_centered_text.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#else
// Universal Interfaces doesn't define these.
enum
{
	kFontNoNameCode     = 0,
	kFontNoPlatformCode = 0,
	kFontNoScriptCode   = 0,
	kFontNoLanguageCode = 0,
};
#endif

// Mac OS
#ifndef __MACTEXTEDITOR__
#include <MacTextEditor.h>
#endif

// Standard C
#include <string.h>


#ifndef MAC_OS_X_VERSION_10_5

static inline
ATSUFontID ATSUFindFontFromName( const char* name, size_t len )
{
	ATSUFontID result;
	
	OSStatus err;
	err = ::ATSUFindFontFromName( const_cast< char* >( name ),
	                              len,
	                              kFontNoNameCode,
	                              kFontNoPlatformCode,
	                              kFontNoScriptCode,
	                              kFontNoLanguageCode,
	                              &result );
	
	return err ? 0 : result;
}

static inline
ATSUFontID ATSUFindFontFromName( const char* name )
{
	return ATSUFindFontFromName( name, strlen( name ) );
}

static
void set_font_and_size( ATSUStyle style, ATSUFontID font, UInt32 size )
{
	size <<= 16;  // integer to Fixed
	
	const ATSUAttributeTag       tags  [] = { kATSUFontTag, kATSUSizeTag };
	const ByteCount              sizes [] = { sizeof font,  sizeof size  };
	const ATSUAttributeValuePtr  values[] = { &font,        &size        };
	
	ATSUSetAttributes( style, 2, tags, sizes, values );
}

#endif  // #ifndef MAC_OS_X_VERSION_10_5

namespace mac {
namespace app {
	
	void draw_centered_text( CFStringRef  text,
	                         Rect         bounds,
	                         const char*  fontName,
	                         UInt32       fontSize )
	{
		ATSUStyle style = NULL;
		
	#if ! __LP64__
		
	#ifndef MAC_OS_X_VERSION_10_5
		
		ATSUFontID font = ATSUFindFontFromName( fontName );
		
		if ( font  &&  ATSUCreateStyle( &style ) == noErr )
		{
			set_font_and_size( style, font, fontSize );
		}
		
	#endif
		
		TXNTextBoxOptionsData options =
		{
			kTXNSetFlushnessMask,
			kATSUCenterAlignment,
		};
		
		TXNDrawCFStringTextBox( text,
		                        &bounds,
		                        style,
		                        &options );
		
	#ifndef MAC_OS_X_VERSION_10_5
		
		if ( style )
		{
			ATSUDisposeStyle( style );
		}
		
	#endif
		
	#endif  // #if ! __LP64__
	}
	
}
}
