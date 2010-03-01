/*	===========
 *	Document.hh
 *	===========
 */

#ifndef USEEDIT_DOCUMENT_HH
#define USEEDIT_DOCUMENT_HH

// Standard C++
#include <string>

// plus
#include "plus/make_string.hh"

// UseEdit
#include "UseEdit/Window.hh"


namespace UseEdit
{
	
	class Document
	{
		private:
			std::auto_ptr< Pedestal::Window >  itsWindow;
			bool                               itHasFile;
			bool                               itIsDirty;
		
		public:
			Document();
			Document( const FSSpec& file );
			Document( const FSRef & file );
			
			Pedestal::Window const& GetWindow() const  { return *itsWindow.get(); }
			Pedestal::Window      & GetWindow()        { return *itsWindow.get(); }
			
			Nitrogen::WindowRef GetWindowRef() const  { return GetWindow().Get(); }
			
			std::string GetName() const
			{
				return plus::make_string( Nitrogen::GetWTitle( GetWindowRef() ) );
			}
	};
	
}

#endif

