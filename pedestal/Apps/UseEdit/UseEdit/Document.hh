/*	===========
 *	Document.hh
 *	===========
 */

#ifndef USEEDIT_DOCUMENT_HH
#define USEEDIT_DOCUMENT_HH

// iota
#include "iota/convert_string.hh"

// plus
#include "plus/ref_count.hh"
#include "plus/string.hh"

// UseEdit
#include "UseEdit/Window.hh"


namespace UseEdit
{
	
	class Document : public plus::ref_count< Document >
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
			
			plus::string GetName() const
			{
				return iota::convert_string< plus::string >( Nitrogen::GetWTitle( GetWindowRef() ) );
			}
	};
	
}

#endif

