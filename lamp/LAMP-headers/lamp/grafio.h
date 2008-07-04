#ifndef	LAMP_GRAFIO_H
#define	LAMP_GRAFIO_H

// POSIX
#include <sys/ioccom.h>


#define GIOCGBASEADDR  _IOR( 'g',  1, void*          )  // get image buffer base address
#define GIOCGROWBYTES  _IOW( 'g',  2, unsigned short )  // get image buffer bytes per row
#define GIOCGNROWS     _IOR( 'g',  3, unsigned short )  // get image buffer row count
#define GIOCGDEPTH     _IOR( 'g',  4, unsigned char  )  // get image buffer bit depth


#endif

