// Script.h

#ifndef NITROGEN_SCRIPT_H
#define NITROGEN_SCRIPT_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __SCRIPT__
#include FRAMEWORK_HEADER(CarbonCore,Script.h)
#endif

namespace Nitrogen
{
	
	static const ScriptCode smSystemScript  = ScriptCode::Make( ::smSystemScript  );
	static const ScriptCode smCurrentScript = ScriptCode::Make( ::smCurrentScript );
	static const ScriptCode smAllScripts    = ScriptCode::Make( ::smAllScripts    );
	
}

#endif

