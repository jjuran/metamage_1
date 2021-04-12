/*
	Amethyst.cc
	-----------
*/

// amicus
#include "amicus/quickdraw.hh"

// Amethyst
#include "blit_CG.hh"
#include "blit_QD.hh"


#if CONFIG_QUICKDRAW
	typedef QD_blitter Blitter;
#else
	typedef CG_blitter Blitter;
#endif

#include "amicus/skeleton.cc.hh"
