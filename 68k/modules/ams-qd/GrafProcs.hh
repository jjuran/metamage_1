/*
	GrafProcs.hh
	------------
*/

#ifndef GRAFPROCS_HH
#define GRAFPROCS_HH


#define CHECK_CUSTOM_GRAFPROC(port, name)  \
	if ((port).grafProcs  &&  (port).grafProcs->name) \
		{static bool has = custom_grafProc(#name);} else

bool custom_grafProc( const char* proc_name );

#endif
