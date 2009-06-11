/*	=======
 *	Sync.hh
 *	=======
 */

#ifndef MACIO_SYNC_HH
#define MACIO_SYNC_HH


namespace MacIO
{
	
	template < class Call_Traits, class Policy >
	inline typename Policy::Result
	//
	PBSync( typename Call_Traits::PB& pb )
	{
		return Policy::HandleOSStatus( Call_Traits::Sync( pb ) );
	}
	
}

#endif

