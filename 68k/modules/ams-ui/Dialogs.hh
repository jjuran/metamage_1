/*
	Dialogs.hh
	----------
*/

#ifndef DIALOGS_HH
#define DIALOGS_HH

struct EventRecord;
struct GrafPort;
struct MacRegion;
struct Point;
struct Rect;

typedef unsigned char Boolean;
typedef unsigned char Byte;

typedef char** Handle;

typedef MacRegion** RgnHandle;

typedef GrafPort* WindowRef;
typedef GrafPort* DialogRef;

typedef pascal Boolean (*ModalFilterUPP)( DialogRef     dialog,
                                          EventRecord*  event,
                                          short*        itemHit );

pascal Boolean basic_filterProc( DialogRef     dialog,
                                 EventRecord*  event,
                                 short*        itemHit );

pascal void InitDialogs_patch( void* proc );
pascal void ErrorSound_patch ( void* proc );

pascal DialogRef NewDialog_patch( void*        storage,
                                  const Rect*  bounds,
                                  const Byte*  title,
                                  Boolean      visible,
                                  short        procID,
                                  WindowRef    behind,
                                  Boolean      closable,
                                  long         refCon,
                                  Handle       items );

pascal DialogRef GetNewDialog_patch( short id, void* ds, WindowRef behind );

pascal void CloseDialog_patch( DialogRef dialog );

pascal void DisposeDialog_patch( DialogRef dialog );

pascal void CouldDialog_patch( short id );
pascal void FreeDialog_patch ( short id );

pascal void ModalDialog_patch( ModalFilterUPP filterProc, short* itemHit );

pascal Boolean IsDialogEvent_patch( const EventRecord* event );

pascal Boolean DialogSelect_patch( const EventRecord*  event,
                                   DialogRef*          dialogHit,
                                   short*              itemHit );

pascal void DrawDialog_patch( DialogRef dialog );

pascal short Alert_patch       ( short alertID, ModalFilterUPP filterProc );
pascal short StopAlert_patch   ( short alertID, ModalFilterUPP filterProc );
pascal short NoteAlert_patch   ( short alertID, ModalFilterUPP filterProc );
pascal short CautionAlert_patch( short alertID, ModalFilterUPP filterProc );

pascal void CouldAlert_patch( short id );
pascal void FreeAlert_patch ( short id );

pascal void ParamText_patch( const Byte*  p1,
                             const Byte*  p2,
                             const Byte*  p3,
                             const Byte*  p4 );

pascal void GetDItem_patch( DialogRef  dialog,
                            short      i,
                            short*     type,
                            Handle*    h,
                            Rect*      box );

pascal void SetDItem_patch( DialogRef    dialog,
                            short        i,
                            short        type,
                            Handle       h,
                            const Rect*  box );

pascal void GetIText_patch( Handle h, Byte text[ 255 ] );

pascal void SetIText_patch( Handle h, const Byte* text );

pascal void SelIText_patch( DialogRef  dialog,
                            short      item,
                            short      start,
                            short      end );

pascal void HideDItem_patch( DialogRef dialog, short i );
pascal void ShowDItem_patch( DialogRef dialog, short i );

pascal short FindDItem_patch( DialogRef dialog, Point pt );

pascal void UpdateDialog_patch( DialogRef dialog, RgnHandle updateRgn );

#endif
