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
	
	class Document
	{
		private:
			std::auto_ptr< Window >  itsWindow;
			bool                     itHasFile;
			bool                     itIsDirty;
		
		public:
			Document();
			Document( const FSSpec& file );
			Document( const FSRef & file );
			
			Window const& GetWindow() const  { return *itsWindow.get(); }
			Window      & GetWindow()        { return *itsWindow.get(); }
			
			Nitrogen::WindowRef GetWindowRef() const  { return GetWindow().Get(); }
			
			std::string GetName() const  { return GetWindow().GetName(); }
	};
	
}

#endif

