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
			Document( Ped::WindowClosure& closure );
			Document( Ped::WindowClosure& closure, const FSSpec& file );
			Document( Ped::WindowClosure& closure, const FSRef & file );
			
			Window const& GetWindow() const  { return itsWindow.Get(); }
			Window      & GetWindow()        { return itsWindow.Get(); }
			
			N::WindowRef GetWindowRef() const  { return GetWindow().Get(); }
			
			std::string GetName() const  { return GetWindow().GetName(); }
	};
	
}

#endif

