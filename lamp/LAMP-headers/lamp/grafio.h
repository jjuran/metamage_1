#ifndef	LAMP_GRAFIO_H
#define	LAMP_GRAFIO_H

// POSIX
#include <sys/ioccom.h>


#define GIOCGBASEADDR  _IOR( 'g',  1, void*             )  // get image buffer base address
#define GIOCGROWBYTES  _IOW( 'g',  2, unsigned short    )  // get image buffer bytes per row
#define GIOCGNROWS     _IOR( 'g',  3, unsigned short    )  // get image buffer row count
#define GIOCGDEPTH     _IOR( 'g',  4, unsigned char     )  // get image buffer bit depth
#define GIOCSDEPTH     _IOW( 'g',  5, unsigned char     )  // set image buffer bit depth
#define GIOCFLUSHRECT  _IOW( 'g',  6, short[4]          )  // flush buffer to screen
#define GIOCSBGCOLOR   _IOW( 'g',  7, unsigned short[3] )  // set background color


#endif

