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


//
	
	// This exercise is necessary because we need to pass the string with
	// different constness depending on whether we're setting or putting.
	// With setenv(), we construct a new std::string on which we call c_str(),
	// which returns const char*.  With putenv(), we're using the actual string,
	// and storing it as char*.  Type-safety for the win.
	
	template < bool putting > struct overwrite_traits;
	
	template <> struct overwrite_traits< false >
	{
		static const bool user_owned = false;
		
		typedef const char* param_type;
		
		static char* new_storage( param_type, std::size_t length )
		{
			return new char[ length + 1 ];
		}
	};
	
	template <> struct overwrite_traits< true >
	{
		static const bool user_owned = true;
		
		typedef char* param_type;
		
		static char* new_storage( param_type string, std::size_t )
		{
			return string;
		}
	};
	
	
	class environ_store
	{
		private:
			std::vector< char* >     itsVars;
			std::set< const char* >  itsUserOwnedVars;
			environ_store*           itsNext;
			
		private:
			// Non-copyable
			environ_store           ( const environ_store& );
			environ_store& operator=( const environ_store& );
			
		private:
			void update_environ();
			void preallocate();
			
			template < bool putting >
			void overwrite( std::vector< char* >::iterator                    it,
	                        typename overwrite_traits< putting >::param_type  string,
	                        std::size_t                                       new_len );
			
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
	
#endif

