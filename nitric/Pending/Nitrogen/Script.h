// Script.h

#ifndef NITROGEN_SCRIPT_H
#define NITROGEN_SCRIPT_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __SCRIPT__
#include FRAMEWORK_HEADER(CarbonCore,Script.h)
#endif

#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif

namespace Nitrogen
{
	
	inline ScriptCode SMSystemScript ()  { return ScriptCode::Make( smSystemScript  ); }
	inline ScriptCode SMCurrentScript()  { return ScriptCode::Make( smCurrentScript ); }
	inline ScriptCode SMAllScripts   ()  { return ScriptCode::Make( smAllScripts    ); }
	
}

#endif

