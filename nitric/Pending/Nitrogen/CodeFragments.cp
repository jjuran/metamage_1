// CodeFragments.cp

#ifndef NITROGEN_CODEFRAGMENTS_H
#include "Nitrogen/CodeFragments.h"
#endif

namespace
{

	void RegisterCodeFragmentManagerErrors()
	{
		RegisterOSStatus< paramErr                 >();
		RegisterOSStatus< fragContextNotFound      >();
		RegisterOSStatus< fragConnectionIDNotFound >();
		RegisterOSStatus< fragSymbolNotFound       >();
		RegisterOSStatus< fragSectionNotFound      >();
		RegisterOSStatus< fragLibNotFound          >();
		RegisterOSStatus< fragDupRegLibName        >();
		RegisterOSStatus< fragFormatUnknown        >();
		RegisterOSStatus< fragHadUnresolveds       >();
		RegisterOSStatus< fragNoMem                >();
		RegisterOSStatus< fragNoAddrSpace          >();
		RegisterOSStatus< fragNoContextIDs         >();
		RegisterOSStatus< fragObjectInitSeqErr     >();
		RegisterOSStatus< fragImportTooOld         >();
		RegisterOSStatus< fragImportTooNew         >();
		RegisterOSStatus< fragInitLoop             >();
		RegisterOSStatus< fragInitRtnUsageErr      >();
		RegisterOSStatus< fragLibConnErr           >();
		RegisterOSStatus< fragMgrInitErr           >();
		RegisterOSStatus< fragConstErr             >();
		RegisterOSStatus< fragCorruptErr           >();
		RegisterOSStatus< fragUserInitProcErr      >();
		RegisterOSStatus< fragAppNotFound          >();
		RegisterOSStatus< fragArchErr              >();
		RegisterOSStatus< fragInvalidFragmentUsage >();
	}
	
}

