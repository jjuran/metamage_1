/*	===================
 *	Vertice/Document.hh
 *	===================
 */

#ifndef VERTICE_DOCUMENT_HH
#define VERTICE_DOCUMENT_HH

// Io
#include "Io/TextInput.hh"

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
			Window( Ped::WindowClosure& closure, ConstStr255Param title );
			
			Model& ItsModel()  { return SubView().ItsModel(); }
			
			void Load( Io::TextInputAdapter input );
			void Load( const FSSpec& file );
			void Store();
	};
	
}

#endif

