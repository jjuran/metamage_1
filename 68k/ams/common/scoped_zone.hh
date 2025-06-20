/*
	scoped_zone.hh
	--------------
*/

#ifndef SCOPEDZONE_HH
#define SCOPEDZONE_HH

// iota
#include "iota/class.hh"


void* TheZone : 0x0118;

class scoped_zone
{
	NON_COPYABLE( scoped_zone )
	NO_NEW_DELETE
	
	private:
		void* const its_saved_zone;
	
	public:
		scoped_zone( void* zone = 0 ) : its_saved_zone( TheZone )
		{
			TheZone = zone;
		}
		
		~scoped_zone()
		{
			TheZone = its_saved_zone;
		}
};

#endif
