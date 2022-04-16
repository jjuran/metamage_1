/*
	scoped_zone.hh
	--------------
*/

#ifndef SCOPEDZONE_HH
#define SCOPEDZONE_HH

void* TheZone : 0x0118;

class scoped_zone
{
	private:
		void* const its_saved_zone;
		
		// non-copyable
		scoped_zone           ( const scoped_zone& );
		scoped_zone& operator=( const scoped_zone& );
	
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
