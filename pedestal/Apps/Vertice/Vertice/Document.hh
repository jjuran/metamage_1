/*	===================
 *	Vertice/Document.hh
 *	===================
 */

#ifndef VERTICE_DOCUMENT_HH
#define VERTICE_DOCUMENT_HH

// Pedestal
#include "Pedestal/Window.hh"

// Vertice
#include "Vertice/PortView.hh"


namespace Vertice
{
	
	namespace Ped = Pedestal;
	
	
	class Window : public Ped::Window< PortView >
	{
		public:
			Window( const boost::shared_ptr< Ped::WindowCloseHandler >&  handler, ConstStr255Param title );
			
			Model& ItsModel()  { return SubView().ItsModel(); }
			
			void Load( const FSSpec& file );
			void Store();
	};
	
}

#endif

