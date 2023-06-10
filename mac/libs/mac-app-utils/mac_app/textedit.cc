/*
	textedit.hh
	-----------
*/

#include "mac_app/textedit.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

// mac-config
#include "mac_config/upp-macros.hh"


namespace mac {
namespace app {

TEClickLoop_callback  current_TEClick_scroller_code;
void*                 current_TEClick_scroller_data;
TEClickLoop_callback  current_TEClick_contents_code;
void*                 current_TEClick_contents_data;


static
void TEClickLoop_custom()
{
	if ( current_TEClick_scroller_code )
	{
		current_TEClick_scroller_code( current_TEClick_scroller_data );
	}
	
	if ( current_TEClick_contents_code )
	{
		current_TEClick_contents_code( current_TEClick_contents_data );
	}
}

// This is set the first time we call install_custom_TEClickLoop().
static TEClickLoopUPP gSystemClickLoop = NULL;

#if TARGET_CPU_68K  &&  ! TARGET_RT_MAC_CFM

static
asm
pascal void TEClickLoop_master()
{
	MOVEA.L     gSystemClickLoop,A0  // load the default clickLoop
	
	JSR         (A0)                 // invoke it
	
	MOVEM.L     D1-D2/A1,-(SP)       // save registers
	JSR         TEClickLoop_custom   // invoke our custom clickLoop
	MOVEM.L     (SP)+,D1-D2/A1       // restore registers
	
	MOVEQ       #1,D0                // return true by clearing the zero flag
	RTS
}

#else

static
pascal Boolean TEClickLoop_master( TEPtr pTE )
{
	(void) InvokeTEClickLoopUPP( pTE, gSystemClickLoop );
	
	TEClickLoop_custom();
	
	return true;
}

#endif

void install_custom_TEClickLoop( TEHandle hTE )
{
	static TEClickLoopUPP clickLoop = gSystemClickLoop = hTE[0]->clickLoop;
	
	DEFINE_UPP( TEClickLoop, TEClickLoop_master )
	
	hTE[0]->clickLoop = UPP_ARG( TEClickLoop_master );
}

}
}
