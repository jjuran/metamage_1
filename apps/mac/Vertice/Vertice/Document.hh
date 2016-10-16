/*
	Vertice/Document.hh
	-------------------
*/

#ifndef VERTICE_DOCUMENT_HH
#define VERTICE_DOCUMENT_HH

// Pedestal
#ifndef PEDESTAL_USERWINDOW_HH
#include "Pedestal/UserWindow.hh"
#endif


namespace Vertice
{
	
	class Window : public Pedestal::UserWindow
	{
		public:
			Window( ConstStr255Param title );
			
			void Load( const FSSpec& file );
	};
	
}

#endif
