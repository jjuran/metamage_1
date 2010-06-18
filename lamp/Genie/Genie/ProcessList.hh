/*
	ProcessList.hh
	--------------
*/

#ifndef GENIE_PROCESSLIST_HH
#define GENIE_PROCESSLIST_HH

// Standard C++
#include <map>

// POSIX
#include <sys/types.h>

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// Genie
#include "Genie/Process.hh"


namespace Genie
{
	
	class ProcessList
	{
		public:
			typedef std::map< pid_t, boost::intrusive_ptr< Process > >  Map;
			typedef Map::value_type                                     value_type;
			typedef Map::const_iterator                                 const_iterator;
			typedef Map::iterator                                       iterator;
		
		private:
			Map    itsMap;
		
		public:
			ProcessList();
			
			~ProcessList();
			
			const boost::intrusive_ptr< Process >& NewProcess( Process::RootProcess );
			const boost::intrusive_ptr< Process >& NewProcess( Process& parent );
			
			void RemoveProcess( pid_t pid );
			
			void KillAll();
			
			Map const& GetMap() const  { return itsMap; }
			Map      & GetMap()        { return itsMap; }
			
			const_iterator begin() const  { return itsMap.begin(); }
			const_iterator end  () const  { return itsMap.end  (); }
			
			iterator begin()  { return itsMap.begin(); }
			iterator end  ()  { return itsMap.end  (); }
	};
	
	typedef ProcessList GenieProcessTable;
	
	extern ProcessList gProcessTable;
	
	ProcessList& GetProcessList();
	
	inline const boost::intrusive_ptr< Process >& NewProcess( Process& parent )
	{
		return GetProcessList().NewProcess( parent );
	}
	
	Process& GetInitProcess();
	
}

#endif

