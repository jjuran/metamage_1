/*
	ClickTarget.cc
	--------------
*/

#include "Pedestal/ClickTarget.hh"

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	static View* gCurrentTarget = NULL;
	
	
	View* Get_ClickTarget()
	{
		return gCurrentTarget;
	}
	
	void Set_ClickTarget( View* view )
	{
		gCurrentTarget = view;
	}
	
	void Untarget( const View* view )
	{
		if ( gCurrentTarget == view )
		{
			gCurrentTarget = NULL;
		}
	}
	
}
