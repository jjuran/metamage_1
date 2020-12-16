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
struct Point;
struct Rect;
struct WindowRecord;

extern MacRegion**        BezelRgn;
extern MenuList_header**  MenuList : 0x00A1C;

pascal void InitWindows_patch();

pascal void GetWMgrPort_patch( struct GrafPort** port );

pascal GrafPort* NewWindow_patch( void*                 storage,
                                  const Rect*           bounds,
                                  const unsigned char*  title,
                                  unsigned char         visible,
                                  short                 procID,
                                  GrafPort*             behind,
                                  unsigned char         closeBox,
                                  long                  refCon );

pascal GrafPort* GetNewWindow_patch( short      resID,
                                     void*      storage,
                                     GrafPort*  behind );

pascal void CloseWindow_patch  ( GrafPort* window );
pascal void DisposeWindow_patch( GrafPort* window );

pascal void SetWTitle_patch( WindowRecord* window, const unsigned char* s );
pascal void GetWTitle_patch( WindowRecord* window,       unsigned char* s );

pascal void SelectWindow_patch( WindowRecord* window );
pascal void HideWindow_patch  ( WindowRecord* window );
pascal void ShowWindow_patch  ( WindowRecord* window );
pascal void BringToFront_patch( WindowRecord* window );
pascal void DrawGrowIcon_patch( WindowRecord* window );

pascal void SendBehind_patch  ( WindowRecord* window, WindowRecord* behind );

pascal void ShowHide_patch    ( WindowRecord* window, unsigned char showFlag );
pascal void HiliteWindow_patch( WindowRecord* window, unsigned char hilite );

pascal GrafPort* FrontWindow_patch();

pascal short FindWindow_patch( Point pt, GrafPort** window );

pascal unsigned char TrackGoAway_patch( GrafPort* w, Point pt );

pascal void MoveWindow_patch( GrafPort* w, short h, short v, char activate );
pascal void SizeWindow_patch( GrafPort* w, short h, short v, char update   );

pascal void DragWindow_patch( GrafPort* w, Point start, const Rect* bounds );
pascal long GrowWindow_patch( GrafPort* w, Point start, const Rect* size   );

pascal void InvalRect_patch( const Rect* rect );
pascal void ValidRect_patch( const Rect* rect );

pascal void InvalRgn_patch( MacRegion** rgn );
pascal void ValidRgn_patch( MacRegion** rgn );

pascal void BeginUpdate_patch( GrafPort* window );
pascal void EndUpdate_patch  ( GrafPort* window );

pascal void SetWRefCon_patch( WindowRecord* window, long data );
pascal long GetWRefCon_patch( WindowRecord* window );

pascal unsigned char CheckUpdate_patch( EventRecord* event );

pascal void ClipAbove_patch( WindowRecord* window );

pascal void PaintOne_patch   ( WindowRecord* w, MacRegion** rgn );
pascal void PaintBehind_patch( WindowRecord* w, MacRegion** rgn );

pascal void CalcVis_patch( WindowRecord* window );

pascal void CalcVBehind_patch( WindowRecord* w, MacRegion** rgn );

#endif
