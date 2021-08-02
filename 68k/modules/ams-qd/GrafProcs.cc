/*
	GrafProcs.cc
	------------
*/

#include "GrafProcs.hh"

// log-of-war
#include "logofwar/report.hh"


bool custom_grafProc( const char* proc_name )
{
	WARNING = "custom grafProcs->", proc_name;
	
	return true;
}
