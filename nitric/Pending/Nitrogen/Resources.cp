// Resources.cp

#ifndef NITROGEN_RESOURCES_H
#include "Nitrogen/Resources.h"
#endif

#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif

namespace Nitrogen
{
	void RegisterResourceManagerErrors()
	{
		RegisterOSStatus< dirFulErr        >();
		RegisterOSStatus< dskFulErr        >();
		RegisterOSStatus< bdNamErr         >();
		RegisterOSStatus< ioErr            >();
		RegisterOSStatus< bdNamErr         >();
		RegisterOSStatus< eofErr           >();
		RegisterOSStatus< tmfoErr          >();
		RegisterOSStatus< fnfErr           >();
		RegisterOSStatus< wPrErr           >();
		RegisterOSStatus< fLckdErr         >();
		RegisterOSStatus< vLckdErr         >();
		RegisterOSStatus< dupFNErr         >();
		RegisterOSStatus< opWrErr          >();
		RegisterOSStatus< permErr          >();
		RegisterOSStatus< extFSErr         >();
		RegisterOSStatus< memFullErr       >();
		RegisterOSStatus< dirNFErr         >();
		RegisterOSStatus< resourceInMemory >();
		RegisterOSStatus< writingPastEnd   >();
		RegisterOSStatus< inputOutOfBounds >();
		RegisterOSStatus< resNotFound      >();
		RegisterOSStatus< resFNotFound     >();
		RegisterOSStatus< addResFailed     >();
		RegisterOSStatus< rmvResFailed     >();
		RegisterOSStatus< resAttrErr       >();
		RegisterOSStatus< mapReadErr       >();
	}
}

