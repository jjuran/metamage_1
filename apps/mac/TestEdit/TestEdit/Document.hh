/*
	Document.hh
	-----------
*/

#ifndef TESTEDIT_DOCUMENT_HH
#define TESTEDIT_DOCUMENT_HH

struct FSRef;
struct FSSpec;


namespace TestEdit
{
	
	void NewWindow( const FSSpec& file );
	void NewWindow( const FSRef&  file );
	
}

#endif
