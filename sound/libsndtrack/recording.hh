/*
	recording.hh
	------------
*/

#ifndef RECORDING_HH
#define RECORDING_HH

void start_recording( const char* path );

void append_to_recording( const void* data, unsigned size );

void end_recording();

#endif
