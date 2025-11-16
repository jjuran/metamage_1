/*
	location.hh
	-----------
*/

#ifndef LOCATION_HH
#define LOCATION_HH


struct FSSpec;

typedef short OSErr;

extern FSSpec install_location;

OSErr choose_install_location();

#endif
