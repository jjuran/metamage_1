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
		char                         label;
		bool                         selected;
		bool                         disabling;
		
		Icon_Parameters() : align(), xform(), label(), selected(), disabling()
		{
		}
	};
	
	inline Nitrogen::IconTransformType
	//
	CombinedIconTransforms( const Icon_Parameters& params )
	{
		typedef Nitrogen::IconTransformType Type;
		
		Type state    = Type( params.xform                         );
		Type label    = Type( params.label    * kTransformLabel1   );
		Type selected = Type( params.selected * kTransformSelected );
		
		return state | label | selected;
	}
	
}

#endif

