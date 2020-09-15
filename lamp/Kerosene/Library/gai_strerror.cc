/*
	gai_strerror.cc
	---------------
*/

// POSIX
#include <netdb.h>


#define ARRAY_LEN( a )  (sizeof a / sizeof a[0])

static const char* error_strings[] =
{
	"unknown error",
	"invalid value for ai_flags",
	"name or service is not known",
	"temporary failure in name resolution",
	"non-recoverable failure in name resolution",
	"no address associated with name",
	"ai_family not supported",
	"ai_socktype not supported",
	"service not supported for ai_socktype",
	"address family for name not supported",
	"memory allocation failure",
	"system error",
	"invalid value for hints",
	"resolved protocol is unknown",
};

const char* gai_strerror( int error )
{
	error = -error;
	
	if ( unsigned( error ) >= ARRAY_LEN( error_strings ) )
	{
		error = 0;
	}
	
	return error_strings[ error ];
}
