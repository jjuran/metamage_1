/*	=========
 *	FSTree.hh
 *	=========
 */

#ifndef GENIE_FILESYSTEM_FSTREE_HH
#define GENIE_FILESYSTEM_FSTREE_HH

// POSIX
#include <sys/types.h>

// plus
#include "plus/ref_count.hh"
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	typedef void (*node_destructor)( const FSTree* );
	
	struct node_method_set;
	
	
	extern const FSTreePtr null_FSTreePtr;
	
	const FSTreePtr& FSRoot();
	
	
	class FSTree : public plus::ref_count< FSTree >
	{
		private:
			FSTreePtr     itsParent;
			plus::string  itsName;
			mode_t        itsMode;
			
			const node_method_set*  its_methods;
			
			void*  its_extra;
			
			node_destructor  its_destructor;
			
		private:
			// non-copyable
			FSTree           ( const FSTree& );
			FSTree& operator=( const FSTree& );
		
		public:
			FSTree();
			
			FSTree( const FSTreePtr&        parent,
			        const plus::string&     name,
			        mode_t                  mode,
			        const node_method_set*  methods = NULL,
			        std::size_t             n_extra = 0,
			        node_destructor         dtor    = NULL );
			
			~FSTree();
			
			// General methods
			
			const FSTree* owner() const  { return itsParent.get(); }
			
			const plus::string& name() const  { return itsName; }
			const plus::string& Name() const  { return itsName; }
			
			mode_t FileMode() const  { return itsMode; }
			
			const node_method_set* methods() const  { return its_methods; }
			
			void* extra() const  { return its_extra; }
			
			node_destructor destructor() const  { return its_destructor; }
	};
	
}

#endif

