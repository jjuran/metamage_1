/*	===================
 *	FSTree_Directory.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_DIRECTORY_HH
#define GENIE_FILESYSTEM_FSTREE_DIRECTORY_HH

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/FSTreeCache.hh"


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
	}
	
	class FSTree_Premapped : public FSTree
	{
		public:
			typedef premapped::function Function;
			typedef premapped::mapping  Mapping;
			
			static const Mapping empty_mappings[];
		
		private:
			typedef const Mapping* Mappings;
			
			typedef void (*Destructor)( const FSTree* );
			
			Destructor  itsDestructor;
			Mappings    itsMappings;
		
		public:
			FSTree_Premapped( const FSTreePtr&     parent,
			                  const plus::string&  name,
			                  Mappings             mappings = empty_mappings,
			                  Destructor           dtor     = NULL )
			:
				FSTree( parent, name, S_IFDIR | 0700 ),
				itsMappings( mappings ),
				itsDestructor( dtor )
			{
			}
			
			~FSTree_Premapped();
			
			void Delete() const;
			
			FSTreePtr Lookup_Child( const plus::string& name, const FSTree* parent ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	
	template < class FSTree_Type >
	FSTreePtr Basic_Factory( const FSTreePtr&     parent,
	                         const plus::string&  name,
	                         const void*          args )
	{
		return new FSTree_Type( parent, name );
	}
	
	FSTreePtr Premapped_Factory( const FSTreePtr&            parent,
	                             const plus::string&         name,
	                             const premapped::mapping    mappings[],
	                             void                      (*dtor)(const FSTree*) = NULL );
	
}

#endif

