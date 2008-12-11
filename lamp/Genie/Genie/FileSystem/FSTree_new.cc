/*	=============
 *	FSTree_new.cc
 *	=============
 */

#include "Genie/FileSystem/FSTree_new.hh"

// Genie
#include "Genie/FileSystem/FSTree_new_caption.hh"
#include "Genie/FileSystem/FSTree_new_window.hh"


namespace Genie
{
	
	const Singleton_Mapping new_Mappings[] =
	{
		{ "caption", &Singleton_Factory< FSTree_new_caption > },
		{ "window",  &Singleton_Factory< FSTree_new_window  > },
		
		{ NULL, NULL }
	};
	
}

