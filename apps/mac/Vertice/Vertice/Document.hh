/*
	Vertice/Document.hh
	-------------------
*/

#ifndef VERTICE_DOCUMENT_HH
#define VERTICE_DOCUMENT_HH

// Mac OS
//#include <Files.h>
struct FSSpec;


namespace Vertice
{
	
	void OpenDocument( const FSSpec& file );
	
}

#endif
