/*	==========
 *	AutoKey.hh
 *	==========
 */

#ifndef PEDESTAL_AUTOKEY_HH
#define PEDESTAL_AUTOKEY_HH


struct EventRecord;


namespace Pedestal
{
	
	void ResetAutoKeyCount();
	
	bool Try_IgnoreAutoKey( const EventRecord& event );
	
}

#endif

