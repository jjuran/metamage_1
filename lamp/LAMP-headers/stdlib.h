// stdlib.h

#ifndef KEROSENE_STDLIB_H
#define KEROSENE_STDLIB_H

#ifdef __cplusplus
extern "C" {
#endif
	
	void exit( int status );
	
	int system( const char* command );
	
	char* getenv( const char* name );
	
	int setenv( const char* name, const char* value, int overwrite );
	
	int resetenv( const char* name, const char* value );
	
	int putenv( const char* string );
	
	void unsetenv( const char* name );
	
#ifdef __cplusplus
}
#endif

#endif

