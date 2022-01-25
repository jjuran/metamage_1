/*
	Utilities.hh
	------------
*/

#ifndef UTILITIES_HH
#define UTILITIES_HH

typedef char** Handle;

pascal Handle GetPattern_patch( short id );
pascal Handle GetCursor_patch ( short id );
pascal Handle GetPicture_patch( short id );

pascal Handle GetString_patch ( short id );

pascal Handle GetIcon_patch   ( short id );

#endif
