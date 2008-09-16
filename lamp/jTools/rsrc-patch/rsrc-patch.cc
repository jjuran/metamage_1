/*	=============
 *	rsrc-patch.cc
 *	=============
 */

// Standard C++
#include <functional>
#include <vector>

// Nitrogen
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/Resources.h"
#include "Nitrogen/Str.h"

// BitsAndBytes
#include "HexStrings.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Bits = BitsAndBytes;
	namespace Div = Divergence;
	namespace O = Orion;
	
	
	static const char* gResFilePathname = NULL;
	
	static NN::Owned< N::ResFileRefNum > gResFile;
	
	static const char* gResType = NULL;
	static const char* gResID   = NULL;
	
	static N::Handle gHandle;
	
	static std::size_t gOffset = 0;
	
	
	static void FileOptor( const char* param )
	{
		gResFilePathname = param;
		
		gResFile.Reset();  // next line resets only if resolve and open succeed
		
		gResFile = N::FSpOpenResFile( Div::ResolvePathToFSSpec( param ), N::fsRdWrPerm );
	}
	
	static void LoadOptor( const char* )
	{
		gOffset = 0;
		
		if ( gHandle )
		{
			N::ReleaseResource( gHandle );
		}
		
		if ( !gResFile.Get() )
		{
			throw N::ResFNotFound();
		}
		
		if ( gResType == NULL  ||  gResID == NULL )
		{
			throw N::ResNotFound();
		}
		
		N::ResType resType = NN::Convert< N::ResType >( std::string( gResType ) );
		
		N::ResID   resID   = N::ResID( std::atoi( gResID ) );
		
		gHandle = N::Get1Resource( resType, resID );
	}
	
	static void Find( const std::string& pattern )
	{
		const char* begin = *gHandle.Get();
		const char* end   = begin + N::GetHandleSize( gHandle );
		
		const char* mark = std::search( begin + gOffset,
		                                end,
		                                pattern.begin(),
		                                pattern.end() );
		
		if ( mark == end )
		{
			throw N::ParamErr();
		}
		
		gOffset = mark - begin;
	}
	
	static void FindOptor( const char* param )
	{
		Find( param );
	}
	
	static void FindHexOptor( const char* param )
	{
		Find( Bits::DecodeHex( param ) );
	}
	
	static void SeekOptor( const char* param )
	{
		std::size_t delta = std::atoi( param );
		
		gOffset += delta;
	}
	
	static void Write( const std::string& bytes )
	{
		if ( gOffset + bytes.size() > N::GetHandleSize( gHandle ) )
		{
			throw N::ParamErr();
		}
		
		char* begin = *gHandle.Get();
		
		std::copy( bytes.begin(), bytes.end(), begin + gOffset );
		
		gOffset += bytes.size();
		
		N::ChangedResource( gHandle );
	}
	
	static void WriteOptor( const char* param )
	{
		Write( param );
	}
	
	static void WriteHexOptor( const char* param )
	{
		Write( Bits::DecodeHex( param ) );
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
		
		O::BindOption( "--type", gResType );
		O::BindOption( "--id",   gResID   );
		
		O::BindOptionTrigger( "--file", std::ptr_fun( FileOptor ) );
		O::BindOptionTrigger( "--load", std::ptr_fun( LoadOptor ) );
		O::BindOptionTrigger( "--find", std::ptr_fun( FindOptor ) );
		O::BindOptionTrigger( "--seek", std::ptr_fun( SeekOptor ) );
		
		O::BindOptionTrigger( "--write", std::ptr_fun( WriteOptor ) );
		
		O::BindOptionTrigger( "--find-hex",  std::ptr_fun( FindHexOptor  ) );
		O::BindOptionTrigger( "--write-hex", std::ptr_fun( WriteHexOptor ) );
		
		//O::AliasOption( "--type", "-t" );
		
		O::GetOptions( argc, argv );
		
		char const *const *freeArgs = O::FreeArguments();
		
		std::size_t argCount = O::FreeArgumentCount();
		
		return EXIT_SUCCESS;
	}

}

namespace Orion
{
	
	int Main( int argc, iota::argv_t argv )
	{
		return tool::Main( argc, argv );
	}
	
}

