/*
	Files.hh
	--------
*/

#ifndef FILES_HH
#define FILES_HH

struct FileParam;
struct VolumeParam;

short GetVol_patch( short trap_word : __D1, VolumeParam* pb : __A0 );

short Create_patch( short trap_word : __D1, FileParam* pb : __A0 );
short Open_patch  ( short trap_word : __D1, FileParam* pb : __A0 );

#endif
