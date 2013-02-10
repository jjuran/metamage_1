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

// Genie
#include "Genie/IO/IOPtr.hh"


namespace Genie
{
	
	class Session : public plus::ref_count< Session >
	{
		private:
			int    itsID;
			IOPtr  itsControllingTerminal;
		
		public:
			Session();
			
			Session( int id );
			
			~Session();
			
			int ID() const  { return itsID; }
			
			const IOPtr& GetControllingTerminal() const  { return itsControllingTerminal; }
			
			void SetControllingTerminal( const IOPtr& terminal );
	};
	
	class ProcessGroup : public plus::ref_count< ProcessGroup >
	{
		private:
			int                              itsID;
			boost::intrusive_ptr< Session >  itsSession;
		
		public:
			ProcessGroup()  {}
			
			ProcessGroup( int id, const boost::intrusive_ptr< Session >& session )
			:
				itsID( id ),
				itsSession( session )
			{
			}
			
			~ProcessGroup();
			
			int ID() const  { return itsID; }
			
			int GetSID() const  { return itsSession->ID(); }
			
			const boost::intrusive_ptr< Session >& GetSession() const  { return itsSession; }
	};
	
	boost::intrusive_ptr< Session > NewSession( pid_t sid );
	
}

#endif

