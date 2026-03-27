/*
	Amphitheatre.cc
	---------------
*/

// glfb-common
#include "glfb/glfb.hh"

// glitter
#include "glitter/display_events.hh"

// Amphitheatre
#include "blit_CGL.hh"


#define ON_DISPLAY_EVENT  &::glitter::on_display_event

#define HARDWARE_CURSOR 1

using glfb::overlay_enabled;

typedef CGL_blitter Blitter;

#include "amnesty/skeleton.cc.hh"
