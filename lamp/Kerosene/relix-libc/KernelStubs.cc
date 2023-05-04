/*	==============
 *	KernelStubs.cc
 *	==============
 */

// POSIX
#include "netdb.h"


#pragma exceptions off


#if 0
#pragma export on
#endif
	
	void endhostent()
	{
	}
	
	void endprotoent()
	{
	}
	
	void endservent()
	{
	}
	
	/*
	struct hostent* gethostbyaddr( const void*, size_t, int )
	{
		return NULL;
	}
	*/
	
	struct hostent* gethostent()
	{
		return NULL;
	}
	
	struct protoent* getprotobyname( const char* name )
	{
		return NULL;
	}
	
	struct protoent* getprotobynumber( int )
	{
		return NULL;
	}
	
	struct protoent* getprotoent()
	{
		return NULL;
	}
	
	struct servent* getservbyname( const char*, const char* )
	{
		return NULL;
	}
	
	struct servent* getservbyport( int, const char* )
	{
		return NULL;
	}
	
	struct servent* getservent()
	{
		return NULL;
	}
	
	void herror( const char* )
	{
	}
	
	/*
	char* hstrerror( int )
	{
		return "";
	}
	*/
	
	void setprotoent( int )
	{
	}
	
	void setservent( int )
	{
	}
	
#if 0	
#pragma export reset
#endif
