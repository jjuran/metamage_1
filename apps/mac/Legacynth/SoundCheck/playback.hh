/*
	playback.hh
	-----------
*/

#ifndef PLAYBACK_HH
#define PLAYBACK_HH


typedef short OSErr;

extern volatile bool playing;
extern volatile bool updated;

OSErr start_playback();
OSErr stop_playback();

#endif
