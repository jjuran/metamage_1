/*
	Document.hh
	-----------
*/

#ifndef TESTEDIT_DOCUMENT_HH
#define TESTEDIT_DOCUMENT_HH

// Pedestal
#include "Pedestal/WindowStorage.hh"

// TestEdit
#include "TestEdit/Window.hh"


namespace TestEdit
{
	
	WindowRef NewWindow( const FSSpec& file );
	WindowRef NewWindow( const FSRef&  file );
	
	class Document
	{
		private:
			WindowRef  itsWindow;
			
			// non-copyable
			Document           ( const Document& );
			Document& operator=( const Document& );
		
		public:
			Document()                     : itsWindow( NewWindow(      ) )  {}
			Document( const FSSpec& file ) : itsWindow( NewWindow( file ) )  {}
			Document( const FSRef&  file ) : itsWindow( NewWindow( file ) )  {}
			
			~Document()
			{
				if ( itsWindow )
				{
					Pedestal::set_window_closed_proc( itsWindow, NULL );
					
					Pedestal::close_window( itsWindow );
				}
			}
			
			WindowRef GetWindowRef() const  { return itsWindow; }
	};
	
}

#endif
