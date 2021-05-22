/*
	environ_store.hh
	----------------
*/

#ifndef KEROSENE_ENVIRONSTORE_HH
#define KEROSENE_ENVIRONSTORE_HH

namespace _relix_libc
{
	
	class environ_store
	{
		public:
			environ_store( char** envp );
			
			static char* get( const char* name );
			static void set( const char* name, const char* value, bool overwriting );
			static void put( char* string );
			static void unset( const char* name );
			static void clear();
	};
	
}

#endif
