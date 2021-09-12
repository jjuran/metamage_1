/*
	patterns.cc
	-----------
*/

#include "mac_qd/globals/patterns.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif


namespace mac {
namespace qd  {

#if ! __LP64__
#if OPAQUE_TOOLBOX_STRUCTS

static Pattern qd_dkGray;
static Pattern qd_ltGray;
static Pattern qd_gray;
static Pattern qd_black;
static Pattern qd_white;

const Pattern& dkGray()  { return *GetQDGlobalsDarkGray ( &qd_dkGray ); }
const Pattern& ltGray()  { return *GetQDGlobalsLightGray( &qd_ltGray ); }
const Pattern& gray()    { return *GetQDGlobalsGray     ( &qd_gray   ); }
const Pattern& black()   { return *GetQDGlobalsBlack    ( &qd_black  ); }
const Pattern& white()   { return *GetQDGlobalsWhite    ( &qd_white  ); }

#endif
#endif

int dummy;

}
}
