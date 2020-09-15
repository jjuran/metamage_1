/*
	Mac/TextEdit/Types/TEHandle.hh
	------------------------------
*/

#ifndef MAC_TEXTEDIT_TYPES_TEHANDLE_HH
#define MAC_TEXTEDIT_TYPES_TEHANDLE_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

// nucleus
#ifndef NUCLEUS_DISPOSER_HH
#include "nucleus/disposer.hh"
#endif


namespace nucleus
{
	
#if ! __LP64__
	
	template <> struct disposer< TEHandle >
	{
		typedef TEHandle  argument_type;
		typedef void      result_type;
		
		void operator()( TEHandle hTE ) const
		{
			::TEDispose( hTE );
		}
	};
	
#endif  // #if ! __LP64__
	
}

#endif
