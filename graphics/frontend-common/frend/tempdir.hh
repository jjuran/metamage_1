/*
	tempdir.hh
	----------
*/

#ifndef FREND_TEMPDIR_HH
#define FREND_TEMPDIR_HH

namespace frend
{

const char* chewable_items_path();

const char* tmp_dir_path();

inline
const char* tempdir_path()
{
#ifdef __APPLE__
	
	return chewable_items_path();
	
#endif
	
	return tmp_dir_path();
}

}

#endif
