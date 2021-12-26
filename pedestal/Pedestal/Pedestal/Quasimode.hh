/*	============
 *	Quasimode.hh
 *	============
 */

#ifndef PEDESTAL_QUASIMODE_HH
#define PEDESTAL_QUASIMODE_HH

// vxo
#include "vxo/ref_count.hh"


struct EventRecord;


namespace Pedestal
{
	
	class Quasimode : public vxo::ref_count< Quasimode >
	{
		public:
			virtual ~Quasimode()  {}
			
			virtual bool KeyDown( const EventRecord& event )  { return false; }
	};
	
}

#endif
