/*
	iterate_resources.hh
	--------------------
*/

#ifndef MACVFS_UTIL_ITERATERESOURCES_HH
#define MACVFS_UTIL_ITERATERESOURCES_HH

typedef short OSErr;


namespace vfs
{
	
	class dir_contents;
	
	OSErr iterate_resources( vfs::dir_contents& cache );
	
}

#endif
