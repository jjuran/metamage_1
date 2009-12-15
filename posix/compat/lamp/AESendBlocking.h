/*
	AESendBlocking.h
	----------------
	
	Joshua Juran
*/

#ifndef LAMP_AESENDBLOCKING_H
#define LAMP_AESENDBLOCKING_H

#ifdef __cplusplus
extern "C" {
#endif

long AESendBlocking( const struct AEDesc *appleEvent, struct AEDesc *reply );

#ifdef __cplusplus
}
#endif

#endif

