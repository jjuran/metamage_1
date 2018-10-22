/*
	Volumes.hh
	----------
*/

#ifndef VOLUMES_HH
#define VOLUMES_HH

struct VolumeParam;
struct WDPBRec;

void mount_virtual_network_volume();

short GetVol_patch( short trap_word : __D1, WDPBRec* pb : __A0 );

short FlushVol_patch( short trap_word : __D1, VolumeParam* pb : __A0 );

#endif
