/*	=============
 *	UserWindow.cc
 *	=============
 */

#include "Pedestal/UserWindow.hh"


namespace Pedestal
{
	
	UserView::~UserView()
	{
	}
	
	UserWindow::UserWindow( const NewWindowContext&    context,
			                Nitrogen::WindowDefProcID  procID )
	: Window< UserView, Variable_DefProcID >( context, procID, Initializer() )
	{
	}
	
}

