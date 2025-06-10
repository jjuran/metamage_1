/*
	GrafProcs.cc
	------------
*/

#include "GrafProcs.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// log-of-war
#include "logofwar/report.hh"


enum
{
	_StdText    = 0xA882,
	_StdLine    = 0xA890,
	_StdRect    = 0xA8A0,
	_StdRRect   = 0xA8AF,
	_StdOval    = 0xA8B6,
	_StdArc     = 0xA8BD,
	_StdPoly    = 0xA8C5,
	_StdRgn     = 0xA8D1,
	_StdBits    = 0xA8EB,
	_StdTxMeas  = 0xA8ED,
	_StdGetPic  = 0xA8EE,
	_StdPutPic  = 0xA8F0,
	_StdComment = 0xA8F1,
	
	autoPop = 0x0400,
};

static const UInt16 QDProc_traps[] =
{
	_StdText    | autoPop,
	_StdLine    | autoPop,
	_StdRect    | autoPop,
	_StdRRect   | autoPop,
	_StdOval    | autoPop,
	_StdArc     | autoPop,
	_StdPoly    | autoPop,
	_StdRgn     | autoPop,
	_StdBits    | autoPop,
	_StdComment | autoPop,
	_StdTxMeas  | autoPop,
	_StdGetPic  | autoPop,
	_StdPutPic  | autoPop,
};

const QDProcs null_QDProcs =
{
	(QDTextUPP)    &QDProc_traps[  0 ],
	(QDLineUPP)    &QDProc_traps[  1 ],
	(QDRectUPP)    &QDProc_traps[  2 ],
	(QDRRectUPP)   &QDProc_traps[  3 ],
	(QDOvalUPP)    &QDProc_traps[  4 ],
	(QDArcUPP)     &QDProc_traps[  5 ],
	(QDPolyUPP)    &QDProc_traps[  6 ],
	(QDRgnUPP)     &QDProc_traps[  7 ],
	(QDBitsUPP)    &QDProc_traps[  8 ],
	(QDCommentUPP) &QDProc_traps[  9 ],
	(QDTxMeasUPP)  &QDProc_traps[ 10 ],
	(QDGetPicUPP)  &QDProc_traps[ 11 ],
	(QDPutPicUPP)  &QDProc_traps[ 12 ],
};

bool custom_grafProc( const char* proc_name )
{
	WARNING = "custom grafProcs->", proc_name;
	
	return true;
}
