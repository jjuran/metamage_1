/*
	payload.hh
	----------
*/

#ifndef PAYLOAD_HH
#define PAYLOAD_HH


typedef unsigned char Byte;

typedef short OSErr;

extern unsigned long payload_size;

extern const Byte* payload_data;
extern const Byte* payload_name;

OSErr ready_payload();

#endif
