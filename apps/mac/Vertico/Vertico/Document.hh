/*
	Vertico/Document.hh
	-------------------
*/

#ifndef VERTICO_DOCUMENT_HH
#define VERTICO_DOCUMENT_HH

// Mac OS
//#include <Files.h>
struct FSSpec;


namespace Vertico
{
	
	void OpenDocument( const FSSpec& file );
	
}

#endif
