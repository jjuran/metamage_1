/*	=============
 *	FSTree_new.cc
 *	=============
 */

#include "Genie/FileSystem/FSTree_new.hh"

// Genie
#include "Genie/FileSystem/FSTree_new_caption.hh"
#include "Genie/FileSystem/FSTree_new_frame.hh"
#include "Genie/FileSystem/FSTree_new_icon.hh"
#include "Genie/FileSystem/FSTree_new_iconid.hh"
#include "Genie/FileSystem/FSTree_new_scroller.hh"
#include "Genie/FileSystem/FSTree_new_scrollframe.hh"
#include "Genie/FileSystem/FSTree_new_textedit.hh"
#include "Genie/FileSystem/FSTree_new_window.hh"


namespace Genie
{
	
	const FSTree_Premapped::Mapping new_Mappings[] =
	{
		{ "caption",      &Basic_Factory< FSTree_new_caption      > },
		{ "frame",        &Basic_Factory< FSTree_new_frame        > },
		{ "icon",         &Basic_Factory< FSTree_new_icon         > },
		{ "iconid",       &Basic_Factory< FSTree_new_iconid       > },
		{ "scroller",     &Basic_Factory< FSTree_new_scroller     > },
		{ "scrollframe",  &Basic_Factory< FSTree_new_scrollframe  > },
		{ "textedit",     &Basic_Factory< FSTree_new_textedit     > },
		{ "window",       &Basic_Factory< FSTree_new_window       > },
		
		{ NULL, NULL }
	};
	
}

