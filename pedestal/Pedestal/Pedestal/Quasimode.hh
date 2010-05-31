/*	============
 *	Quasimode.hh
 *	============
 */

#ifndef PEDESTAL_QUASIMODE_HH
#define PEDESTAL_QUASIMODE_HH

// plus
#include "plus/ref_count.hh"


struct EventRecord;


namespace Pedestal
{
	
	class Quasimode : public plus::ref_count< Quasimode >
	{
		public:
			virtual ~Quasimode()  {}
			
			virtual bool KeyDown( const EventRecord& event )  { return false; }
	};
	
}

#endif

