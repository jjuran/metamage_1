/*
	DRVR.hh
	-------
*/

#ifndef DRVR_HH
#define DRVR_HH

struct CntrlParam;
struct DCtlEntry;
struct DRVRHeader;
struct IOParam;

typedef DCtlEntry DCE;

typedef short (*driver_routine)( short : __D1, IOParam* : __A0, DCE* : __A1 );

typedef short (*cntrl_routine)( short : __D1, CntrlParam* : __A0, DCE* : __A1 );

DRVRHeader** make_DRVR( const unsigned char*  name,
                        driver_routine        open,
                        driver_routine        prime,
                        cntrl_routine         cntrl,
                        cntrl_routine         status,
                        driver_routine        close );

#endif
