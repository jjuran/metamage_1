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
	
	inline ResType CFragResourceType()     { return ResType    ( kCFragResourceType    ); }
	inline ResID   CFragResourceID()       { return ResID::Make( kCFragResourceID      ); }
	inline OSType  CFragLibraryFileType()  { return OSType     ( kCFragLibraryFileType ); }
	// kCFragAllFileTypes
	
	class CFragArchitecture_Tag {};
	typedef SelectorType< CFragArchitecture_Tag, ::CFragArchitecture, kAnyCFragArch > CFragArchitecture;
	
	inline CFragArchitecture PowerPCCFragArch()      { return CFragArchitecture( kPowerPCCFragArch     ); }
	inline CFragArchitecture Motorola68KCFragArch()  { return CFragArchitecture( kMotorola68KCFragArch ); }
	inline CFragArchitecture AnyCFragArch()          { return CFragArchitecture( kAnyCFragArch         ); }
	
	inline CFragArchitecture CompiledCFragArch()     { return CFragArchitecture( kCompiledCFragArch    ); }
	
	inline std::size_t CFragGoesToEOF()  { return kCFragGoesToEOF; }
	
	using ::CFragConnectionID;
	
	template <>
	struct Disposer< CFragConnectionID > : public std::unary_function< CFragConnectionID, void >,
	                                       private DefaultDestructionOSStatusPolicy
	{
		void operator()( CFragConnectionID connID ) const
		{
			OnlyOnce< RegisterCodeFragmentManagerErrors >();
			HandleDestructionOSStatus( ::CloseConnection( &connID ) );
		}
	};
	
	class CFragLoadOptions_Tag {};
	typedef FlagType< CFragLoadOptions_Tag, ::CFragLoadOptions, 0 > CFragLoadOptions;
	
	inline CFragLoadOptions ReferenceCFrag()    { return CFragLoadOptions( kReferenceCFrag   ); }
	inline CFragLoadOptions FindCFrag()         { return CFragLoadOptions( kFindCFrag        ); }
	inline CFragLoadOptions PrivateCFragCopy()  { return CFragLoadOptions( kPrivateCFragCopy ); }
	
	class CFragSymbolClass_Tag {};
	typedef SelectorType< CFragSymbolClass_Tag, ::CFragSymbolClass, 0 > CFragSymbolClass;
	
	inline CFragSymbolClass CodeCFragSymbol()     { return CFragSymbolClass( kCodeCFragSymbol    ); }
	inline CFragSymbolClass DataCFragSymbol()     { return CFragSymbolClass( kDataCFragSymbol    ); }
	inline CFragSymbolClass TVectorCFragSymbol()  { return CFragSymbolClass( kTVectorCFragSymbol ); }
	inline CFragSymbolClass TOCCFragSymbol()      { return CFragSymbolClass( kTOCCFragSymbol     ); }
	inline CFragSymbolClass GlueCFragSymbol()     { return CFragSymbolClass( kGlueCFragSymbol    ); }
	
	template < class SymbolAddress = void* >
	struct FindSymbol_Result
	{
		SymbolAddress symAddr;
		::CFragSymbolClass symClass;
		
		operator SymbolAddress() const  { return symAddr; }
	};
	
	FindSymbol_Result<> FindSymbol( CFragConnectionID id, const unsigned char* name );
	
	template < class SymbolAddress >
	FindSymbol_Result< SymbolAddress > FindSymbol( CFragConnectionID id, const unsigned char* name )
	{
		FindSymbol_Result< SymbolAddress > result;
		FindSymbol_Result<> oldResult = FindSymbol( id, name );
		
		// The double reinterpret_cast is necessary if SymbolAddress is a 
		// pointer-to-function.  Gcc doesn't like casting a pointer-to-object
		// to a pointer-to-function, so we use long as an intermediary.
		result.symAddr = reinterpret_cast< SymbolAddress >
		(
			reinterpret_cast< long >( oldResult.symAddr )
		);
		result.symClass = oldResult.symClass;
		
		return result;
	}
	
}

#endif
