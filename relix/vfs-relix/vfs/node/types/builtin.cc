/*
	builtin.hh
	----------
*/

#include "vfs/node/types/builtin.hh"

// POSIX
#include <sys/stat.h>

// vfs
#include "vfs/node.hh"
#include "vfs/program_ptr.hh"
#include "vfs/methods/file_method_set.hh"
#include "vfs/methods/node_method_set.hh"

// vfs-relix
#include "vfs/program.hh"


namespace vfs
{
	
	class fixed_address : public program
	{
		private:
			relix_entry its_entry_point;
		
		public:
			fixed_address( relix_entry entry ) : its_entry_point( entry )
			{
			}
			
			relix_entry get_main_entry_point() const  { return its_entry_point; }
	};
	
	
	static program_ptr builtin_loadexec( const node* that )
	{
		relix_entry& extra = *(relix_entry*) that->extra();
		
		return new fixed_address( extra );
	}
	
	static const file_method_set builtin_file_methods =
	{
		NULL,
		NULL,
		NULL,
		&builtin_loadexec
	};
	
	static const node_method_set builtin_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&builtin_file_methods
	};
	
	
	node_ptr builtin_factory( const node*          parent,
	                          const plus::string&  name,
	                          const void*          args )
	{
		node* result = new node( parent,
		                         name,
		                         S_IFREG | 0500,
		                         &builtin_methods,
		                         sizeof (relix_entry) );
		
		relix_entry& extra = *(relix_entry*) result->extra();
		
		extra = (relix_entry) args;
		
		return result;
	}
	
}

