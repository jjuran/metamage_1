/*
	icon.hh
	-------
*/

#ifndef LOADER_ICON_HH
#define LOADER_ICON_HH

typedef char** Handle;
typedef unsigned char Str255[ 256 ];

Handle get_icon( Str255 name );

#endif
