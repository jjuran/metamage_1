/*
	Controls.hh
	-----------
*/

#ifndef CONTROLS_HH
#define CONTROLS_HH

struct GrafPort;
struct MacRegion;
struct Point;
struct Rect;
struct ControlRecord;

pascal void UpdateControls_patch( GrafPort* window, MacRegion** updateRgn );

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

pascal short TrackControl_patch( ControlRecord**  control,
                                 Point            start,
                                 pascal void    (*action)() );

pascal void DrawControls_patch( GrafPort* window );

pascal short FindControl_patch( Point             where,
                                GrafPort*         window,
                                ControlRecord***  which );

pascal void SetCtlValue_patch( ControlRecord** control, short value );

pascal short GetCtlValue_patch( ControlRecord** control );

#endif
