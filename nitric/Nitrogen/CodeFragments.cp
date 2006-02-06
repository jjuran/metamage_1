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
	
	template < class ErrorCode >
	void ThrowOSStatusErrMessage( ConstStr255Param errMessage )
	{
		throw OSStatusErrMessage< ErrorCode >( errMessage );
	}
	
	static void ThrowOSStatusErrMessage( OSStatus status, ConstStr255Param errMessage )
	{
		switch ( status.Get ())
		{
			case noErr:
				return;
				break;
			
			case paramErr:
				throw ParamErr();
				break;
			
			case cfragNoLibraryErr:     ThrowOSStatusErrMessage< CFragNoLibraryErr    >( errMessage );
			case cfragUnresolvedErr:    ThrowOSStatusErrMessage< CFragUnresolvedErr   >( errMessage );
			case cfragNoPrivateMemErr:  ThrowOSStatusErrMessage< CFragNoPrivateMemErr >( errMessage );
			case cfragNoClientMemErr:   ThrowOSStatusErrMessage< CFragNoClientMemErr  >( errMessage );
			case cfragInitOrderErr:     ThrowOSStatusErrMessage< CFragInitOrderErr    >( errMessage );
			case cfragImportTooOldErr:  ThrowOSStatusErrMessage< CFragImportTooOldErr >( errMessage );
			case cfragImportTooNewErr:  ThrowOSStatusErrMessage< CFragImportTooNewErr >( errMessage );
			case cfragInitLoopErr:      ThrowOSStatusErrMessage< CFragInitLoopErr     >( errMessage );
			case cfragLibConnErr:       ThrowOSStatusErrMessage< CFragLibConnErr      >( errMessage );
			case cfragInitFunctionErr:  ThrowOSStatusErrMessage< CFragInitFunctionErr >( errMessage );
			
			default:
				ThrowOSStatus( status );
		}
	}
	
	void GetSharedLibrary( ConstStr63Param     libName,
	                       CFragArchitecture   archType,
	                       CFragLoadOptions    options,
	                       CFragConnectionID*  connID,
	                       SymbolAddressPtr*   mainAddr )
	{
		Nucleus::OnlyOnce< RegisterCodeFragmentManagerErrors >();
		
		CFragConnectionID tempConnID;  // Can't be NULL in OS 8.1 and earlier
		::Ptr tempMainAddr;
		Str255 errMessage;
		
		ThrowOSStatusErrMessage( ::GetSharedLibrary( libName,
		                                             archType,
		                                             options,
		                                             &tempConnID,
		                                             &tempMainAddr,
		                                             errMessage ),
		                          errMessage );
		
		if ( connID != NULL )
		{
			*connID = tempConnID;
		}
		
		if ( mainAddr != NULL )
		{
			*mainAddr = reinterpret_cast< SymbolAddressPtr >( tempMainAddr );
		}
	}
	
	void GetDiskFragment( const FSSpec&       file,
	                      std::size_t         offset,
	                      std::size_t         length,
	                      ConstStr63Param     fragName,
	                      CFragLoadOptions    options,
	                      CFragConnectionID*  connID,
	                      SymbolAddressPtr*   mainAddr )
	{
		Nucleus::OnlyOnce< RegisterCodeFragmentManagerErrors >();
		
		::Ptr tempMainAddr;
		Str255 errMessage;
		
		// This works for all result codes that GetDiskFragment() might throw.
		// Ideally, it should work for any registered OSStatus, but that would take some work.
		// It may require substantial modification and extension of the ErrorCode mechanism.
		
		ThrowOSStatusErrMessage( ::GetDiskFragment( &file,
		                                            offset,
		                                            length,
		                                            fragName,
		                                            options,
		                                            connID,
		                                            &tempMainAddr,
		                                            errMessage ),
		                          errMessage );
		
		if ( mainAddr != NULL )
		{
			*mainAddr = reinterpret_cast< SymbolAddressPtr >( tempMainAddr );
		}
	}
	
	void GetMemFragment( const void*         memAddr,
	                     std::size_t         length,
	                     ConstStr63Param     fragName,
	                     CFragLoadOptions    options,
	                     CFragConnectionID*  connID,
	                     SymbolAddressPtr*   mainAddr )
	{
		Nucleus::OnlyOnce< RegisterCodeFragmentManagerErrors >();
		
		::Ptr tempMainAddr;
		Str255 errMessage;
		
		ThrowOSStatusErrMessage( ::GetMemFragment( const_cast< void* >( memAddr ),
		                                           length,
		                                           fragName,
		                                           options,
		                                           connID,
		                                           &tempMainAddr,
		                                           errMessage ),
		                          errMessage );
		
		if ( mainAddr != NULL )
		{
			*mainAddr = reinterpret_cast< SymbolAddressPtr >( tempMainAddr );
		}
	}
	
	void CloseConnection( Nucleus::Owned< CFragConnectionID > connID )
	{
		Nucleus::OnlyOnce< RegisterCodeFragmentManagerErrors >();
		
		CFragConnectionID connIDcopy = connID.Release();
		
		ThrowOSStatus( ::CloseConnection( &connIDcopy ) );
	}
	
	void FindSymbol( CFragConnectionID  connID, 
	                 ConstStr255Param   symName, 
	                 SymbolAddressPtr*  symAddr, 
	                 CFragSymbolClass*  symClass )
	{
		Nucleus::OnlyOnce< RegisterCodeFragmentManagerErrors >();
		
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
	
	std::size_t CountSymbols( CFragConnectionID connID )
	{
		Nucleus::OnlyOnce< RegisterCodeFragmentManagerErrors >();
		
		long result;
		
		ThrowOSStatus( ::CountSymbols( connID, &result ) );
		
		return result;
	}
	
	void GetIndSymbol( CFragConnectionID  connID,
	                   std::size_t        symIndex,
	                   Str255             symName,
	                   SymbolAddressPtr*  symAddr,
	                   CFragSymbolClass*  symClass )
	{
		Nucleus::OnlyOnce< RegisterCodeFragmentManagerErrors >();
		
		::Ptr tempSymAddr;
		::CFragSymbolClass tempSymClass;
		
		ThrowOSStatus( ::GetIndSymbol( connID,
		                               symIndex,
		                               symName,
		                               &tempSymAddr,
		                               &tempSymClass ) );
		
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

