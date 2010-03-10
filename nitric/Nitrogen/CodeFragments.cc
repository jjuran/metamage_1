// Nitrogen/CodeFragments.cc
// -------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/CodeFragments.hh"

// Nitrogen
#if NUCLEUS_RICH_ERRORCODES
#include "Nitrogen/MacErrors.hh"
#endif
#include "Nitrogen/OSStatus.hh"


namespace Nitrogen
{
	
	// does nothing, but guarantees construction of theRegistration
	NUCLEUS_DEFINE_ERRORS_DEPENDENCY( CodeFragmentManager )
	
	
	static void RegisterCodeFragmentManagerErrors();
	
	
#if NUCLEUS_RICH_ERRORCODES
#pragma force_active on
	
	class CodeFragmentManagerErrorsRegistration
	{
		public:
			CodeFragmentManagerErrorsRegistration()  { RegisterCodeFragmentManagerErrors(); }
	};
	
	static CodeFragmentManagerErrorsRegistration theRegistration;
	
#pragma force_active reset
#endif
	
	
	template < class ErrorCode >
	void ThrowOSStatusErrMessage( ConstStr255Param errMessage )
	{
		throw OSStatusErrMessage< ErrorCode >( errMessage );
	}
	
	static void ThrowOSStatusErrMessage( OSStatus status, ConstStr255Param errMessage )
	{
		switch ( status.Get ())
		{
		#if NUCLEUS_RICH_ERRORCODES
			
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
			
		#else
			
			case cfragNoLibraryErr:
			case cfragUnresolvedErr:
			case cfragNoPrivateMemErr:
			case cfragNoClientMemErr:
			case cfragInitOrderErr:
			case cfragImportTooOldErr:
			case cfragImportTooNewErr:
			case cfragInitLoopErr:
			case cfragLibConnErr:
			case cfragInitFunctionErr:
				throw OSStatusErrMessage< Nucleus::ErrorCode< OSStatus, 0 > >( status, errMessage );
			
		#endif
			
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
	
	void CloseConnection( nucleus::owned< CFragConnectionID > connID )
	{
		CFragConnectionID connIDcopy = connID.release();
		
		ThrowOSStatus( ::CloseConnection( &connIDcopy ) );
	}
	
	void FindSymbol( CFragConnectionID  connID, 
	                 ConstStr255Param   symName, 
	                 SymbolAddressPtr*  symAddr, 
	                 CFragSymbolClass*  symClass )
	{
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

