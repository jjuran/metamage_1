/*
	Windows.hh
	----------
*/

#ifndef WINDOWS_HH
#define WINDOWS_HH

struct EventRecord;
struct GrafPort;
struct MacRegion;
struct MenuList_header;
struct Picture;
struct Point;
struct Rect;
struct WindowRecord;

typedef unsigned char Boolean;

typedef const unsigned char* ConstStr255Param;
typedef       unsigned char* StringPtr;

typedef Picture**   PicHandle;
typedef MacRegion** RgnHandle;

typedef GrafPort* GrafPtr;
typedef GrafPort* WindowRef;

typedef WindowRecord* WindowPeek;

extern RgnHandle          BezelRgn;
extern MenuList_header**  MenuList : 0x00A1C;

pascal void InitWindows_patch();

pascal void GetWMgrPort_patch( GrafPtr* port );

pascal WindowRef NewWindow_patch( void*             storage,
                                  const Rect*       bounds,
                                  ConstStr255Param  title,
                                  Boolean           visible,
                                  short             procID,
                                  WindowRef         behind,
                                  Boolean           closeBox,
                                  long              refCon );

pascal WindowRef GetNewWindow_patch( short      resID,
                                     void*      storage,
                                     WindowRef  behind );

pascal void CloseWindow_patch  ( WindowRef window );
pascal void DisposeWindow_patch( WindowRef window );

pascal void SetWTitle_patch( WindowPeek window, ConstStr255Param s );
pascal void GetWTitle_patch( WindowPeek window, StringPtr        s );

pascal void SelectWindow_patch( WindowPeek window );
pascal void HideWindow_patch  ( WindowPeek window );
pascal void ShowWindow_patch  ( WindowPeek window );
pascal void BringToFront_patch( WindowPeek window );
pascal void DrawGrowIcon_patch( WindowPeek window );

pascal void SendBehind_patch  ( WindowPeek window, WindowPeek behind );

pascal void ShowHide_patch    ( WindowPeek window, Boolean showFlag );
pascal void HiliteWindow_patch( WindowPeek window, Boolean hilite );

pascal WindowRef FrontWindow_patch();

pascal short FindWindow_patch( Point pt, WindowRef* window );

pascal Boolean TrackGoAway_patch( WindowRef w, Point pt );

pascal void MoveWindow_patch( WindowRef w, short h, short v, char activate );
pascal void SizeWindow_patch( WindowRef w, short h, short v, char update   );

pascal void DragWindow_patch( WindowRef w, Point start, const Rect* bounds );
pascal long GrowWindow_patch( WindowRef w, Point start, const Rect* size   );

pascal void InvalRect_patch( const Rect* rect );
pascal void ValidRect_patch( const Rect* rect );

pascal void InvalRgn_patch( RgnHandle rgn );
pascal void ValidRgn_patch( RgnHandle rgn );

pascal void BeginUpdate_patch( WindowRef window );
pascal void EndUpdate_patch  ( WindowRef window );

pascal void SetWRefCon_patch( WindowPeek window, long data );
pascal long GetWRefCon_patch( WindowPeek window );

pascal void SetWindowPic_patch( WindowPeek window, PicHandle pic );

pascal Boolean CheckUpdate_patch( EventRecord* event );

pascal void ClipAbove_patch( WindowPeek window );

pascal void PaintOne_patch   ( WindowPeek w, RgnHandle rgn );
pascal void PaintBehind_patch( WindowPeek w, RgnHandle rgn );

pascal void CalcVis_patch( WindowPeek window );

pascal void CalcVBehind_patch( WindowPeek w, RgnHandle rgn );

#endif
