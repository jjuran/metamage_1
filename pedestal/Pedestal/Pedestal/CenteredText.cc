/*
	CenteredText.cc
	---------------
*/

#include "Pedestal/CenteredText.hh"

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

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_TOOLBOX_UTILITIES_THROWOSSTATUS_HH
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"
#endif


#ifndef __MAC_10_11

namespace nucleus
{
	
	template <> struct disposer< ATSUStyle >
	{
		typedef ATSUStyle  argument_type;
		typedef void       result_type;
		
		void operator()( ATSUStyle style ) const
		{
			::ATSUDisposeStyle( style );
		}
	};
	
}

namespace Nitrogen
{
	
	static
	nucleus::owned< ATSUStyle > ATSUCreateStyle()
	{
		ATSUStyle result;
		
		Mac::ThrowOSStatus( ::ATSUCreateStyle( &result ) );
		
		return nucleus::owned< ATSUStyle >::seize( result );
	}
	
	static
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
		
		Mac::ThrowOSStatus( err );
		
		return result;
	}
	
	static inline
	ATSUFontID ATSUFindFontFromName( const char* name )
	{
		return ATSUFindFontFromName( name, strlen( name ) );
	}
	
}

#endif  // #ifndef __MAC_10_11

namespace Pedestal
{
	
#ifndef __MAC_10_11
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	static
	n::owned< ATSUStyle > ATSUFontAndSize( ATSUFontID font, UInt32 size )
	{
		n::owned< ATSUStyle > style = N::ATSUCreateStyle();
		
		size <<= 16;  // integer to Fixed
		
		const ATSUAttributeTag       tags  [] = { kATSUFontTag, kATSUSizeTag };
		const ByteCount              sizes [] = { sizeof font,  sizeof size  };
		const ATSUAttributeValuePtr  values[] = { &font,        &size        };
		
		ATSUSetAttributes( style, 2, tags, sizes, values );
		
		return style;
	}
	
#endif
	
	void DrawCenteredText( CFStringRef  text,
	                       Rect         bounds,
	                       const char*  fontName,
	                       UInt32       fontSize )
	{
	#ifndef __MAC_10_11
		
		ATSUFontID font = N::ATSUFindFontFromName( fontName );
		
		n::owned< ATSUStyle > style = ATSUFontAndSize( font, fontSize );
		
	#else
		
		ATSUStyle style = NULL;
		
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
	}
	
}
