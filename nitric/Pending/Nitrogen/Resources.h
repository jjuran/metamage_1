// Resources.h

#ifndef NITROGEN_RESOURCES_H
#define NITROGEN_RESOURCES_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __RESOURCES__
#include FRAMEWORK_HEADER(CarbonCore,Resources.h)
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_IDTYPE_H
#include "Nitrogen/IDType.h"
#endif

namespace Nitrogen
{
	
	class ResID_Tag {};
	typedef IDType< ResID_Tag, ::ResID, 0 > ResID;
	typedef ResID ResourceID;
	
}

#endif

