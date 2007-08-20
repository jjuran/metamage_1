/*	============
 *	Quasimode.hh
 *	============
 */

#ifndef PEDESTAL_QUASIMODE_HH
#define PEDESTAL_QUASIMODE_HH


struct EventRecord;


namespace Pedestal
{
	
	class Quasimode
	{
		public:
			virtual ~Quasimode()  {}
			
			virtual bool KeyDown( const EventRecord& event )  { return false; }
	};
	
}

#endif

