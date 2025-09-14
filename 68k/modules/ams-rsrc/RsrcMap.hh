/*
	RsrcMap.hh
	----------
*/

#ifndef RSRCMAP_HH
#define RSRCMAP_HH


struct rsrc_header;
struct rsrc_map_header;

typedef char** Handle;

typedef rsrc_map_header**  RsrcMapHandle;


rsrc_header* recover_rsrc_header( Handle resource );

RsrcMapHandle& find_rsrc_map( short refnum );

RsrcMapHandle home_rsrc_map( Handle resource : __A0 );

#endif
