/*	==========
 *	Console.cc
 *	==========
 */

#include "Pedestal/Console.hh"

// Mac OS Universal Headers
#include <Menus.h>

// Standard C++
#include <algorithm>
#include <string>

// Nitrogen
#include "Nitrogen/QuickDraw.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	using std::string;
	
	static Rect MakeWindowRect()
	{
		Rect screenBounds = N::GetQDGlobalsScreenBits().bounds;
		
		Rect rect = N::SetRect(0, 18, 2*4+6*80+16, 18+2*4+11*25+16);
		short mbarHeight = ::GetMBarHeight();
		short vMargin = (screenBounds.bottom - rect.bottom) - mbarHeight;
		short hMargin = (screenBounds.right  - rect.right);
		
		return N::OffsetRect(rect, hMargin / 2, mbarHeight + vMargin / 3);
	}
	
	int Console::WriteChars(const char* data, unsigned int byteCount)
	{
		if (byteCount == 0)  return 0;
		
		const char* end = data + byteCount;
		
		string text;
		
		if (data[0] == '\n')
		{
			if (!pendingCR)
			{
				text += "\r";
			}
			data++;
		}
		
		pendingCR = false;
		
		if (end[-1] == '\r')
		{
			pendingCR = true;
			end--;
		}
		
		while (true)
		{
			const char* lf = std::find(data, end, '\n');
			text += string(data, lf - data);
			if (lf == end)
			{
				break;
			}
			if (lf[-1] != '\r')
			{
				text += "\r";
			}
			data = lf + 1;
		}
		
		if (pendingCR)
		{
			text += "\r";
		}
		
		if (TextLength() + text.size() >= 32000)
		{
			SelectAll();
			Clear();
		}
		
		AppendChars(text.c_str(), text.size());
		return byteCount;
	}
	
}

