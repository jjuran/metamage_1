/*
	session.hh
	----------
*/

#ifndef RELIX_TASK_SESSION_HH
#define RELIX_TASK_SESSION_HH

// vxo
#include "vxo/ref_count.hh"

// vfs
#include "vfs/filehandle_ptr.hh"


namespace relix
{
	
	class session : public vxo::ref_count< session >
	{
		private:
			int its_id;
			
			vfs::filehandle_ptr its_terminal;
		
		public:
			session( int id );
			
			~session();
			
			int id() const  { return its_id; }
			
			const vfs::filehandle_ptr& get_ctty() const  { return its_terminal; }
			
			void set_ctty( vfs::filehandle& terminal );
	};
	
}

#endif
