/*
	_OTInetMailExchange.h
	---------------------
	
	Joshua Juran
*/

#ifndef RELIX_OTINETMAILEXCHANGE_H
#define RELIX_OTINETMAILEXCHANGE_H

#ifdef __cplusplus
extern "C" {
#endif

long _OTInetMailExchange( char *domain, unsigned short *count, struct InetMailExchange *result );

#ifdef __cplusplus
}
#endif

#endif

