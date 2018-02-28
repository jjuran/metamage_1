/*
	config/resfs.hh
	---------------
*/

#ifndef GENIE_CONFIG_RESFS_HH
#define GENIE_CONFIG_RESFS_HH

// relix-kernel
#include "relix/config/mini.hh"


#ifndef CONFIG_RESFS
#define CONFIG_RESFS  (! CONFIG_MINI)
#endif

#endif
