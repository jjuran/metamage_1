/*	=============
 *	UserWindow.hh
 *	=============
 */

#ifndef PEDESTAL_USERWINDOW_HH
#define PEDESTAL_USERWINDOW_HH

// Standard C++
#include <memory>

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	class UserView : public View
	{
		private:
			boost::shared_ptr< View > itsView;
		
		public:
			~UserView();
			
			void Set( boost::shared_ptr< View > const& view )  { itsView = view; }
			void Set( std::auto_ptr    < View >        view )  { itsView = view; }
			
			View      & Get()        { return *itsView; }
			View const& Get() const  { return *itsView; }
			
			template < class ViewType >
			ViewType& Get()  { return static_cast< ViewType& >( *itsView ); }
			
			template < class ViewType >
			const ViewType& Get() const  { return static_cast< const ViewType& >( *itsView ); }
			
			void Idle     ( const EventRecord& event )  {        itsView->Idle     ( event ); }
			void MouseDown( const EventRecord& event )  {        itsView->MouseDown( event ); }
			bool KeyDown  ( const EventRecord& event )  { return itsView->KeyDown  ( event ); }
			
			boost::shared_ptr< Quasimode > EnterShiftSpaceQuasimode( const EventRecord& event )
			{
				return itsView->EnterShiftSpaceQuasimode( event );
			}
			
			void Resize( short width, short height )  { itsView->Resize( width, height ); }
			
			void Draw( const Rect& bounds )  { itsView->Draw( bounds ); }
			
			void Activate( bool activating )  { itsView->Activate( activating ); }
			
			bool SetCursor( Point      location,
			                RgnHandle  mouseRgn  )  { return itsView->SetCursor( location, mouseRgn ); }
			
			bool UserCommand( MenuItemCode code  )  { return itsView->UserCommand( code ); }
	};
	
}

#endif

