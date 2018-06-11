/*
	Dialogs.hh
	----------
*/

#ifndef DIALOGS_HH
#define DIALOGS_HH

struct EventRecord;
struct GrafPort;

typedef pascal unsigned char (*ModalFilterUPP)( GrafPort*     dialog,
                                                EventRecord*  event,
                                                short*        itemHit );

pascal void InitDialogs_patch( void* proc );

pascal short Alert_patch       ( short alertID, ModalFilterUPP filterProc );
pascal short StopAlert_patch   ( short alertID, ModalFilterUPP filterProc );
pascal short NoteAlert_patch   ( short alertID, ModalFilterUPP filterProc );
pascal short CautionAlert_patch( short alertID, ModalFilterUPP filterProc );

pascal void ParamText_patch( const unsigned char*  p1,
                             const unsigned char*  p2,
                             const unsigned char*  p3,
                             const unsigned char*  p4 );

#endif
