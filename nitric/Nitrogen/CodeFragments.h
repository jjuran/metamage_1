// CodeFragments.h

#ifndef NITROGEN_CODEFRAGMENTS_H
#define NITROGEN_CODEFRAGMENTS_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __CODEFRAGMENTS__
#include FRAMEWORK_HEADER(CarbonCore,CodeFragments.h)
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_CFBUNDLE_H
#include "Nitrogen/CFBundle.h"
#endif
#ifndef NITROGEN_FILES_H
#include "Nitrogen/Files.h"
#endif
#ifndef NITROGEN_MULTIPROCESSING_H
#include "Nitrogen/Multiprocessing.h"
#endif
#ifndef NITROGEN_RESOURCES_H
#include "Nitrogen/Resources.h"
#endif


#ifdef CFragHasFileLocation
#undef CFragHasFileLocation

inline bool CFragHasFileLocation( CFragLocatorKind where )
{
	return where == kDataForkCFragLocator
		|| where == kResourceCFragLocator
	#if TARGET_RT_MAC_CFM
		|| where == kCFBundleIntCFragLocator
	#endif
		;
}

#endif

namespace Nitrogen
{
	
	void RegisterCodeFragmentManagerErrors();
	
	#pragma mark -
	#pragma mark ¥ General Types and Constants ¥
	
	class CFragArchitecture_Tag {};
	typedef Nucleus::SelectorType< CFragArchitecture_Tag, ::CFragArchitecture, kAnyCFragArch > CFragArchitecture;
	
	class CFragLoadOptions_Tag {};
	typedef Nucleus::FlagType< CFragLoadOptions_Tag, ::CFragLoadOptions, 0 > CFragLoadOptions;
	
	class CFragSymbolClass_Tag {};
	typedef Nucleus::SelectorType< CFragSymbolClass_Tag, ::CFragSymbolClass, 0 > CFragSymbolClass;
	
	using ::CFragConnectionID;
	
  }

namespace Nucleus
  {
	template <>
	struct Disposer< Nitrogen::CFragConnectionID > : public std::unary_function< Nitrogen::CFragConnectionID, void >,
	                                                 private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::CFragConnectionID connID ) const
		{
			Nucleus::OnlyOnce< Nitrogen::RegisterCodeFragmentManagerErrors >();
			HandleDestructionOSStatus( ::CloseConnection( &connID ) );
		}
	};
  }

