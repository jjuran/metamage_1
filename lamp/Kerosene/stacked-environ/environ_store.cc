/*
	environ_store.cc
	----------------
*/

#include "environ_store.hh"

// Standard C
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Standard C++
#include <algorithm>

// more-libc
#include "more/string.h"

// vxo
#include "vxo/ptrvec.hh"


#pragma exceptions off


typedef vxo::UPtrVec< char > CStrVec;

char** environ = NULL;

static char*        its_buffer;
static std::size_t  its_length;
static CStrVec      its_vars;


namespace _relix_libc
{

static inline uintptr_t max( uintptr_t a, uintptr_t b )
{
	return a > b ? a : b;
}

static
size_t sizeof_argv( char** argv )
{
	size_t size = 0;
	
	while ( *argv != NULL )
	{
		size += strlen( *argv++ ) + 1;
	}
	
	return size;
}

static bool cstr_less( const char* a, const char* b )
{
	return strcmp( a, b ) < 0;
}


// var is either NULL or of the form "name=oldvalue"
// name may be either "name" or "name=newvalue"
// Returns pointer to "oldvalue" on match or NULL otherwise.

static char* var_match( char* var, const char* name )
{
	if ( var )
	{
		for ( ;  ;  ++var, ++name )
		{
			const char c = *name != '\0' ? *name : '=';
			
			if ( *var != c )
			{
				break;
			}
			
			if ( c == '=' )
			{
				return var + 1;
			}
		}
	}
	
	return NULL;
}

static inline const char* var_match( const char* var, const char* name )
{
	return var_match( const_cast< char* >( var ), name );
}

static bool var_less( const char* var, const char* name )
{
	for ( ;  ;  ++var, ++name )
	{
		const char c = *name != '\0' ? *name : '=';
		
		if ( c == '=' )
		{
			// End of name.  var is longer or equal length, not less.
			return false;
		}
		
		if ( *var != c )
		{
			return *var < c  ||  *var == '=';
		}
	}
}

static
CStrVec::iterator find_var( CStrVec& vars, const char* name )
{
	return std::lower_bound( vars.begin(),
	                         vars.end() - 1,
	                         name,
	                         std::ptr_fun( var_less ) );
}


static
char* new_buffer( size_t size )
{
	char* buffer = (char*) malloc( size + 1 );
	
	if ( buffer )
	{
		memset( buffer, '\0', size );
		
		buffer[ size ] = '=';
	}
	
	return buffer;
}

static
bool ptr_within( void const* p, void const* block, size_t n )
{
	uintptr_t addr  = (uintptr_t) p;
	uintptr_t begin = (uintptr_t) block;
	uintptr_t end   = (uintptr_t) block + n;
	
	return addr >= begin  &&  addr < end;
}

static char* find_space( char* buffer, size_t n )
{
	/*
		The buffer's unused bytes contain zero.  The last byte of an
		environment string is also zero, so therefore the last byte of
		the buffer itself must be zero at all times.  The byte after that
		is valid memory containing '=', which never otherwise follows a
		zero byte.
		
		buffer may be NULL, but it may not be empty (i.e. a zero-length
		series of zero bytes, i.e. pointing at the trailing '=').
		
		n is the number of bytes needed, including trailing NUL.
	*/
	
	if ( buffer == NULL )
	{
		return NULL;
	}
	
	char* p = buffer;
	
	if ( *p == '\0' )
	{
		// Without this we couldn't reuse the first byte once zeroed.
		goto leading_NUL;
	}
	
	while ( true )
	{
		// This skips the trailing NUL of the previous entry.
		while ( *p++ != '\0' )  continue;
		
	leading_NUL:
		char* q = p;
		
		while ( *q++ == '\0' )  continue;
		
		/*
			q now points to the byte after the non-NUL after the last NUL.
			q is off by one, so q - 1 - p is the number of NUL bytes in
			this sequence.  It's enough if it's at least n, so we need
			q - 1 - p >= n, which is q - p > n.
		*/
		
		if ( q - p > n )
		{
			return p;
		}
		
		p = q;  // The non-NUL is already checked; try the next byte.
		
		if ( *--q == '=' )
		{
			// We reached the end.
			return NULL;
		}
	}
}

static
char* find_space_or_reallocate( size_t extra_space )
{
	if ( char* result = find_space( its_buffer, extra_space ) )
	{
		return result;
	}
	
	size_t size = max( its_length * 2, its_length + extra_space );
	
	char* buffer = new_buffer( size );
	
	if ( buffer == NULL )
	{
		return NULL;
	}
	
	char* p = buffer;
	
	for ( char** vars = &its_vars[0];  *vars != NULL;  ++vars )
	{
		const bool is_managed = ptr_within( *vars, its_buffer, its_length );
		
		if ( is_managed )
		{
			const char* var = *vars;
			
			*vars = p;
			
			p = (char*) mempcpy( p, var, strlen( var ) + 1 );
		}
	}
	
	free( its_buffer );
	
	its_buffer = buffer;
	its_length = size;
	
	return p;
}

static
void update_environ()
{
	environ = &its_vars.front();
}

bool load_environ()
{
	if ( its_buffer )
	{
		return true;
	}
	
	char** envp = environ;
	
	if ( envp != NULL  &&  *envp != NULL )
	{
		char** env = envp;
		
		while ( *env++ ) continue;
		
		const int envc = env - envp;  // var count + 1
		
		if ( ! its_vars.expand_by_nothrow( envc ) )
		{
			return false;
		}
		
		its_vars.clear();
		
		its_length = sizeof_argv( envp ) * 2;  // leave room for more vars
		
		its_buffer = new_buffer( its_length );
		
		if ( its_buffer == NULL )
		{
			return false;
		}
		
		char* p = its_buffer;
		
		while ( const char* var = *envp++ )
		{
			its_vars.push_back_nothrow( p );  // already expanded above
			
			p = (char*) mempcpy( p, var, strlen( var ) + 1 );
		}
		
		std::sort( its_vars.begin(),
		           its_vars.end(),
		           std::ptr_fun( &cstr_less ) );
	}
	
	if ( ! its_vars.push_back_nothrow( NULL ) )
	{
		return false;
	}
	
	update_environ();
	
	return true;
}

static
const void* preallocate()
{
	// We reserve an extra slot so we can later insert without allocating memory, which
	// (a) could fail and throw bad_alloc, or
	// (b) could succeed and invalidate iterators.
	
	const void* ptr = its_vars.expand_by_nothrow( 1 );
	
	if ( ptr != NULL )
	{
		its_vars.pop_back();
		
		update_environ();
	}
	
	return ptr;
}

static
void erase( char* var )
{
	if ( ptr_within( var, its_buffer, its_length ) )
	{
		memset( var, '\0', strlen( var ) );
	}
}

char* environ_get( const char* name )
{
	CStrVec::iterator it = find_var( its_vars, name );
	
	char *const var = *it;
	
	return var_match( var, name );
}

char* environ_set( const char* name, const char* value, bool overwriting )
{
	if ( ! preallocate() )
	{
		return NULL;
	}
	
	// Insertion is now safe
	
	CStrVec::iterator it = find_var( its_vars, name );
	
	char *const var = *it;
	
	// Did we find the right environment variable?
	const char *const match = var_match( var, name );
	
	// If it doesn't match, we insert (otherwise, we possibly overwrite)
	const bool inserting = !match;
	
	size_t name_len;
	
	if ( ! inserting )
	{
		if ( ! overwriting )
		{
			return var;
		}
		
		name_len = match - var - 1;
	}
	else
	{
		name_len = strlen( name );
	}
	
	const size_t value_len = strlen( value );
	
	const size_t var_len = name_len + 1 + value_len + 1;
	
	char* new_var = find_space_or_reallocate( var_len );
	
	if ( new_var == NULL )
	{
		return NULL;
	}
	
	char* p = new_var;
	
	if ( inserting )
	{
		if ( ! its_vars.insert_nothrow( it, new_var ) )
		{
			return NULL;
		}
	}
	else
	{
		*it = new_var;
		
		erase( var );
	}
	
	p = (char*) mempcpy( p, name,  name_len  );
	*p++ = '=';
	p = (char*) mempcpy( p, value, value_len );
	// *p is already NUL
	
	return new_var;
}

char* environ_put( char* string )
{
	if ( ! preallocate() )
	{
		return NULL;
	}
	
	// Insertion is now safe
	
	CStrVec::iterator it = find_var( its_vars, string );
	
	char *const var = *it;
	
	// Did we find the right environment variable?
	const char *const match = var_match( var, string );
	
	// If it doesn't match, we insert (otherwise, we possibly overwrite)
	const bool inserting = !match;
	
	if ( inserting )
	{
		if ( ! its_vars.insert_nothrow( it, string ) )
		{
			return NULL;
		}
	}
	else
	{
		*it = string;
		
		erase( var );
	}
	
	return string;
}

void environ_unset( const char* name )
{
	CStrVec::iterator it = find_var( its_vars, name );
	
	char *const var = *it;
	
	// Did we find the right environment variable?
	const bool match = var_match( var, name );
	
	if ( match )
	{
		erase( var );
		
		its_vars.erase( it );
	}
}

void environ_clear()
{
	its_vars.clear();
	
	environ = NULL;
}

}
