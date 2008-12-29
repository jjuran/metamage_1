/*	==========
 *	Console.cc
 *	==========
 */

#include "Pedestal/Console.hh"

// Mac OS Universal Headers
#ifndef __MENUS__
#include <Menus.h>
#endif

// Standard C++
#include <algorithm>
#include <string>

// Nitrogen
#include "Nitrogen/QuickDraw.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	static Rect MakeWindowRect()
	{
		Rect screenBounds = N::GetQDGlobalsScreenBits().bounds;
		
		Rect rect = N::SetRect( 0,
		                        18,
		                        0   +  2 * 4  +   6 * 80  +  16,
		                        18  +  2 * 4  +  11 * 25  +  16 );
		
		short mbarHeight = ::GetMBarHeight();
		
		short vMargin = ( screenBounds.bottom - rect.bottom ) - mbarHeight;
		short hMargin = ( screenBounds.right  - rect.right  );
		
		return N::OffsetRect( rect, hMargin / 2, mbarHeight + vMargin / 3 );
	}
	
	
	int Console::WriteChars( const char* data, unsigned int byteCount )
	{
		if ( byteCount == 0 )  return 0;
		
		const char* end = data + byteCount;
		
		std::string text;
		
		if ( data[0] == '\n' )
		{
			if ( !itHasPendingCR )
			{
				text += "\r";
			}
			
			++data;
		}
		
		itHasPendingCR = false;
		
		if ( end[-1] == '\r' )
		{
			itHasPendingCR = true;
			
			--end;
		}
		
		while ( true )
		{
			const char* lf = std::find( data, end, '\n' );
			
			text += std::string( data, lf - data );
			
			if ( lf == end )
			{
				break;
			}
			
			if ( lf[-1] != '\r' )
			{
				text += "\r";
			}
			
			data = lf + 1;
		}
		
		if ( itHasPendingCR )
		{
			text += "\r";
		}
		
		TEHandle hTE = Get();
		
		TERec& te = **hTE;
		
		if ( te.teLength + text.size() >= 32000 )
		{
			te.selStart = 0;
			te.selEnd   = 0;
			
			te.teLength = 0;
			
			N::SetHandleSize( te.hText, 0 );
			
			N::TECalText( hTE );
		}
		
		AppendChars( text.c_str(), text.size(), true );
		
		return byteCount;
	}
	
}

