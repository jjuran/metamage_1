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


namespace POSeven
{
	
	typedef DIR* DirHandle;
	
}

namespace Nucleus
{
	
	template <> struct Disposer< POSeven::DirHandle > : public std::unary_function< POSeven::DirHandle, void >//,
	                                                    //private POSeven::DefaultDestructionPOSIXResultPolicy
	{
		void operator()( POSeven::DirHandle dir ) const
		{
			//(void) Nitrogen::FileManagerErrorsRegistrationDependency();
			//HandleDestructionPOSIXResult( ::closedir( dir ) );
			::closedir( dir );
		}
	};
}

namespace POSeven
{
	
	inline Nucleus::Owned< DirHandle > OpenDir( const char* pathname )
	{
		DIR* handle = ::opendir( pathname );
		
		if ( handle == NULL )
		{
			ThrowErrno( errno );
		}
		
		return Nucleus::Owned< DirHandle >::Seize( handle );
	}
	
	inline Nucleus::Owned< DirHandle > OpenDir( const std::string& pathname )
	{
		return OpenDir( pathname.c_str() );
	}
	
	
	inline void CloseDir( Nucleus::Owned< DirHandle > )  {}
	
	
	inline const dirent& ReadDir( DirHandle dir )
	{
		const dirent* entry = ::readdir( dir );
		
		if ( entry == NULL )
		{
			ThrowErrno( errno );
		}
		
		return *entry;
	}
	
	
	class DirectoryContents_Container
	{
		private:
			std::string                   itsDirPathname;
			Nucleus::Shared< DirHandle >  itsDirHandle;
			
			// not implemented:
			DirectoryContents_Container& operator=( const DirectoryContents_Container& );
		
		public:
			typedef std::string  value_type;
			typedef unsigned     size_type;
			typedef int          difference_type;
			
			class const_iterator
			{
				friend class DirectoryContents_Container;
				
				public:
					typedef DirectoryContents_Container::size_type size_type;
					typedef DirectoryContents_Container::difference_type difference_type;
					typedef DirectoryContents_Container::value_type value_type;
					
					typedef const value_type& reference;
					typedef const value_type& const_reference;
					
					typedef const value_type *pointer;
					typedef const value_type *const_pointer;
					
					typedef std::forward_iterator_tag iterator_category;
					
				private:
					std::string                   itsDirPathname;
					Nucleus::Shared< DirHandle >  itsDirHandle;
					value_type                    value;
					bool                          done;
					
					void GetNextValue()
					{
						using namespace io::path_descent_operators;
						
						try
						{
							const dirent entry = ReadDir( itsDirHandle );
							
							value = itsDirPathname / entry.d_name;
						}
						catch ( const Errno& error )
						{
							if ( error != ENOENT )
							{
								throw;
							}
							
							done = true;
						}
					}
					
					const_iterator( const std::string&                   dirPathname,
					                const Nucleus::Shared< DirHandle >&  dirHandle ) : itsDirPathname( dirPathname ),
					                                                                   itsDirHandle( dirHandle ),
					                                                                   done( false )
					{
						GetNextValue();
						
						// skip . and ..
						GetNextValue();
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
			
			DirectoryContents_Container( const std::string& dirPathname ) : itsDirPathname( dirPathname ),
			                                                                itsDirHandle  ( POSeven::OpenDir( dirPathname ) )
			{
			}
			
			const_iterator begin() const                    { return const_iterator( itsDirPathname, itsDirHandle ); }
			const_iterator end() const                      { return const_iterator(                              ); }
			
	};
	
	
	inline DirectoryContents_Container DirectoryContents( const std::string& dir )
	{
		return DirectoryContents_Container( dir );
	}
	
}

namespace io
{
	
	template <> struct directory_contents_traits< std::string >
	{
		typedef POSeven::DirectoryContents_Container container_type;
	};
	
	inline POSeven::DirectoryContents_Container directory_contents( const std::string& dir )
	{
		return POSeven::DirectoryContents( dir );
	}
	
}

#endif

