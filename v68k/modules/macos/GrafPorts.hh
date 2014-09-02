/*
	GrafPorts.hh
	------------
*/

#ifndef GRAFPORTS_HH
#define GRAFPORTS_HH

pascal void OpenPort_patch ( struct GrafPort* port );
pascal void InitPort_patch ( struct GrafPort* port );
pascal void ClosePort_patch( struct GrafPort* port );

pascal void GetPort_patch( struct GrafPort** port_ptr );

pascal void SetPort_patch( struct GrafPort* port );

pascal void SetPortBits_patch( const struct BitMap* bitmap );

#endif
