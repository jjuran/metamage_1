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
			basic_directory( const FSTreePtr&     parent,
			                 const plus::string&  name,
			                 Lookup_Proc          lookup,
			                 Iterate_Proc         iterate )
			:
				FSTree_Directory( parent, name ),
				itsLookup ( lookup  ),
				itsIterate( iterate )
			{
			}
			
			mode_t FilePermMode() const;
			
			FSTreePtr Lookup_Child( const plus::string& name, const FSTree* parent ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	
	mode_t basic_directory::FilePermMode() const
	{
		const mode_t perm = itsIterate ? S_IRUSR | 0 | S_IXUSR
		                               :               S_IXUSR;
		
		return perm;
	}
	
	FSTreePtr basic_directory::Lookup_Child( const plus::string& name, const FSTree* parent ) const
	{
		return itsLookup( (parent ? parent : this)->Self(), name );
	}
	
	void basic_directory::IterateIntoCache( FSTreeCache& cache ) const
	{
		if ( itsIterate != NULL )
		{
			itsIterate( Self(), Name(), cache );
		}
	}
	
	
	FSTreePtr new_basic_directory( const FSTreePtr&     parent,
	                               const plus::string&  name,
	                               Lookup_Proc          lookup,
	                               Iterate_Proc         iterate )
	{
		return seize_ptr( new basic_directory( parent, name, lookup, iterate ) );
	}
	
}

