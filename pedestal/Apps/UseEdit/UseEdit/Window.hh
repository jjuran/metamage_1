/*	=========
 *	Window.hh
 *	=========
 */

#ifndef USEEDIT_WINDOW_HH
#define USEEDIT_WINDOW_HH

// Standard C++
#include <string>

// Pedestal
#include "Pedestal/Scroller.hh"
#include "Pedestal/TEView.hh"
#include "Pedestal/Window.hh"


namespace UseEdit
{
	
	namespace Ped = Pedestal;
	
	class Window : public Ped::Window< Ped::Scroller< Ped::TEView,
	                                                  Ped::kLiveFeedbackVariant,
	                                                  Ped::kNoScrollbar > >
	{
		public:
			typedef Ped::Window< Ped::Scroller< Ped::TEView,
			                                    Ped::kLiveFeedbackVariant,
			                                    Ped::kNoScrollbar > > Base;
			
			Window( const boost::shared_ptr< Ped::WindowCloseHandler >&  handler,
			        ConstStr255Param                                     title = "\p" "UseEdit" );
			
			std::string GetName() const;
			
			void SetName( ConstStr255Param   name );
			void SetName( const std::string& name );
	};
	
}

#endif

