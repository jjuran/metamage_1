/*
	Windows.hh
	----------
*/

#ifndef WINDOWS_HH
#define WINDOWS_HH

struct EventRecord;
struct GrafPort;
struct Point;
struct Rect;

extern struct MacRegion** BezelRgn;

pascal void DrawGrowIcon_patch( struct WindowRecord* window );

pascal void ClipAbove_patch( struct WindowRecord* window );

pascal void PaintOne_patch   ( struct WindowRecord* w, struct MacRegion** rgn );
pascal void PaintBehind_patch( struct WindowRecord* w, struct MacRegion** rgn );

pascal unsigned char CheckUpdate_patch( EventRecord* event );

pascal void InitWindows_patch();

pascal struct GrafPort* NewWindow_patch( void*                 storage,
                                         const struct Rect*    bounds,
                                         const unsigned char*  title,
                                         short                 visible,
                                         short                 procID,
                                         struct GrafPort*      behind,
                                         unsigned char         closeBox,
                                         long                  refCon );

pascal void DisposeWindow_patch( struct GrafPort* window );

pascal void MoveWindow_patch( GrafPort* w, short h, short v, char activate );
pascal void SizeWindow_patch( GrafPort* w, short h, short v, char update   );

pascal unsigned char TrackGoAway_patch( struct GrafPort* w, struct Point pt );

pascal void BeginUpdate_patch( struct GrafPort* window );
pascal void EndUpdate_patch  ( struct GrafPort* window );

pascal struct GrafPort* FrontWindow_patch();

pascal void DragWindow_patch( GrafPort* w, Point start, const Rect* bounds );
pascal long GrowWindow_patch( GrafPort* w, Point start, const Rect* size   );

pascal void InvalRect_patch( const Rect* rect );
pascal void ValidRect_patch( const Rect* rect );

pascal void InvalRgn_patch( struct MacRegion** rgn );
pascal void ValidRgn_patch( struct MacRegion** rgn );

pascal short FindWindow_patch( struct Point pt, struct GrafPort** window );

pascal void CloseWindow_patch( struct GrafPort* window );

#endif
