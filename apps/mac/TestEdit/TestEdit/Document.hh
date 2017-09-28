/*
	Document.hh
	-----------
*/

#ifndef TESTEDIT_DOCUMENT_HH
#define TESTEDIT_DOCUMENT_HH

// plus
#include "plus/ref_count.hh"
#include "plus/string.hh"

// TestEdit
#include "TestEdit/Window.hh"


namespace TestEdit
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
			
			WindowRef GetWindowRef() const  { return GetWindow().Get(); }
			
			plus::string GetName() const;
	};
	
}

#endif
