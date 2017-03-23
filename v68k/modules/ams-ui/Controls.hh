/*
	Controls.hh
	-----------
*/

#ifndef CONTROLS_HH
#define CONTROLS_HH

struct GrafPort;
struct ControlRecord;

pascal void DisposeControl_patch( ControlRecord** control );

pascal void KillControls_patch( GrafPort* window );

#endif
