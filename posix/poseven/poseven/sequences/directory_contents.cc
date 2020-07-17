// directory_contents.cc
// ---------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007-2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.

#include "poseven/sequences/directory_contents.hh"

// poseven
#include "poseven/functions/readdir.hh"


namespace poseven
{
	
	directory_contents_container::directory_contents_container( nucleus::owned< dir_t > dir )
	:
		itsDirHandle( dir )
	{
	}
	
	directory_contents_container::directory_contents_container( const directory_contents_container& other )
	:
		itsDirHandle( other.itsDirHandle )
	{
	}
	
	directory_contents_container::~directory_contents_container()
	{
	}
	
	static inline bool name_is_dots( const char* name )
	{
		return name[0] == '.'  &&  ( name[ 1 + (name[1] == '.') ] == '\0' );
	}
	
	void directory_contents_container::const_iterator::GetNextValue()
	{
		dir_t dir = itsDirHandle;
		
		const dirent* entry;
		
		while ( (entry = ::readdir( dir ))  &&  name_is_dots( entry->d_name ) )
		{
			continue;
		}
		
		if ( entry )
		{
			value = entry->d_name;
			
			return;
		}
		
		value = NULL;
		
		if ( errno != ENOENT )
		{
			throw_errno( errno );
		}
	}
	
}
