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

typedef unsigned char Boolean;
typedef unsigned char Byte;

typedef ControlRecord** ControlRef;

typedef GrafPort* WindowRef;

typedef MacRegion** RgnHandle;

typedef pascal void (*ControlActionProcPtr)( ControlRef, short );

typedef ControlActionProcPtr ControlActionUPP;

pascal ControlRef NewControl_patch( WindowRef    window,
                                    const Rect*  bounds,
                                    const Byte*  title,
                                    Boolean      visible,
                                    short        value,
                                    short        min,
                                    short        max,
                                    short        procID,
                                    long         refCon );

pascal ControlRef GetNewControl_patch( short controlID, WindowRef window );

pascal void DisposeControl_patch( ControlRef control );

pascal void KillControls_patch( WindowRef window );

pascal void SetCTitle_patch( ControlRef cntl, const Byte* title );
pascal void GetCTitle_patch( ControlRef cntl,       Byte* title );

pascal void HideControl_patch( ControlRef control );
pascal void ShowControl_patch( ControlRef control );

pascal void DrawControls_patch( WindowRef window );

pascal void HiliteControl_patch( ControlRef control, short hiliteState );

pascal short FindControl_patch( Point        where,
                                WindowRef    window,
                                ControlRef*  which );

pascal short TrackControl_patch( ControlRef        control,
                                 Point             start,
                                 ControlActionUPP  action );

pascal short TestControl_patch( ControlRef control, Point pt );

pascal void MoveControl_patch( ControlRef control, short w, short h );
pascal void SizeControl_patch( ControlRef control, short w, short h );

pascal void SetCtlValue_patch( ControlRef control, short value );

pascal short GetCtlValue_patch( ControlRef control );

pascal void SetMinCtl_patch( ControlRef control, short min );
pascal void SetMaxCtl_patch( ControlRef control, short max );

pascal short GetMinCtl_patch( ControlRef control );
pascal short GetMaxCtl_patch( ControlRef control );

pascal void SetCRefCon_patch( ControlRef control, long refCon );
pascal long GetCRefCon_patch( ControlRef control );

pascal void SetCtlAction_patch( ControlRef control, ControlActionProcPtr action );

pascal ControlActionProcPtr GetCtlAction_patch( ControlRef control );

pascal void UpdateControls_patch( WindowRef window, RgnHandle updateRgn );

pascal void Draw1Control_patch( ControlRef control );

pascal short GetCVariant_patch( ControlRef control );

#endif
