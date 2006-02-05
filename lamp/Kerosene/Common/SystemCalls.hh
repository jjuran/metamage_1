/*	==============
 *	SystemCalls.hh
 *	==============
 */

#ifndef KEROSENE_SYSTEMCALLS_HH
#define KEROSENE_SYSTEMCALLS_HH

// Universal Interfaces
#include <AEDataModel.h>
#include <Files.h>

//#include <OpenTransportProviders.h>

/*
   Define the InetSvcRef type.  This type needs special
   processing because in C++ it's a subclass of TProvider.
   See the definition of TEndpointRef in "OpenTransport.h"
   for the logic behind this definition.
*/

#ifdef __cplusplus
 typedef class TInternetServices*   InetSvcRef;
#else
 typedef void*                      InetSvcRef;
#endif


#ifdef __cplusplus
#include <string>
#endif


#ifdef __cplusplus
extern "C" {
#endif
	
	int* ErrnoPtr();
	
	char*** EnvironPtr();
	
	OSStatus AESendBlocking( const AppleEvent* appleEvent, AppleEvent* reply );
	
	InetSvcRef InternetServices();
	
	void PrintPS();
	
	FSSpec CurrentDirectory();
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

