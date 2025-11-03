/*
	raster_updating.hh
	------------------
*/

#ifndef FREND_RASTERUPDATING_HH
#define FREND_RASTERUPDATING_HH

// POSIX
#include <sys/stat.h>
#include <unistd.h>

// iota
#include "iota/class.hh"

// frontend-common
#include "frend/displayfs.hh"
#include "frend/update_fifo.hh"


namespace frend
{

class raster_updating
{
	NON_COPYABLE( raster_updating )
	
	public:
		raster_updating()
		{
			if ( CONFIG_UPDATES_VIA_FIFO )
			{
				mkfifo( update_fifo, 0666 );
			}
		}
		
		~raster_updating()
		{
			if ( CONFIG_UPDATES_VIA_FIFO )
			{
				unlink( update_fifo );
			}
		}
};

}

#endif
