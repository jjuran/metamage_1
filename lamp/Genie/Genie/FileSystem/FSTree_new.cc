/*	=============
 *	FSTree_new.cc
 *	=============
 */

#include "Genie/FileSystem/FSTree_new.hh"

// Genie
#include "Genie/FileSystem/FSTree_new_button.hh"
#include "Genie/FileSystem/FSTree_new_caption.hh"
#include "Genie/FileSystem/FSTree_new_console.hh"
#include "Genie/FileSystem/FSTree_new_frame.hh"
#include "Genie/FileSystem/FSTree_new_icon.hh"
#include "Genie/FileSystem/FSTree_new_list.hh"
#include "Genie/FileSystem/FSTree_new_scroller.hh"
#include "Genie/FileSystem/FSTree_new_scrollframe.hh"
#include "Genie/FileSystem/FSTree_new_stack.hh"
#include "Genie/FileSystem/FSTree_new_textedit.hh"
#include "Genie/FileSystem/FSTree_new_window.hh"


namespace Genie
{
	
	const FSTree_Premapped::Mapping new_Mappings[] =
	{
		{ "button",       &Basic_Factory< FSTree_new_button       > },
		{ "caption",      &Basic_Factory< FSTree_new_caption      > },
		{ "console",      &Basic_Factory< FSTree_new_console      > },
		{ "frame",        &Basic_Factory< FSTree_new_frame        > },
		{ "icon",         &Basic_Factory< FSTree_new_icon         > },
		{ "list",         &Basic_Factory< FSTree_new_list         > },
		{ "scroller",     &Basic_Factory< FSTree_new_scroller     > },
		{ "scrollframe",  &Basic_Factory< FSTree_new_scrollframe  > },
		{ "stack",        &Basic_Factory< FSTree_new_stack        > },
		{ "textedit",     &Basic_Factory< FSTree_new_textedit     > },
		{ "window",       &Basic_Factory< FSTree_new_window       > },
		
		{ NULL, NULL }
	};
	
}

