/*	===========
 *	ListView.hh
 *	===========
 */

#ifndef PEDESTAL_LISTVIEW_HH
#define PEDESTAL_LISTVIEW_HH

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_LISTS_TYPES_LISTHANDLE_HH
#include "Mac/Lists/Types/ListHandle.hh"
#endif

// Pedestal
#ifndef PEDESTAL_VIEW_HH
#include "Pedestal/View.hh"
#endif


namespace Pedestal
{
	
	class ListView : public View
	{
		private:
			nucleus::owned< ListHandle > itsList;
		
		private:
			void Install( const Rect& bounds );
			
			void Uninstall();
		
		public:
			virtual bool IntersectsGrowBox() const = 0;
			
			ListHandle Get() const  { return itsList; }
			
			void SetBounds( const Rect& bounds );
			
			bool MouseDown( const EventRecord& event );
			bool KeyDown  ( const EventRecord& event );
			
			void Activate( bool activating )  { ::LActivate( activating, itsList ); }
			
			void Draw( const Rect& bounds, bool erasing );
			
			void AppendCell( const char* data, std::size_t length );
			
			void DeleteCells();
	};
	
}

#endif
