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
	
	void GetDiskFragment( const FSSpec& file, 
	                      std::size_t offset, 
	                      std::size_t length, 
	                      ConstStr63Param fragName, 
	                      CFragLoadOptions findFlags, 
	                      CFragConnectionID* connID, 
	                      SymbolAddressPtr* mainAddr, 
	                      ::Str255 errName )
	{
		OnlyOnce< RegisterCodeFragmentManagerErrors >();
		
		::Ptr tempMainAddr;
		
		ThrowOSStatus
		(
			::GetDiskFragment
			(
				&file, offset, length, fragName, findFlags, connID, &tempMainAddr, errName
			)
		);
		
		if ( mainAddr != NULL )
		{
			*mainAddr = reinterpret_cast< SymbolAddressPtr >( tempMainAddr );
		}
	}
	
	void FindSymbol( CFragConnectionID  connID, 
	                 ConstStr255Param   symName, 
	                 SymbolAddressPtr*  symAddr, 
	                 CFragSymbolClass*  symClass )
	{
		OnlyOnce< RegisterCodeFragmentManagerErrors >();
		
		::Ptr tempSymAddr;
		::CFragSymbolClass tempSymClass;
		
		ThrowOSStatus( ::FindSymbol( connID, symName, &tempSymAddr, &tempSymClass ) );
		
		if ( symAddr != NULL )
		{
			*symAddr = reinterpret_cast< SymbolAddressPtr >( tempSymAddr );
		}
		
		if ( symClass != NULL )
		{
			*symClass = CFragSymbolClass( tempSymClass );
		}
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

