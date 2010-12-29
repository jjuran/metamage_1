/*	================
 *	DynamicGroups.hh
 *	================
 */

#ifndef GENIE_FILESYSTEM_DYNAMICGROUPS_HH
#define GENIE_FILESYSTEM_DYNAMICGROUPS_HH

// POSIX
#include <sys/stat.h>

// Genie 
#include "Genie/FS/FSTree.hh"
#include "Genie/IO/DynamicGroup.hh"


namespace Genie
{
	
	typedef boost::shared_ptr< IOHandle > (*DynamicElementGetter)( std::size_t );
	
	
	class FSTree_Dynamic_N : public FSTree
	{
		private:
			DynamicElementGetter itsGetter;
		
		public:
			FSTree_Dynamic_N( const FSTreePtr&      parent,
			                  const plus::string&   name,
			                  DynamicElementGetter  getter );
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	class FSTree_DynamicGroup_Base : public FSTree
	{
		public:
			typedef DynamicGroup Sequence;
			
			FSTree_DynamicGroup_Base( const FSTreePtr&     parent,
			                          const plus::string&  name )
			:
				FSTree( parent, name, S_IFDIR | 0700 )
			{
			}
			
			virtual const Sequence& ItemSequence() const = 0;
			
			virtual DynamicElementGetter Getter() const = 0;
			
			FSTreePtr Lookup_Child( const plus::string& name, const FSTree* parent ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	template < class Handle >
	class FSTree_DynamicGroup : public FSTree_DynamicGroup_Base
	{
		public:
			FSTree_DynamicGroup( const FSTreePtr&     parent,
			                     const plus::string&  name )
			:
				FSTree_DynamicGroup_Base( parent, name )
			{
			}
			
			const Sequence& ItemSequence() const
			{
				return GetDynamicGroup< Handle >();
			}
			
			DynamicElementGetter Getter() const
			{
				return &GetDynamicElementByID< Handle >;
			}
	};
	
}

#endif

