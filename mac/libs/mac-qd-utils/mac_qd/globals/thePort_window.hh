/*
	thePort.hh
	----------
*/

#ifndef MACQD_GLOBALS_THEPORTWINDOW_HH
#define MACQD_GLOBALS_THEPORTWINDOW_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// mac-qd-utils
#include "mac_qd/globals/thePort.hh"


namespace mac {
namespace qd  {

inline
WindowRef thePort_window()
{
	return GetWindowFromPort( (CGrafPtr) thePort() );
}

}
}

#endif
