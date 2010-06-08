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
#include <boost/shared_ptr.hpp>


namespace Genie
{
	
	class IOHandle;
	
	class Session : public plus::ref_count< Session >
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
	
}

#endif

