/*	==============
 *	SystemCalls.hh
 *	==============
 */

#ifndef KEROSENE_SYSTEMCALLS_HH
#define KEROSENE_SYSTEMCALLS_HH

// Universal Interfaces
//#include <AEDataModel.h>
#include <Files.h>

struct AEDesc;
typedef AEDesc AppleEvent;

struct InetMailExchange;


#ifdef __cplusplus
#include <string>
#endif


#ifdef __cplusplus
extern "C" {
#endif
	
	int* ErrnoPtr();
	
	char*** EnvironPtr();
	
	OSStatus AESendBlocking( const AppleEvent* appleEvent, AppleEvent* reply );
	
	OSStatus OTInetMailExchange_Kernel( char* domain, UInt16* count, InetMailExchange* result );
	
	FSSpec Path2FSS( const char* pathname );
	
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
	
	inline int& Errno()  { return *ErrnoPtr(); }
	
	inline char**& Environ()  { return *EnvironPtr(); }
	
	inline FSSpec Path2FSS( const std::string& pathname )
	{
		return Path2FSS( pathname.c_str() );
	}
	
#endif

#endif

