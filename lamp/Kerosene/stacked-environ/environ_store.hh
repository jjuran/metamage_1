/*
	environ_store.hh
	----------------
*/

#ifndef KEROSENE_ENVIRONSTORE_HH
#define KEROSENE_ENVIRONSTORE_HH

namespace _relix_libc
{
	
	bool load_environ();
	
	char* environ_get( const char* name );
	char* environ_set( const char* name, const char* value, bool overwriting );
	char* environ_put( char* string );
	void environ_unset( const char* name );
	void environ_clear();
	
}

#endif
