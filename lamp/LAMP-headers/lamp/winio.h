#ifndef	LAMP_WINIO_H
#define	LAMP_WINIO_H

// POSIX
#include <sys/ioccom.h>


#define WIOCGTITLE  _IOR( 'w',  1, char     )  // get window title
#define WIOCSTITLE  _IOW( 'w',  2, char     )  // set window title
#define WIOCGPOS    _IOR( 'w',  3, short[2] )  // get window position
#define WIOCSPOS    _IOW( 'w',  4, short[2] )  // set window position
#define WIOCGSIZE   _IOR( 'w',  5, short[2] )  // get window size
#define WIOCSSIZE   _IOW( 'w',  6, short[2] )  // set window size
#define WIOCGDIM    _IOR( 'w',  7, short[2] )  // get window dimensions (in chars)
#define WIOCSDIM    _IOW( 'w',  8, short[2] )  // set window dimensions (in chars)
#define WIOCGVIS    _IOR( 'w',  9, int      )  // get window visibility
#define WIOCSVIS    _IOW( 'w', 10, int      )  // set window visibility


enum
{
	kLampSalvageWindowOnExitNever      = 0,
	kLampSalvageWindowOnExitForFailure = 1,
	kLampSalvageWindowOnExitForSignal  = 2,
	kLampSalvageWindowOnExitAlways     = 0xFFFFFFFFU
};

#define WIOCGEXIT  _IOR( 'w', 128, int )  // get close-on-exit behavior
#define WIOCSEXIT  _IOW( 'w', 129, int )  // set close-on-exit behavior

#endif

