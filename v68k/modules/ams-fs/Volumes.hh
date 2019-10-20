/*
	Volumes.hh
	----------
*/

#ifndef VOLUMES_HH
#define VOLUMES_HH

struct VCB;
struct VolumeParam;
struct WDPBRec;

void mount_virtual_network_volume();

VCB* VCB_lookup( short whichVol );

VCB* VCB_lookup( const unsigned char* name );

short GetVolInfo_patch( short trap_word : __D1, VolumeParam* pb : __A0 );

short GetVol_patch( short trap_word : __D1, WDPBRec* pb : __A0 );
short SetVol_patch( short trap_word : __D1, WDPBRec* pb : __A0 );

short FlushVol_patch( short trap_word : __D1, VolumeParam* pb : __A0 );

#endif
