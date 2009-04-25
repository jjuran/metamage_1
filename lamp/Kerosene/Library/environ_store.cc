/*	================
 *	environ_store.cc
 *	================
 */

#include "environ_store.hh"

// Standard C
#include <assert.h>

// Standard C/C++
#include <cstring>

// Standard C++
#include <string>


iota::environ_t environ = NULL;


namespace kerosene
{
	
	inline char* copy_string( const char* s )
	{
		std::size_t len = std::strlen( s );
		
		char* result = new char[ len + 1 ];
		
		std::copy( s, s + len + 1, result );
		
		return result;
	}
	
	static void DeleteVars( std::vector< char* >& result )
	{
		for ( int i = result.size() - 1;  i >= 0;  --i )
		{
			delete [] result[ i ];
		}
	}
	
	static void CopyVars( char const *const *vars, std::vector< char* >& result  )
	{
		try
		{
			if ( vars != NULL )
			{
				while ( *vars )
				{
					// This ensures push_back() won't (fail to) allocate memory
					result.reserve( result.size() + 1 );
					
					result.push_back( copy_string( *vars++ ) );
				}
			}
			
			result.push_back( NULL );
		}
		catch ( ... )
		{
			DeleteVars( result );
			
			throw;
		}
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
		return var_match( (char*) var, name );
	}
	
	static inline std::string MakeVar( const std::string& name, const char* value )
	{
		return value != NULL ? name + "=" + value : name;
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
	
	static std::vector< char* >::iterator FindVar( std::vector< char* >& vars, const char* name )
	{
		return std::lower_bound( vars.begin(),
		                         vars.end() - 1,
		                         name,
		                         std::ptr_fun( var_less ) );
	}
	
	
	environ_store* environ_store::pop( environ_store* top )
	{
		assert( top != NULL );
		
		environ_store* next = top->itsNext;
		
		assert( next != NULL );
		
		next->update_environ();
		
		delete top;
		
		return next;
	}
	
	environ_store::environ_store( environ_store* next, iota::environ_t envp )
	:
		itsNext( next )
	{
		CopyVars( envp, itsVars );
		
		update_environ();
	}
	
	environ_store::~environ_store()
	{
		clear();
	}
	
	void environ_store::update_environ()
	{
		environ = &itsVars.front();
	}
	
	void environ_store::preallocate()
	{
		// We reserve an extra slot so we can later insert without allocating memory, which
		// (a) could fail and throw bad_alloc, or
		// (b) could succeed and invalidate iterators.
		
		itsVars.reserve( itsVars.size() + 1 );
		
		update_environ();
	}
	
	template < bool putting >
	void environ_store::overwrite( std::vector< char* >::iterator                    it,
	                               typename overwrite_traits< putting >::param_type  string,
	                               std::size_t                                       new_len )
	{
		typedef overwrite_traits< putting > traits;
		
		// true for putenv(), false for setenv(), known at compile time.
		const bool new_is_user_owned = traits::user_owned;
		
		char* var = *it;
		
		std::size_t old_len = std::strlen( var );
		
		std::set< const char* >::iterator user_ownership = itsUserOwnedVars.find( var );
		
		// true for putenv(), false for setenv(), known at runtime.
		bool old_is_user_owned = user_ownership != itsUserOwnedVars.end();
		
		// If neither var string is user-owned and the lengths are the same,
		// it's a straight copy -- no memory management is required.
		// User-owned var strings don't get allocated or deallocated here,
		// but instead we have to mark them so we don't delete them later.
		
		if ( old_is_user_owned  ||  new_is_user_owned  ||  new_len != old_len )
		{
			if ( new_is_user_owned )
			{
				itsUserOwnedVars.insert( string );  // may throw
			}
			
			*it = traits::new_storage( string, new_len );
			
			if ( old_is_user_owned )
			{
				itsUserOwnedVars.erase( user_ownership );
			}
			else
			{
				delete [] var;
			}
			
			var = *it;
		}
		
		if ( !new_is_user_owned )
		{
			std::copy( string, string + new_len + 1, var );
		}
	}
	
	void environ_store::reset_user_owned()
	{
		// Here we zero out user-owned var string storage.  This is a convenience
		// that allows us to subsequently call DeleteVars() safely without
		// giving it a dependency on the user ownership structure.
		
		for ( std::vector< char* >::iterator it = itsVars.begin();  it != itsVars.end();  ++it )
		{
			std::set< const char* >::iterator user_ownership = itsUserOwnedVars.find( *it );
			
			if ( user_ownership != itsUserOwnedVars.end() )
			{
				//itsUserOwnedVars.erase( user_ownership );
				
				*it = NULL;
			}
		}
		
		itsUserOwnedVars.clear();
	}
	
	char* environ_store::get( const char* name )
	{
		std::vector< char* >::iterator it = FindVar( itsVars, name );
		
		char* var = *it;
		
		return var_match( var, name );
	}
	
	void environ_store::set( const char* name, const char* value, bool overwriting )
	{
		preallocate();  // make insertion safe
		
		std::vector< char* >::iterator it = FindVar( itsVars, name );
		
		const char* var = *it;
		
		// Did we find the right environment variable?
		const bool match = var_match( var, name );
		
		// If it doesn't match, we insert (otherwise, we possibly overwrite)
		bool inserting = !match;
		
		if ( inserting )
		{
			std::size_t name_length  = std::strlen( name  );
			std::size_t value_length = std::strlen( value );
			
			std::size_t total_length = name_length + 1 + value_length;
			
			char* new_string = new char[ total_length + 1 ];
			
			std::memcpy( new_string, name, name_length );
			
			new_string[ name_length ] = '=';
			
			std::memcpy( new_string + name_length + 1, value, value_length + 1 );
			
			itsVars.insert( it, new_string );  // won't throw
		}
		else if ( overwriting )
		{
			std::string new_var = MakeVar( name, value );
			
			overwrite< false >( it, new_var.c_str(), new_var.length() );
		}
	}
	
	void environ_store::put( char* string )
	{
		preallocate();  // make insertion safe
		
		std::vector< char* >::iterator it = FindVar( itsVars, string );
		
		const char* var = *it;
		
		// Did we find the right environment variable?
		const bool match = var_match( var, string );
		
		// If it doesn't match, we insert (otherwise, we possibly overwrite)
		bool inserting = !match;
		
		if ( inserting )
		{
			itsUserOwnedVars.insert( string );  // may throw
			
			itsVars.insert( it, string );  // memory already reserved
		}
		else
		{
			std::size_t length = std::strlen( string );
			
			overwrite< true >( it, string, length );
		}
	}
	
	void environ_store::unset( const char* name )
	{
		std::vector< char* >::iterator it = FindVar( itsVars, name );
		
		const char* var = *it;
		
		// Did we find the right environment variable?
		const bool match = var_match( var, name );
		
		
		if ( match )
		{
			std::set< const char* >::iterator user_ownership = itsUserOwnedVars.find( var );
			
			bool user_owned = user_ownership != itsUserOwnedVars.end();
			
			if ( user_owned )
			{
				itsUserOwnedVars.erase( user_ownership );
			}
			
			itsVars.erase( it );
			
			delete [] var;
		}
	}
	
	void environ_store::clear()
	{
		// Zero out user-owned memory so we don't try to delete it.
		reset_user_owned();
		
		DeleteVars( itsVars );
		
		itsVars.clear();
		
		itsVars.resize( 1, NULL );
		
		environ = NULL;
	}
	
}

