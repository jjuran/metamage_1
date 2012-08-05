/*
	AESendBlocking.h
	----------------
	
	Joshua Juran
*/

#ifndef RELIX_AESENDBLOCKING_H
#define RELIX_AESENDBLOCKING_H

#ifdef __cplusplus
extern "C" {
#endif

long AESendBlocking( const struct AEDesc *appleEvent, struct AEDesc *reply );

#ifdef __cplusplus
}
#endif

#endif

