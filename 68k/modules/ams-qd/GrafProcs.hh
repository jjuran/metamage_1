/*
	GrafProcs.hh
	------------
*/

#ifndef GRAFPROCS_HH
#define GRAFPROCS_HH


#define CHECK_CUSTOM_GRAFPROC(port, name)  \
	if ((port).grafProcs  &&  (port).grafProcs->name) \
		{static bool has = custom_grafProc(#name);} else

#define GRAFPROCS( port, name )  \
	(&null_QDProcs)

#define GRAFPROC( port, name )  GRAFPROCS( port, name )->name

#define GRAFPROC_ARC( port )     GRAFPROC( port, arcProc     )
#define GRAFPROC_BITS( port )    GRAFPROC( port, bitsProc    )
#define GRAFPROC_LINE( port )    GRAFPROC( port, lineProc    )
#define GRAFPROC_OVAL( port )    GRAFPROC( port, ovalProc    )
#define GRAFPROC_POLY( port )    GRAFPROC( port, polyProc    )
#define GRAFPROC_RECT( port )    GRAFPROC( port, rectProc    )
#define GRAFPROC_RGN( port )     GRAFPROC( port, rgnProc     )
#define GRAFPROC_RRECT( port )   GRAFPROC( port, rRectProc   )
#define GRAFPROC_TEXT( port )    GRAFPROC( port, textProc    )
#define GRAFPROC_TXMEAS( port )  GRAFPROC( port, txMeasProc  )

extern const struct QDProcs null_QDProcs;

bool custom_grafProc( const char* proc_name );

#endif
