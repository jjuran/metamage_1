// CodeFragments.cp

#ifndef NITROGEN_CODEFRAGMENTS_H
#include "Nitrogen/CodeFragments.h"
#endif

#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif
#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen
{
	
	FindSymbol_Result<> FindSymbol( CFragConnectionID id, const unsigned char* name )
	{
		OnlyOnce< RegisterCodeFragmentManagerErrors >();
		
		::Ptr symAddr;
		::CFragSymbolClass symClass;
		
		ThrowOSStatus( ::FindSymbol( id, name, &symAddr, &symClass ) );
		
		FindSymbol_Result<> result;
		result.symAddr  = symAddr;
		result.symClass = symClass;
		
		return result;
	}
	
	void RegisterCodeFragmentManagerErrors()
	{
		RegisterOSStatus< paramErr                >();
		RegisterOSStatus< cfragContextIDErr       >();
		RegisterOSStatus< cfragConnectionIDErr    >();
		RegisterOSStatus< cfragNoSymbolErr        >();
		RegisterOSStatus< cfragNoSectionErr       >();
		RegisterOSStatus< cfragNoLibraryErr       >();
		RegisterOSStatus< cfragDupRegistrationErr >();
		RegisterOSStatus< cfragFragmentFormatErr  >();
		RegisterOSStatus< cfragUnresolvedErr      >();
		RegisterOSStatus< cfragNoPrivateMemErr    >();
		RegisterOSStatus< cfragNoClientMemErr     >();
		RegisterOSStatus< cfragNoIDsErr           >();
		RegisterOSStatus< cfragInitOrderErr       >();
		RegisterOSStatus< cfragImportTooOldErr    >();
		RegisterOSStatus< cfragImportTooNewErr    >();
		RegisterOSStatus< cfragInitLoopErr        >();
		RegisterOSStatus< cfragInitAtBootErr      >();
		RegisterOSStatus< cfragLibConnErr         >();
		RegisterOSStatus< cfragCFMStartupErr      >();
		RegisterOSStatus< cfragCFMInternalErr     >();
		RegisterOSStatus< cfragFragmentCorruptErr >();
		RegisterOSStatus< cfragInitFunctionErr    >();
		RegisterOSStatus< cfragNoApplicationErr   >();
		RegisterOSStatus< cfragArchitectureErr    >();
		RegisterOSStatus< cfragFragmentUsageErr   >();
		RegisterOSStatus< cfragLastErrCode        >();
	}
	
}

