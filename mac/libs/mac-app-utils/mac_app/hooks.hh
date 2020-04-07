/*
	hooks.hh
	--------
*/

#ifndef MACAPP_HOOKS_HH
#define MACAPP_HOOKS_HH

namespace mac {
namespace app {
	
	typedef long (*void_hook)();
	
	extern void_hook oapp_hook;
	extern void_hook quit_hook;
	
}
}

#endif
