/*	=============
 *	FSTree_new.cc
 *	=============
 */

#include "Genie/FS/FSTree_new.hh"

// Genie
#include "Genie/FS/FSTree_new_button.hh"
#include "Genie/FS/FSTree_new_caption.hh"
#include "Genie/FS/FSTree_new_console.hh"
#include "Genie/FS/new_defaultkeys.hh"
#include "Genie/FS/new_focuser.hh"
#include "Genie/FS/FSTree_new_frame.hh"
#include "Genie/FS/FSTree_new_icon.hh"
#include "Genie/FS/FSTree_new_list.hh"
#include "Genie/FS/FSTree_new_scroller.hh"
#include "Genie/FS/FSTree_new_scrollframe.hh"
#include "Genie/FS/FSTree_new_stack.hh"
#include "Genie/FS/FSTree_new_textedit.hh"
#include "Genie/FS/FSTree_new_window.hh"


namespace Genie
{
	
	const FSTree_Premapped::Mapping new_Mappings[] =
	{
		{ "button",       &New_FSTree_new_button              },
		{ "caption",      &New_FSTree_new_caption             },
		{ "console",      &New_FSTree_new_console             },
		{ "defaultkeys",  &New_defaultkeys                    },
		{ "focuser",      &New_focuser                        },
		{ "frame",        &New_FSTree_new_frame               },
		{ "icon",         &New_FSTree_new_icon                },
		{ "list",         &New_FSTree_new_list                },
		{ "scroller",     &New_FSTree_new_scroller            },
		{ "scrollframe",  &New_FSTree_new_scrollframe         },
		{ "stack",        &Basic_Factory< FSTree_new_stack  > },
		{ "textedit",     &New_FSTree_new_textedit            },
		{ "port",         &Basic_Factory< FSTree_new_window > },
		
		{ NULL, NULL }
	};
	
}

