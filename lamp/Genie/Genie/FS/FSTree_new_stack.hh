/*	===================
 *	FSTree_new_stack.hh
 *	===================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_NEW_STACK_HH
#define GENIE_FILESYSTEM_FSTREE_NEW_STACK_HH

// Genie
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	boost::shared_ptr< Pedestal::View > StackFactory( const FSTree* delegate );
	
	
	class FSTree_new_stack : public FSTree_new_View
	{
		public:
			FSTree_new_stack( const FSTreePtr&     parent,
			                  const plus::string&  name )
			:
				FSTree_new_View( parent,
				                 name,
				                 &StackFactory,
				                 NULL,
				                 &DestroyDelegate )
			{
			}
			
			FSTreePtr CreateDelegate( const FSTreePtr&     parent,
			                          const plus::string&  name ) const;
			
			static void DestroyDelegate( const FSTree* delegate );
	};
	
}

#endif

