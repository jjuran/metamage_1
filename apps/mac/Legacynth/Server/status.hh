/*
	status.hh
	---------
*/

#ifndef STATUS_HH
#define STATUS_HH

extern long extension_protocol_version;
extern long extension_software_version;

extern bool running;

extern short startup_error;
extern short sound_mode;

extern volatile bool sound_changed;
extern volatile bool sound_active;

#endif
