/*	===========
 *	ListView.hh
 *	===========
 */

#ifndef PEDESTAL_LISTVIEW_HH
#define PEDESTAL_LISTVIEW_HH

// Nitrogen
#include "Nitrogen/Lists.h"

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	class ListView : public View
	{
		private:
			Nucleus::Owned< ListHandle > itsList;
		
		private:
			void Install( const Rect& bounds );
			
			void Uninstall();
		
		public:
			virtual bool IntersectsGrowBox() const = 0;
			
			ListHandle Get() const  { return itsList; }
			
			void SetBounds( const Rect& bounds );
			
			bool MouseDown( const EventRecord& event );
			bool KeyDown  ( const EventRecord& event );
			
			void Activate( bool activating )  { Nitrogen::LActivate( activating, itsList ); }
			
			void Draw( const Rect& bounds, bool erasing );
			
			void SetCell( UInt16 offset, const char* data, std::size_t length );
			
			void AppendCell( const char* data, std::size_t length );
			
			void DeleteCells();
	};
	
}

#endif

