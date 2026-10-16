/*
	settings.hh
	-----------
*/

#ifndef FREND_SETTINGS_HH
#define FREND_SETTINGS_HH


namespace frend
{

void load_settings( int dir_fd, const char* path );

inline
void load_defaults( int bindir_fd )
{
	const char* defaults_path = "../Resources/defaults.txt";
	
	load_settings( bindir_fd, defaults_path );
}

}

#endif
