/*
	GrafPorts.hh
	------------
*/

#ifndef GRAFPORTS_HH
#define GRAFPORTS_HH

pascal void OpenPort_patch ( struct GrafPort* port );
pascal void InitPort_patch ( struct GrafPort* port );
pascal void ClosePort_patch( struct GrafPort* port );

pascal void SetPort_patch( struct GrafPort* port );

pascal void GetPort_patch( struct GrafPort** port_ptr );

pascal void SetPortBits_patch( const struct BitMap* bitmap );

pascal void PortSize_patch( short width, short height );

pascal void MovePortTo_patch( short left, short top );

pascal void SetOrigin_patch( short h, short v );

pascal void SetClip_patch( struct MacRegion** clipRgn );
pascal void GetClip_patch( struct MacRegion** clipRgn );

pascal void ClipRect_patch( const struct Rect* clipRect );

pascal void BackPat_patch( const struct Pattern* pat );

pascal void ForeColor_patch( long color );
pascal void BackColor_patch( long color );

pascal void LocalToGlobal_patch( struct Point* pt );
pascal void GlobalToLocal_patch( struct Point* pt );

pascal unsigned char GetPixel_patch( short h, short v );

#endif
