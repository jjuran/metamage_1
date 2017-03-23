/*
	Controls.hh
	-----------
*/

#ifndef CONTROLS_HH
#define CONTROLS_HH

struct GrafPort;
struct Rect;
struct ControlRecord;

pascal ControlRecord** NewControl_patch( GrafPort*             window,
                                         const Rect*           bounds,
                                         const unsigned char*  title,
                                         short                 visible,
                                         short                 value,
                                         short                 min,
                                         short                 max,
                                         short                 procID,
                                         long                  refCon );

pascal void DisposeControl_patch( ControlRecord** control );

pascal void KillControls_patch( GrafPort* window );

#endif
