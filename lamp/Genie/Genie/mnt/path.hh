/*
	path.hh
	-------
*/

#ifndef GENIE_MNT_PATH_HH
#define GENIE_MNT_PATH_HH


#define FS_SUBPATH  "/var/run/fs"


namespace Genie
{
	
	const char* socket_path();
	const char* service_path();
	
}

#endif
