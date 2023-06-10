/*
	ClickTarget.cc
	--------------
*/

#include "Pedestal/ClickTarget.hh"

// Pedestal
#include "Pedestal/View.hh"


#ifndef NULL
#define NULL  0
#endif


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
