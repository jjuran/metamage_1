/*	================
 *	environ_store.hh
 *	================
 */

#ifndef KEROSENE_ENVIRONSTORE_HH
#define KEROSENE_ENVIRONSTORE_HH

// Standard C++
#include <set>
#include <vector>

// Iota
#include "iota/environ.hh"


namespace kerosene
{
	
	class environ_store
	{
		private:
			std::vector< char* >     its_vars;
			std::set< const char* >  its_user_owned_vars;
			environ_store*           its_next;
			
		private:
			// Non-copyable
			environ_store           ( const environ_store& );
			environ_store& operator=( const environ_store& );
			
		private:
			void update_environ();
			void preallocate();
			
			template < bool putting >
			void overwrite( std::vector< char* >::iterator  it,
	                        char                           *string,
	                        std::size_t                     new_len );
			
			void reset_user_owned();
		
		public:
			environ_store( environ_store* next, iota::environ_t envp );
			
			~environ_store();
			
			char* get( const char* name );
			void set( const char* name, const char* value, bool overwriting );
			void put( char* string );
			void unset( const char* name );
			void clear();
			
			static environ_store* pop( environ_store* top );
	};
	
}

#endif

