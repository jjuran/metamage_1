/*
	print.cc
	--------
*/

#include "v68k/print.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k
{

char const* const function_code_names[] =
{
	"FC=0 Space",
	"User Data Space",
	"User Program Space",
	"Utility Space",
	"FC=4 Space",
	"Supervisor Data Space",
	"Supervisor Program Space",
	"CPU Space",
};

char const* const mem_access_names[] =
{
	"exec",
	"read",
	"write",
	"update",
};

}
