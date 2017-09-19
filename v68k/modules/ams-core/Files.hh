/*
	Files.hh
	--------
*/

#ifndef FILES_HH
#define FILES_HH

struct FileParam;
struct VolumeParam;

pascal short GetVol_patch( VolumeParam* pb : __A0 ) : __D0;

pascal short Create_patch( FileParam* pb : __A0 ) : __D0;
pascal short Open_patch  ( FileParam* pb : __A0 ) : __D0;

#endif