namespace Nitrogen
  {
	
	
	// Opaque pointer type
	typedef struct SymbolAddress* SymbolAddressPtr;
	
	// errMessage wrapper
	struct ErrMessage
	{
		Str255 errMessage;
		
		ErrMessage( ConstStr255Param errMessage ) : errMessage( errMessage )  {}
	};
	
	// OSStatus wrapper that carries errMessage
	template < class ErrorCode >
	struct OSStatusErrMessage : ErrorCode,
	                            ErrMessage
	{
		OSStatusErrMessage( ConstStr255Param errMessage )
		:
			ErrMessage( errMessage )
		{}
	};
	
	template < ::CFragLoadOptions options >  struct CFragLoadOptions_Traits;
	
	struct Find_CFragConnection_Traits
	{
		typedef CFragConnectionID Result;
		
		static Result MakeResult( CFragConnectionID connID )  { return connID; }
	};
	
	struct Reference_CFragConnection_Traits
	{
		typedef Nucleus::Owned< CFragConnectionID > Result;
		
		static Result MakeResult( CFragConnectionID connID )
		{
			return Nucleus::Owned< CFragConnectionID >::Seize( connID );
		}
	};
	
	template <>  struct CFragLoadOptions_Traits< kFindCFrag > : Find_CFragConnection_Traits {};
	
	template <>  struct CFragLoadOptions_Traits< kReferenceCFrag   > : Reference_CFragConnection_Traits {};
	template <>  struct CFragLoadOptions_Traits< kPrivateCFragCopy > : Reference_CFragConnection_Traits {};
	
	template < class SymbolAddressType >
	SymbolAddressType SymAddr_Cast( SymbolAddressPtr symAddr )
	{
		// The double reinterpret_cast is necessary if SymbolAddressType is a 
		// pointer-to-function.  Gcc doesn't like casting a pointer-to-object
		// to a pointer-to-function, so we use long as an intermediary.
		
		return reinterpret_cast< SymbolAddressType >( reinterpret_cast< long >( symAddr ) );
	}
	
	// GetSharedLibrary
	
	void GetSharedLibrary( ConstStr63Param     libName,
	                       CFragArchitecture   archType,
	                       CFragLoadOptions    options,
	                       CFragConnectionID*  connID   = NULL,
	                       SymbolAddressPtr*   mainAddr = NULL );
	
	template < ::CFragLoadOptions options, class MainAddrType >
	typename CFragLoadOptions_Traits< options >::Result
	GetSharedLibrary( ConstStr63Param     libName,
	                  CFragArchitecture   archType,
	                  MainAddrType*       mainAddr )
	{
		typedef CFragLoadOptions_Traits< options > Traits;
		
		CFragConnectionID connID;
		SymbolAddressPtr tempMainAddr;
		
		GetSharedLibrary( libName,
		                  archType,
		                  CFragLoadOptions( options ),
		                  &connID,
		                  &tempMainAddr );
		
		if ( mainAddr != NULL )
		{
			*mainAddr = SymAddr_Cast< MainAddrType >( tempMainAddr );
		}
		
		return Traits::MakeResult( connID );
	}
	
	template < ::CFragLoadOptions options >
	typename CFragLoadOptions_Traits< options >::Result
	GetSharedLibrary( ConstStr63Param     libName,
	                  CFragArchitecture   archType )
	{
		return GetSharedLibrary< options, SymbolAddressPtr >( libName,
		                                                      archType,
		                                                      NULL );
	}
	
	void GetDiskFragment( const FSSpec&       file,
	                      std::size_t         offset,
	                      std::size_t         length,
	                      ConstStr63Param     fragName,
	                      CFragLoadOptions    options,
	                      CFragConnectionID*  connID   = NULL,
	                      SymbolAddressPtr*   mainAddr = NULL );
	
	template < ::CFragLoadOptions options, class MainAddrType >
	typename CFragLoadOptions_Traits< options >::Result
	GetDiskFragment( const FSSpec&    file,
	                 std::size_t      offset,
	                 std::size_t      length,
	                 ConstStr63Param  fragName,
	                 MainAddrType*    mainAddr )
	{
		typedef CFragLoadOptions_Traits< options > Traits;
		
		CFragConnectionID connID;
		SymbolAddressPtr tempMainAddr;
		
		GetDiskFragment( file,
		                 offset,
		                 length,
		                 fragName,
		                 CFragLoadOptions( options ),
		                 &connID,
		                 &tempMainAddr );
		
		if ( mainAddr != NULL )
		{
			*mainAddr = SymAddr_Cast< MainAddrType >( tempMainAddr );
		}
		
		return Traits::MakeResult( connID );
	}
	
	template < ::CFragLoadOptions options >
	typename CFragLoadOptions_Traits< options >::Result
	GetDiskFragment( const FSSpec&    file,
	                 std::size_t      offset   = 0,
	                 std::size_t      length   = kCFragGoesToEOF,
	                 ConstStr63Param  fragName = NULL )
	{
		return GetDiskFragment< options, SymbolAddressPtr >( file,
		                                                     offset,
		                                                     length,
		                                                     fragName,
		                                                     NULL );
	}
	
	// GetMemFragment
	
	void GetMemFragment( const void*         memAddr,
	                     std::size_t         length,
	                     ConstStr63Param     fragName,
	                     CFragLoadOptions    options,
	                     CFragConnectionID*  connID   = NULL,
	                     SymbolAddressPtr*   mainAddr = NULL );
	
	template < ::CFragLoadOptions options, class MainAddrType >
	typename CFragLoadOptions_Traits< options >::Result
	GetMemFragment( const void*      memAddr,
	                std::size_t      length,
	                ConstStr63Param  fragName,
	                MainAddrType*    mainAddr )
	{
		typedef CFragLoadOptions_Traits< options > Traits;
		
		CFragConnectionID connID;
		SymbolAddressPtr tempMainAddr;
		
		GetMemFragment( memAddr,
		                length,
		                fragName,
		                CFragLoadOptions( options ),
		                &connID,
		                &tempMainAddr );
		
		if ( mainAddr != NULL )
		{
			*mainAddr = SymAddr_Cast< MainAddrType >( tempMainAddr );
		}
		
		return Traits::MakeResult( connID );
	}
	
	template < ::CFragLoadOptions options >
	typename CFragLoadOptions_Traits< options >::Result
	GetMemFragment( const void*      memAddr,
	                std::size_t      length,
	                ConstStr63Param  fragName = NULL )
	{
		return GetMemFragment< options, SymbolAddressPtr >( memAddr,
		                                                    length,
		                                                    fragName,
		                                                    NULL );
	}
	
	void CloseConnection( Nucleus::Owned< CFragConnectionID > connID );
	
	// 384
	void FindSymbol( CFragConnectionID  connID,
	                 ConstStr255Param   symName,
	                 SymbolAddressPtr*  symAddr,
	                 CFragSymbolClass*  symClass );
	
	template < class SymAddrType >
	void FindSymbol( CFragConnectionID  connID,
	                 ConstStr255Param   symName,
	                 SymAddrType*       symAddr,
	                 CFragSymbolClass*  symClass = NULL )
	{
		SymbolAddressPtr tempSymAddr;
		
		FindSymbol( connID, symName, &tempSymAddr, symClass );
		
		if ( symAddr != NULL )
		{
			*symAddr = SymAddr_Cast< SymAddrType >( tempSymAddr );
		}
	}
	
	template < class SymAddrType >
	SymAddrType FindSymbol( CFragConnectionID  connID, 
	                        ConstStr255Param   symName, 
	                        CFragSymbolClass*  symClass = NULL )
	{
		SymAddrType result;
		
		FindSymbol< SymAddrType >( connID, symName, &result, symClass );
		
		return result;
	}
	
	std::size_t CountSymbols( CFragConnectionID connID );
	
	void GetIndSymbol( CFragConnectionID  connID,
	                   std::size_t        symIndex,
	                   Str255             symName,
	                   SymbolAddressPtr*  symAddr,
	                   CFragSymbolClass*  symClass );
	
	template < class SymAddrType >
	void GetIndSymbol( CFragConnectionID  connID,
	                   std::size_t        symIndex,
	                   Str255             symName,
	                   SymAddrType*       symAddr,
	                   CFragSymbolClass*  symClass = NULL )
	{
		SymbolAddressPtr tempSymAddr;
		
		GetIndSymbol( connID, symIndex, symName, &tempSymAddr, symClass );
		
		if ( symAddr != NULL )
		{
			*symAddr = SymAddr_Cast< SymAddrType >( tempSymAddr );
		}
	}
	
	// ...
	
}

#endif
