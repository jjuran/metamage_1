/*
	Mac/TextEdit/Types/TEHandle.hh
	------------------------------
*/

#ifndef MAC_TEXTEDIT_TYPES_TEHANDLE_HH
#define MAC_TEXTEDIT_TYPES_TEHANDLE_HH

// Mac OS
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

// nucleus
#include "nucleus/disposer.hh"


namespace nucleus
{
	
	template <> struct disposer< TEHandle >
	{
		typedef TEHandle  argument_type;
		typedef void      result_type;
		
		void operator()( TEHandle hTE ) const
		{
			::TEDispose( hTE );
		}
	};
	
}

#endif

