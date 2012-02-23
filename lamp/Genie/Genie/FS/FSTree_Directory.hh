/*	===================
 *	FSTree_Directory.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_DIRECTORY_HH
#define GENIE_FILESYSTEM_FSTREE_DIRECTORY_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	namespace premapped
	{
		
		typedef FSTreePtr (*function)( const FSTreePtr&,
		                               const plus::string&,
		                               const void* );
		
		struct mapping
		{
			const char*  name;
			function     f;
			const void*  args;
		};
		
		extern const mapping empty_mappings[];
		
	}
	
	
	template < class FSTree_Type >
	FSTreePtr Basic_Factory( const FSTreePtr&     parent,
	                         const plus::string&  name,
	                         const void*          args )
	{
		return new FSTree_Type( parent, name );
	}
	
	FSTreePtr Premapped_Factory( const FSTreePtr&            parent,
	                             const plus::string&         name,
	                             const premapped::mapping    mappings[] = premapped::empty_mappings,
	                             void                      (*dtor)(const FSTree*) = NULL );
	
}

#endif

