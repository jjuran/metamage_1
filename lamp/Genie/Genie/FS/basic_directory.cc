/*
	basic_directory.cc
	------------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/FS/basic_directory.hh"

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	class basic_directory : public FSTree
	{
		private:
			Lookup_Proc   itsLookup;
			Iterate_Proc  itsIterate;
		
		public:
			basic_directory( const FSTreePtr&     parent,
			                 const plus::string&  name,
			                 Lookup_Proc          lookup,
			                 Iterate_Proc         iterate )
			:
				FSTree( parent, name, iterate ? S_IFDIR | 0500
				                              : S_IFDIR | 0100 ),
				itsLookup ( lookup  ),
				itsIterate( iterate )
			{
			}
			
			FSTreePtr Lookup_Child( const plus::string& name, const FSTree* parent ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	
	FSTreePtr basic_directory::Lookup_Child( const plus::string& name, const FSTree* parent ) const
	{
		return itsLookup( parent, name );
	}
	
	void basic_directory::IterateIntoCache( FSTreeCache& cache ) const
	{
		if ( itsIterate != NULL )
		{
			itsIterate( Self(), cache );
		}
	}
	
	
	FSTreePtr new_basic_directory( const FSTreePtr&     parent,
	                               const plus::string&  name,
	                               Lookup_Proc          lookup,
	                               Iterate_Proc         iterate )
	{
		return new basic_directory( parent, name, lookup, iterate );
	}
	
}

