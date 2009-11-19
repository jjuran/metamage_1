/*
	CurrentFocus.hh
	---------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef PEDESTAL_CURRENTFOCUS_HH
#define PEDESTAL_CURRENTFOCUS_HH


namespace Pedestal
{
	
	class View;
	
	View* Get_Focus();
	
	inline bool Is_Focus( const View* view )  { return view == Get_Focus(); }
	
	void Set_Focus( View* view );
	
	inline void Reset_Focus()  { Set_Focus( NULL ); }
	
	void Change_Focus( View* view );
	
	void Unfocus( const View* view );
	
}

#endif

