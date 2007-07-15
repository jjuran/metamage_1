/*	===============
 *	ProcessGroup.hh
 *	===============
 */

#ifndef GENIE_PROCESSGROUP_HH
#define GENIE_PROCESSGROUP_HH

// Boost
#include <boost/shared_ptr.hpp>


namespace Genie
{
	
	class Session
	{
		private:
			int itsID;
		
		public:
			Session()  {}
			
			Session( int id ) : itsID( id )  {}
			
			int ID() const  { return itsID; }
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
			
			int ID() const  { return itsID; }
			
			int GetSID() const  { return itsSession->ID(); }
			
			const boost::shared_ptr< Session >& GetSession() const  { return itsSession; }
	};
	
}

#endif

