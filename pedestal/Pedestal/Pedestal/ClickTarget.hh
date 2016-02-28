/*
	ClickTarget.hh
	--------------
*/

#ifndef PEDESTAL_CLICKTARGET_HH
#define PEDESTAL_CLICKTARGET_HH


namespace Pedestal
{
	
	class View;
	
	View* Get_ClickTarget();
	
	void Set_ClickTarget( View* view );
	
	inline void Reset_ClickTarget()  { Set_ClickTarget( 0 ); }  // NULL
	
	void Untarget( const View* view );
	
}

#endif
