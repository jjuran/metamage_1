// Directory.hh
// ------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007-2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_DIRECTORY_HH
#define POSEVEN_DIRECTORY_HH

// Io
#include "io/files.hh"
#include "io/walk.hh"

// Nucleus
#include "Nucleus/Shared.h"

// poseven
#include "poseven/functions/opendir.hh"


namespace poseven
{
	
	class directory_contents_container
	{
		private:
			Nucleus::Shared< dir_t >  itsDirHandle;
			
			// not implemented:
			directory_contents_container& operator=( const directory_contents_container& );
		
		public:
			typedef const char*  value_type;
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
					Nucleus::Shared< dir_t >  itsDirHandle;
					value_type                value;
					bool                      done;
					
					void GetNextValue();
					
					const_iterator( const Nucleus::Shared< dir_t >& dirHandle ) : itsDirHandle( dirHandle ),
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
			
			directory_contents_container( const Nucleus::Shared< dir_t >& dir ) : itsDirHandle( dir )
			{
			}
			
			const_iterator begin() const                    { return const_iterator( itsDirHandle ); }
			const_iterator end() const                      { return const_iterator(              ); }
			
	};
	
	
	inline directory_contents_container directory_contents( const Nucleus::Shared< dir_t >& dir )
	{
		return directory_contents_container( dir );
	}
	
	inline directory_contents_container directory_contents( const std::string& dir_path )
	{
		return directory_contents_container( Nucleus::Shared< dir_t >( opendir( dir_path ) ) );
	}
	
}

namespace io
{
	
	template <> struct directory_contents_traits< std::string >
	{
		typedef poseven::directory_contents_container container_type;
	};
	
	inline poseven::directory_contents_container directory_contents( const std::string& dir, overload = overload() )
	{
		return poseven::directory_contents( dir );
	}
	
}

#endif

