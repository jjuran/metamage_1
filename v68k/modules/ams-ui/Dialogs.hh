/*
	Dialogs.hh
	----------
*/

#ifndef DIALOGS_HH
#define DIALOGS_HH

struct EventRecord;
struct GrafPort;
struct Point;
struct Rect;

typedef pascal unsigned char (*ModalFilterUPP)( GrafPort*     dialog,
                                                EventRecord*  event,
                                                short*        itemHit );

pascal void InitDialogs_patch( void* proc );
pascal void ErrorSound_patch ( void* proc );

pascal GrafPort* NewDialog_patch( void*                 storage,
                                  const Rect*           bounds,
                                  const unsigned char*  title,
                                  unsigned char         visible,
                                  short                 procID,
                                  GrafPort*             behind,
                                  unsigned char         closable,
                                  long                  refCon,
                                  char**                items );

pascal GrafPort* GetNewDialog_patch( short id, void* ds, GrafPort* behind );

pascal void CloseDialog_patch( GrafPort* dialog );

pascal void DisposeDialog_patch( GrafPort* dialog );

pascal void ModalDialog_patch( ModalFilterUPP filterProc, short* itemHit );

pascal unsigned char IsDialogEvent_patch( const EventRecord* event );

pascal unsigned char DialogSelect_patch( const EventRecord*  event,
                                         GrafPort**          dialogHit,
                                         short*              itemHit );

pascal void DrawDialog_patch( GrafPort* dialog );

pascal short Alert_patch       ( short alertID, ModalFilterUPP filterProc );
pascal short StopAlert_patch   ( short alertID, ModalFilterUPP filterProc );
pascal short NoteAlert_patch   ( short alertID, ModalFilterUPP filterProc );
pascal short CautionAlert_patch( short alertID, ModalFilterUPP filterProc );

pascal void ParamText_patch( const unsigned char*  p1,
                             const unsigned char*  p2,
                             const unsigned char*  p3,
                             const unsigned char*  p4 );

pascal void GetDItem_patch( GrafPort*  dialog,
                            short      i,
                            short*     type,
                            char***    h,
                            Rect*      box );

pascal void SetDItem_patch( GrafPort*    dialog,
                            short        i,
                            short        type,
                            char**       h,
                            const Rect*  box );

pascal void GetIText_patch( char** h, unsigned char text[ 255 ] );

pascal void SetIText_patch( char** h, const unsigned char* text );

pascal void SelIText_patch( GrafPort*  dialog,
                            short      item,
                            short      start,
                            short      end );

pascal void HideDItem_patch( GrafPort* dialog, short i );
pascal void ShowDItem_patch( GrafPort* dialog, short i );

pascal short FindDItem_patch( GrafPort* dialog, Point pt );

#endif
