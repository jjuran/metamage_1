/*
	MacGlue/MacGlue.cc
	------------------
*/

#include "MacGlue/MacGlue.hh"

#if defined( __MC68K__ )  &&  !defined( __CFM68K__ )
#ifndef __TRAPS__
#include <Traps.h>
#endif
#endif


namespace MacGlue
{
	
#if defined( __MC68K__ )  &&  !defined( __CFM68K__ )
	
	enum { kAutoPop = 0x0400 };
	
	#define DEFINE_MAC_GLUE( routine )   \
		MACGLUE_PROTOTYPE( routine );    \
		MACGLUE_PROTOTYPE( routine )     \
		{                                \
			DC.W  _##routine | kAutoPop  \
		}
	
	#define DEFINE_MAC_GLUE_DISPATCHED( routine,        \
	                                    arg_bytes,      \
	                                    trap,           \
	                                    selector )      \
		MACGLUE_PROTOTYPE( routine );                   \
		MACGLUE_PROTOTYPE( routine )                    \
		{                                               \
			MOVE.L  #(arg_bytes << 16 | selector),D0 ;  \
			                                            \
			DC.W  trap | kAutoPop                       \
		}
	
#else
	
	#define DEFINE_MAC_GLUE( routine )   /**/
	
	#define DEFINE_MAC_GLUE_DISPATCHED( routine, arg_bytes, trap, selector )   /**/
	
#endif
	
	DEFINE_MAC_GLUE( NewWindow  )
	DEFINE_MAC_GLUE( NewCWindow )
	
	DEFINE_MAC_GLUE( DisposeWindow )
	
	DEFINE_MAC_GLUE_DISPATCHED( DisposeGWorld, 4, _QDExtensions, 4 )
	
#if !TARGET_API_MAC_CARBON
	
	DEFINE_MAC_GLUE( ClosePort  )
	DEFINE_MAC_GLUE( CloseCPort )
	
#endif
	
}

