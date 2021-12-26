/*
	node.hh
	-------
*/

#ifndef VFS_NODE_HH
#define VFS_NODE_HH

// POSIX
#include <sys/types.h>

// vxo
#include "vxo/ref_count.hh"

// plus
#include "plus/string.hh"


namespace vfs
{
	
	class node;
	
	typedef void (*node_destructor)( const node* );
	
	struct node_method_set;
	
	
	class node : public vxo::ref_count< node >
	{
		private:
			const node*             its_owner;
			plus::string            its_name;
			mode_t                  its_mode;
			uid_t                   its_user;
			const node_method_set*  its_methods;
			void*                   its_extra;
			node_destructor         its_destructor;
			
			// non-copyable
			node           ( const node& );
			node& operator=( const node& );
		
		public:
			node();
			
			node( const node*             owner,
			      const plus::string&     name,
			      mode_t                  mode,
			      const node_method_set*  methods = NULL,
			      std::size_t             n_extra = 0,
			      node_destructor         dtor    = NULL );
			
			node( const node*             owner,
			      const plus::string&     name,
			      mode_t                  mode,
			      uid_t                   user,
			      const node_method_set*  methods = NULL,
			      std::size_t             n_extra = 0,
			      node_destructor         dtor    = NULL );
			
			~node();
			
			const node* owner() const  { return its_owner; }
			
			const plus::string& name() const  { return its_name; }
			
			mode_t filemode() const  { return its_mode; }
			uid_t  user    () const  { return its_user; }
			
			const node_method_set* methods() const  { return its_methods; }
			
			void* extra() const  { return its_extra; }
			
			node_destructor destructor() const  { return its_destructor; }
	};
	
}

#endif
