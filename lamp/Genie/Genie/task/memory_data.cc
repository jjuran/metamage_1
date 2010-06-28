/*
	memory_data.cc
	--------------
	
	Copyright 2010, Joshua Juran
*/

#include "Genie/task/memory_data.hh"

// Standard C++
#include <algorithm>
#include <vector>

// debug
#include "debug/assert.hh"
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// plus
#include "plus/var_string.hh"

// Genie
#include "Genie/mmap/memory_mapping.hh"
#include "Genie/Utilities/simple_map.hh"


namespace Genie
{
	
	static void assign_flattened_argv( plus::var_string& result, char const *const *argv )
	{
		result.clear();
		
		// Check for NULL environ
		
		if ( argv == NULL )
		{
			return;
		}
		
		while ( *argv )
		{
			const char* p = *argv++;
			
			result.append( p, std::strlen( p ) + 1 );  // include trailing NUL
		}
	}
	
	
	struct program_parameters
	{
		plus::var_string its_cmdline;
		plus::var_string its_environ;
		
		std::vector< char* > its_argv;
		std::vector< char* > its_envp;
	};
	
	static void swap( program_parameters& a, program_parameters& b )
	{
		a.its_cmdline.swap( b.its_cmdline );
		a.its_environ.swap( b.its_environ );
		
		std::swap( a.its_argv, b.its_argv );
		std::swap( a.its_envp, b.its_envp );
	}
	
	class memory_data_impl : public memory_data
	{
		public:
			typedef simple_map< addr_t, boost::intrusive_ptr< const memory_mapping > > mmap_map;
			
			program_parameters  its_parameters;
			mmap_map            its_memory_mappings;
		
		public:
			memory_data_impl()
			{
			}
			
			memory_data_impl( const memory_data_impl& x )
			{
				/*
					Don't copy its_parameters -- it exists only to store data
					that argv and initial environ point to.
					
					For vfork(), environ is reallocated in userspace upon
					modification, and argv is simply not handled -- modifying
					argv in a vforked child modifies it in the parent -- which
					is POSIX-conformant since anything between vfork() and exec
					is undefined behavior.
					
					For fork(), we'll need to multiplex all memory belonging to
					the process, including argv/envp pointers and string data
					and private mmap regions.
				*/
			}
			
			void swap( memory_data_impl& other );
	};
	
	void memory_data_impl::swap( memory_data_impl& other )
	{
		Genie::swap( its_parameters,      other.its_parameters      );
		Genie::swap( its_memory_mappings, other.its_memory_mappings );
	}
	
	inline void swap( memory_data_impl& a, memory_data_impl& b )
	{
		a.swap( b );
	}
	
	
	void memory_data::swap( memory_data& other )
	{
		Genie::swap( static_cast< memory_data_impl& >( *this ),
		             static_cast< memory_data_impl& >( other ) );
	}
	
	memory_data* memory_data::create()
	{
		return new memory_data_impl();
	}
	
	memory_data* duplicate( const memory_data& x )
	{
		return new memory_data_impl( static_cast< const memory_data_impl& >( x ) );
	}
	
	void destroy( const memory_data* x )
	{
		delete static_cast< const memory_data_impl* >( x );
	}
	
	static inline const memory_data_impl* impl_cast( const memory_data* x )
	{
		return static_cast< const memory_data_impl* >( x );
	}
	
	static inline memory_data_impl* impl_cast( memory_data* x )
	{
		return static_cast< memory_data_impl* >( x );
	}
	
	const plus::string& memory_data::get_cmdline() const
	{
		return impl_cast( this )->its_parameters.its_cmdline;
	}
	
	
	static void assign_unflattened_argv( std::vector< char* >& result, plus::var_string& flat )
	{
		result.clear();
		
		char* begin = &*flat.begin();
		char* end   = &*flat.end();
		
		while ( begin < end )
		{
			char* null = std::find( begin, end, '\0' );
			
			ASSERT( null != end );
			
			result.push_back( begin );
			
			begin = null + 1;
		}
		
		result.push_back( NULL );
	}
	
	
	void memory_data::set_argv( const char *const *argv )
	{
		plus::var_string& cmdline = impl_cast( this )->its_parameters.its_cmdline;
		
		assign_flattened_argv( cmdline, argv );
		
		assign_unflattened_argv( impl_cast( this )->its_parameters.its_argv, cmdline );
	}
	
	void memory_data::set_envp( const char *const *envp )
	{
		plus::var_string& flatenv = impl_cast( this )->its_parameters.its_environ;
		
		assign_flattened_argv( flatenv, envp );
		
		assign_unflattened_argv( impl_cast( this )->its_parameters.its_envp, flatenv );
	}
	
	int memory_data::get_argc() const
	{
		return impl_cast( this )->its_parameters.its_argv.size() - 1;  // don't count trailing NULL
	}
	
	char** memory_data::get_argv()
	{
		return &impl_cast( this )->its_parameters.its_argv[ 0 ];
	}
	
	char** memory_data::get_envp()
	{
		return &impl_cast( this )->its_parameters.its_envp[ 0 ];
	}
	
	void* memory_data::add_memory_mapping( const memory_mapping* mapping )
	{
		ASSERT( mapping != NULL );
		
		void* key = mapping->get_address();
		
		impl_cast( this )->its_memory_mappings[ key ] = mapping;
		
		return key;
	}
	
	void memory_data::remove_memory_mapping( addr_t key )
	{
		impl_cast( this )->its_memory_mappings.erase( key );
	}
	
	void memory_data::clear_memory_mappings()
	{
		impl_cast( this )->its_memory_mappings.clear();
	}
	
}

