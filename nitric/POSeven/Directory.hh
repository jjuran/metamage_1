// Directory.hh
// ------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_DIRECTORY_HH
#define POSEVEN_DIRECTORY_HH

// Standard C++
#include <string>

// POSIX
#include <dirent.h>

// Io
#include "io/files.hh"
#include "io/walk.hh"

// Nucleus
#include "Nucleus/Shared.h"

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/Pathnames.hh"


namespace poseven
{
	
	typedef DIR* dir_t;
	
}

namespace Nucleus
{
	
	template <> struct Disposer< poseven::dir_t > : public std::unary_function< poseven::dir_t, void >//,
	                                                //private POSeven::DefaultDestructionPOSIXResultPolicy
	{
		void operator()( poseven::dir_t dir ) const
		{
			//(void) Nitrogen::FileManagerErrorsRegistrationDependency();
			//HandleDestructionPOSIXResult( ::closedir( dir ) );
			::closedir( dir );
		}
	};
}

namespace poseven
{
	
	inline Nucleus::Owned< dir_t > opendir( const char* pathname )
	{
		DIR* handle = ::opendir( pathname );
		
		if ( handle == NULL )
		{
			throw_errno( errno );
		}
		
		return Nucleus::Owned< dir_t >::Seize( handle );
	}
	
	inline Nucleus::Owned< dir_t > opendir( const std::string& pathname )
	{
		return opendir( pathname.c_str() );
	}
	
	
	inline void closedir( Nucleus::Owned< dir_t > )  {}
	
	
	inline const dirent& readdir( dir_t dir )
	{
		const dirent* entry = ::readdir( dir );
		
		if ( entry == NULL )
		{
			throw_errno( errno );
		}
		
		return *entry;
	}
	
	
	class directory_contents_container
	{
		private:
			std::string               itsDirPathname;
			Nucleus::Shared< dir_t >  itsDirHandle;
			
			// not implemented:
			directory_contents_container& operator=( const directory_contents_container& );
		
		public:
			typedef std::string  value_type;
			typedef unsigned     size_type;
			typedef int          difference_type;
			
			class const_iterator
			{
				friend class directory_contents_container;
				
				public:
					typedef directory_contents_container::size_type size_type;
					typedef directory_contents_container::difference_type difference_type;
					typedef directory_contents_container::value_type value_type;
					
					typedef const value_type& reference;
					typedef const value_type& const_reference;
					
					typedef const value_type *pointer;
					typedef const value_type *const_pointer;
					
					typedef std::forward_iterator_tag iterator_category;
					
				private:
					std::string               itsDirPathname;
					Nucleus::Shared< dir_t >  itsDirHandle;
					value_type                value;
					bool                      done;
					
					void GetNextValue()
					{
						using namespace io::path_descent_operators;
						
						try
						{
							dirent entry;
							
							do
							{
								entry = poseven::readdir( itsDirHandle );
							}
							while ( entry.d_name[0] == '.'  &&  ( entry.d_name[ 1 + (entry.d_name[1] == '.') ] == '\0' ) );
							
							value = itsDirPathname / entry.d_name;
						}
						catch ( const errno_t& error )
						{
							if ( error != ENOENT )
							{
								throw;
							}
							
							done = true;
						}
					}
					
					const_iterator( const std::string&               dirPathname,
					                const Nucleus::Shared< dir_t >&  dirHandle ) : itsDirPathname( dirPathname ),
					                                                               itsDirHandle( dirHandle ),
					                                                               done( false )
					{
						GetNextValue();
					}
				
				public:
					const_iterator() : done( true )
					{
					}
					
					const_iterator& operator++()              { GetNextValue();  return *this; }
					const_iterator operator++(int)            { const_iterator old = *this; operator++(); return old; }
					
					reference operator*() const               { return value; }
					pointer operator->() const                { return &value; }
					
					friend bool operator==( const const_iterator& a, const const_iterator& b )    { return a.done == b.done; }
					friend bool operator!=( const const_iterator& a, const const_iterator& b )    { return !( a == b ); }
			};
			
			directory_contents_container( const std::string& dirPathname ) : itsDirPathname( dirPathname ),
			                                                                 itsDirHandle  ( poseven::opendir( dirPathname ) )
			{
			}
			
			const_iterator begin() const                    { return const_iterator( itsDirPathname, itsDirHandle ); }
			const_iterator end() const                      { return const_iterator(                              ); }
			
	};
	
	
	inline directory_contents_container directory_contents( const std::string& dir )
	{
		return directory_contents_container( dir );
	}
	
}

namespace io
{
	
	template <> struct directory_contents_traits< std::string >
	{
		typedef poseven::directory_contents_container container_type;
	};
	
	using poseven::directory_contents;
	
}

#endif

