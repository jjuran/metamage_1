/*
	DRVR.hh
	-------
*/

#ifndef DRVR_HH
#define DRVR_HH

struct CntrlParam;
struct DRVRHeader;
struct IOParam;

typedef short (*driver_routine)( short trap_word : __D1, IOParam* pb : __A0 );

typedef short (*cntrl_routine)( short trap_word : __D1, CntrlParam* pb : __A0 );

DRVRHeader** make_DRVR( const unsigned char*  name,
                        driver_routine        open,
                        driver_routine        prime,
                        cntrl_routine         cntrl,
                        cntrl_routine         status,
                        driver_routine        close );

#endif
