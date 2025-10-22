/*
	status.cc
	---------
*/

#include "status.hh"

long extension_protocol_version;
long extension_software_version;

bool running;

short startup_error;
short sound_mode;

volatile bool sound_changed;
volatile bool sound_active;
