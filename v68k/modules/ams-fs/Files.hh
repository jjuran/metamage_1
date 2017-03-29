/*
	Files.hh
	--------
*/

#ifndef FILES_HH
#define FILES_HH

struct FileParam;
struct IOParam;

typedef short (*Open_ProcPtr)( short trap_word : __D1, FileParam* pb : __A0 );
typedef short (*IO_ProcPtr  )( short trap_word : __D1, IOParam*   pb : __A0 );

extern Open_ProcPtr old_Open;
extern IO_ProcPtr   old_Close;
extern IO_ProcPtr   old_Read;
extern IO_ProcPtr   old_Write;

void initialize();

short Create_patch( short trap_word : __D1, FileParam* pb : __A0 );

short Open_patch  ( short trap_word : __D1, IOParam* pb : __A0 );
short OpenRF_patch( short trap_word : __D1, IOParam* pb : __A0 );
short Read_patch  ( short trap_word : __D1, IOParam* pb : __A0 );
short Write_patch ( short trap_word : __D1, IOParam* pb : __A0 );

short GetFPos_patch( short trap_word : __D1, IOParam* pb : __A0 );
short SetFPos_patch( short trap_word : __D1, IOParam* pb : __A0 );
short GetEOF_patch ( short trap_word : __D1, IOParam* pb : __A0 );

short Close_patch( short trap_word : __D1, IOParam* pb : __A0 );

short Delete_patch( short trap_word : __D1, FileParam* pb : __A0 );

short GetFileInfo_patch( short trap_word : __D1, FileParam* pb : __A0 );
short SetFileInfo_patch( short trap_word : __D1, FileParam* pb : __A0 );

void FSDispatch_patch( short trap_word : __D1, short selector : __D0 );

#endif
