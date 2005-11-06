// stdlib.h

#ifndef KEROSENE_STDLIB_H
#define KEROSENE_STDLIB_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
	
	void abort( void );
	
	int atexit( void (*f)(void) );
	
	void exit( int status );
	
	void free( void* );
	
	void* malloc( size_t );
	
	void* realloc( void*, size_t );
	
	int system( const char* command );
	
	char* getenv( const char* name );
	
	int setenv( const char* name, const char* value, int overwrite );
	
	int putenv( const char* string );
	
	void unsetenv( const char* name );
	
	int clearenv();
	
#ifdef __cplusplus
}
#endif

#endif

