/*
	GrafProcs.hh
	------------
*/

#ifndef GRAFPROCS_HH
#define GRAFPROCS_HH


#define CHECK_CUSTOM_GRAFPROC(port, name)  \
	if ((port).grafProcs  &&  (port).grafProcs->name) \
		{static bool has = custom_grafProc(#name);} else

#define GRAFPROC_ARC( port )     StdArc
#define GRAFPROC_BITS( port )    StdBits
#define GRAFPROC_LINE( port )    StdLine
#define GRAFPROC_OVAL( port )    StdOval
#define GRAFPROC_POLY( port )    StdPoly
#define GRAFPROC_RECT( port )    StdRect
#define GRAFPROC_RGN( port )     StdRgn
#define GRAFPROC_RRECT( port )   StdRRect
#define GRAFPROC_TEXT( port )    StdText
#define GRAFPROC_TXMEAS( port )  StdTxMeas

bool custom_grafProc( const char* proc_name );

#endif
