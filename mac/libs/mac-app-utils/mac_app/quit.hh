/*
	quit.hh
	-------
*/

#ifndef MACAPP_QUIT_HH
#define MACAPP_QUIT_HH


namespace mac {
namespace app {

void quit_Cocoa();

inline
void quit()
{
#ifdef __OBJC__
	
	quit_Cocoa();
	
#else
	
	#error Carbon quit() is unimplemented
	
#endif
}

}
}

#endif
