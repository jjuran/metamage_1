/*	===============
 *	ProcessGroup.hh
 *	===============
 */

#ifndef GENIE_PROCESSGROUP_HH
#define GENIE_PROCESSGROUP_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/shared_ptr.hpp>


namespace Genie
{
	
	class IOHandle;
	
	class Session
	{
		private:
			int                             itsID;
			boost::shared_ptr< IOHandle >  itsControllingTerminal;
		
		public:
			Session()  {}
			
			Session( int id ) : itsID( id )  {}
			
			int ID() const  { return itsID; }
			
			const boost::shared_ptr< IOHandle >& GetControllingTerminal() const  { return itsControllingTerminal; }
			
			void SetControllingTerminal( const boost::shared_ptr< IOHandle >& terminal )
			{
				itsControllingTerminal = terminal;
			}
	};
	
	class ProcessGroup
	{
		private:
			int                           itsID;
			boost::shared_ptr< Session >  itsSession;
		
		public:
			ProcessGroup()  {}
			
			ProcessGroup( int id, const boost::shared_ptr< Session >& session ) : itsID( id ),
			                                                                      itsSession( session )
			{
			}
			
			~ProcessGroup();
			
			int ID() const  { return itsID; }
			
			int GetSID() const  { return itsSession->ID(); }
			
			const boost::shared_ptr< Session >& GetSession() const  { return itsSession; }
	};
	
}

#endif

