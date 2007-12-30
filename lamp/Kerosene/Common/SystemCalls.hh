/*	==============
 *	SystemCalls.hh
 *	==============
 */

#ifndef KEROSENE_SYSTEMCALLS_HH
#define KEROSENE_SYSTEMCALLS_HH

// Universal Interfaces
#include <TargetConditionals.h>
#if TARGET_RT_MAC_MACHO
#include <Carbon/Carbon.h>
#else
#include <Files.h>
#endif

struct AEDesc;
typedef AEDesc AppleEvent;

struct InetMailExchange;


#ifdef __cplusplus
#include <string>
#endif


#ifdef __cplusplus
extern "C" {
#endif
	
	OSStatus AESendBlocking( const AppleEvent* appleEvent, AppleEvent* reply );
	
	OSStatus OTInetMailExchange_k( char* domain, UInt16* count, InetMailExchange* result );
	
	FSSpec Path2FSS( const char* pathname );
	
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
	
	inline FSSpec Path2FSS( const std::string& pathname )
	{
		return Path2FSS( pathname.c_str() );
	}
	
#endif

#endif

