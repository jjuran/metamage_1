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

// relix
#include "relix/task/session.hh"

// Genie
#include "Genie/IO/IOPtr.hh"


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

