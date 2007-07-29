/*	===========
 *	Document.hh
 *	===========
 */

#ifndef USEEDIT_DOCUMENT_HH
#define USEEDIT_DOCUMENT_HH

// Standard C++
#include <string>

// UseEdit
#include "UseEdit/Window.hh"


namespace UseEdit
{
	
	namespace N = Nitrogen;
	
	class Document
	{
		private:
			Ped::SingleWindowOwner< Window >  itsWindow;
			bool                              itHasFile;
			bool                              itIsDirty;
		
		public:
			Document( const boost::shared_ptr< Ped::WindowCloseHandler >&  handler );
			Document( const boost::shared_ptr< Ped::WindowCloseHandler >&  handler, const FSSpec& file );
			Document( const boost::shared_ptr< Ped::WindowCloseHandler >&  handler, const FSRef & file );
			
			Window const& GetWindow() const  { return itsWindow.Get(); }
			Window      & GetWindow()        { return itsWindow.Get(); }
			
			N::WindowRef GetWindowRef() const  { return GetWindow().Get(); }
			
			std::string GetName() const  { return GetWindow().GetName(); }
	};
	
}

#endif

