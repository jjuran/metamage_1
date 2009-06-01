/*	============
 *	Clipboard.hh
 *	============
 */

#ifndef PEDESTAL_CLIPBOARD_HH
#define PEDESTAL_CLIPBOARD_HH

// Mac OS
#include <TextEdit.h>


namespace Pedestal
{
	
	struct Clipboard
	{
		static void TECut  ( TEHandle hTE );
		static void TECopy ( TEHandle hTE );
		static void TEPaste( TEHandle hTE );
	};
	
}

#endif

