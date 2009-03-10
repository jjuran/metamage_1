/*	========
 *	Icons.hh
 *	========
 */

#ifndef GENIE_FILESYSTEM_ICONS_HH
#define GENIE_FILESYSTEM_ICONS_HH

// Nitrogen
#include "Nitrogen/Icons.h"


namespace Genie
{
	
	struct Icon_Parameters
	{
		Nitrogen::IconAlignmentType  align;
		Nitrogen::IconTransformType  xform;
		
		Icon_Parameters() : align(), xform()
		{
		}
	};
	
}

#endif

