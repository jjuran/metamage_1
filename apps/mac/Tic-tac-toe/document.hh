/*
	document.hh
	-----------
*/

#ifndef DOCUMENT_HH
#define DOCUMENT_HH

template < class File >
struct global_document_file
{
	static File value;
};

template < class File >
File global_document_file< File >::value;

extern bool document_assigned;
extern bool document_modified;

#endif
