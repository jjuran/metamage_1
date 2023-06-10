/*
	CurrentFocus.cc
	---------------
	
	Copyright 2009, Joshua Juran
*/

#include "Pedestal/CurrentFocus.hh"

// Pedestal
#include "Pedestal/View.hh"


#ifndef NULL
#define NULL  0
#endif


namespace Pedestal
{
	
	static View* gCurrentView = NULL;
	
	
	View* Get_Focus()
	{
		return gCurrentView;
	}
	
	void Set_Focus( View* view )
	{
		gCurrentView = view;
	}
	
	void Change_Focus( View* view )
	{
		if ( gCurrentView != view )
		{
			if ( gCurrentView != NULL )
			{
				gCurrentView->Blur();
			}
			
			gCurrentView = view;
			
			if ( gCurrentView != NULL )
			{
				gCurrentView->Focus();
			}
		}
	}
	
	void Unfocus( const View* view )
	{
		if ( gCurrentView == view )
		{
			gCurrentView = NULL;
		}
	}
	
}
