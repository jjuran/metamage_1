/*
	Document.hh
	-----------
*/

#ifndef TESTEDIT_DOCUMENT_HH
#define TESTEDIT_DOCUMENT_HH

// Pedestal
#include "Pedestal/WindowStorage.hh"


namespace TestEdit
{
	
	class Document
	{
		private:
			WindowRef  itsWindow;
			
			// non-copyable
			Document           ( const Document& );
			Document& operator=( const Document& );
		
		public:
			Document();
			Document( const FSSpec& file );
			Document( const FSRef & file );
			
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
