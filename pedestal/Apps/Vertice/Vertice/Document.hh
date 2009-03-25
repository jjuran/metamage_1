/*	===================
 *	Vertice/Document.hh
 *	===================
 */

#ifndef VERTICE_DOCUMENT_HH
#define VERTICE_DOCUMENT_HH

// Pedestal
#include "Pedestal/UserWindow.hh"

// Vertice
#include "Vertice/PortView.hh"


namespace Vertice
{
	
	class Window : public Pedestal::UserWindow
	{
		public:
			Window( ConstStr255Param title );
			
			Scene& ItsScene();
			
			void Load( const FSSpec& file );
			void Store();
	};
	
}

#endif

