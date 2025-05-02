/*
	Document.hh
	-----------
*/

#ifndef TESTEDIT_DOCUMENT_HH
#define TESTEDIT_DOCUMENT_HH

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_WINDOWS_TYPES_WINDOWREF_HH
#include "Mac/Windows/Types/WindowRef.hh"
#endif


namespace TestEdit
{
	
	class Document
	{
		private:
			nucleus::owned< WindowRef >  itsWindow;
		
		public:
			Document();
			Document( const FSSpec& file );
			Document( const FSRef & file );
			
			WindowRef GetWindowRef() const  { return itsWindow; }
	};
	
}

#endif
