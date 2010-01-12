/*
	basic_directory.cc
	------------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/FS/basic_directory.hh"


namespace Genie
{
	
	class basic_directory : public FSTree_Directory
	{
		private:
			Lookup_Proc   itsLookup;
			Iterate_Proc  itsIterate;
		
		public:
			basic_directory( const FSTreePtr&    parent,
			                 const std::string&  name,
			                 Lookup_Proc         lookup,
			                 Iterate_Proc        iterate )
			:
				FSTree_Directory( parent, name ),
				itsLookup ( lookup  ),
				itsIterate( iterate )
			{
			}
			
			FSTreePtr Lookup_Child( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	
	FSTreePtr basic_directory::Lookup_Child( const std::string& name ) const
	{
		return itsLookup( Self(), name );
	}
	
	void basic_directory::IterateIntoCache( FSTreeCache& cache ) const
	{
		itsIterate( cache );
	}
	
	
	FSTreePtr new_basic_directory( const FSTreePtr&    parent,
	                               const std::string&  name,
	                               Lookup_Proc         lookup,
	                               Iterate_Proc        iterate )
	{
		return seize_ptr( new basic_directory( parent, name, lookup, iterate ) );
	}
	
}

