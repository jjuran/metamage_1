/*
	Amethyst.cc
	-----------
*/

// Amethyst
#include "blit_CG.hh"
#include "blit_QD.hh"
#include "quickdraw.hh"


#if CONFIG_QUICKDRAW
	typedef QD_blitter Blitter;
#else
	typedef CG_blitter Blitter;
#endif

static bool overlay_enabled;

#include "amicus/skeleton.cc.hh"
