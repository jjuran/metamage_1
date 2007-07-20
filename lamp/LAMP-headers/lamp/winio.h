#ifndef	LAMP_WINIO_H
#define	LAMP_WINIO_H

// POSIX
#include <sys/ioccom.h>


#define WIOCGTITLE  _IOR( 'w', 1, char )  // get window title
#define WIOCSTITLE  _IOW( 'w', 2, char )  // set window title

enum
{
	kLampSalvageWindowOnExitNever      = 0,
	kLampSalvageWindowOnExitForFailure = 1,
	kLampSalvageWindowOnExitForSignal  = 2,
	kLampSalvageWindowOnExitAlways     = 0xFFFFFFFFU
};

#define WIOCGEXIT  _IOR( 'w', 3, int )  // get close-on-exit behavior
#define WIOCSEXIT  _IOW( 'w', 4, int )  // set close-on-exit behavior

#endif

