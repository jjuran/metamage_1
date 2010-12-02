/*	=============
 *	UserWindow.hh
 *	=============
 */

#ifndef PEDESTAL_USERWINDOW_HH
#define PEDESTAL_USERWINDOW_HH

// Pedestal
#ifndef PEDESTAL_WINDOW_HH
#include "Pedestal/Window.hh"
#endif


namespace Pedestal
{
	
	class UserWindow : public Window
	{
		private:
			boost::intrusive_ptr< View > itsView;
		
		public:
			UserWindow( const NewWindowContext& context ) : Window( context )
			{
			}
			
			boost::intrusive_ptr< View >& GetView()  { return itsView; }
	};
	
}

#endif

