// ControlDefinitions.cp

#ifndef NITROGEN_CONTROLDEFINITIONS_H
#include "Nitrogen/ControlDefinitions.h"
#endif

namespace Nitrogen
{
	
	namespace Tests
	{
		static void Test_kControlEditTextCFStringTag( ControlRef control, CFStringRef string )
		{
			Nitrogen::SetControlData< kControlEditTextCFStringTag >( control, kControlNoPart, string );
		}
	}
	
}

