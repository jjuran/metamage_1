/*	===============
 *	ProcessGroup.hh
 *	===============
 */

#ifndef GENIE_PROCESSGROUP_HH
#define GENIE_PROCESSGROUP_HH

// Debug
#include "debug/boost_assert.hh"

// plus
#include "plus/ref_count.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// vfs
#include "vfs/filehandle_ptr.hh"

// Genie
#include "Genie/IO/IOPtr.hh"


namespace relix
{
	
	class session : public plus::ref_count< session >
	{
		private:
			int its_id;
			
			vfs::filehandle_ptr its_terminal;
		
		public:
			session();
			
			session( int id );
			
			~session();
			
			int id() const  { return its_id; }
			
			const vfs::filehandle_ptr& get_ctty() const  { return its_terminal; }
			
			void set_ctty( vfs::filehandle& terminal );
	};
	
}

namespace Genie
{
	
	typedef relix::session Session;
	
	class ProcessGroup : public plus::ref_count< ProcessGroup >
	{
		private:
			int                              itsID;
			boost::intrusive_ptr< Session >  itsSession;
		
		public:
			ProcessGroup()  {}
			
			ProcessGroup( int id, Session& session )
			:
				itsID( id ),
				itsSession( &session )
			{
			}
			
			~ProcessGroup();
			
			int ID() const  { return itsID; }
			
			int GetSID() const  { return itsSession->id(); }
			
			Session& GetSession() const  { return *itsSession; }
	};
	
	boost::intrusive_ptr< Session > NewSession( pid_t sid );
	
}

#endif

