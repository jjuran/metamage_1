/*	============
 *	Clipboard.hh
 *	============
 */

#ifndef PEDESTAL_CLIPBOARD_HH
#define PEDESTAL_CLIPBOARD_HH

#if PRAGMA_ONCE
#pragma once
#endif

// Mac OS
#include <TextEdit.h>


namespace Pedestal
{
	
	struct Clipboard
	{
		Clipboard();
		~Clipboard();
		
		static void Suspend();
		static void Resume ();
		
		static void TECut  ( TEHandle hTE );
		static void TECopy ( TEHandle hTE );
		static void TEPaste( TEHandle hTE );
	};
	
}

#endif

