/*	==============
 *	SystemCalls.hh
 *	==============
 */

#ifndef KEROSENE_SYSTEMCALLS_HH
#define KEROSENE_SYSTEMCALLS_HH

struct AEDesc;
typedef AEDesc AppleEvent;

struct InetMailExchange;

#ifdef __cplusplus
extern "C" {
#endif
	
	OSStatus AESendBlocking( const AppleEvent* appleEvent, AppleEvent* reply );
	
	OSStatus OTInetMailExchange_k( char* domain, UInt16* count, InetMailExchange* result );
	
#ifdef __cplusplus
}
#endif

#endif

