/*
	install.hh
	----------
*/

#ifndef INSTALL_HH
#define INSTALL_HH


struct FSSpec;

typedef short OSErr;

OSErr install_to( const FSSpec& location );

#endif
