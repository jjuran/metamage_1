/*	==========
 *	SetFile.cc
 *	==========
 */

// Standard C++
#include <functional>
#include <vector>

// Nitrogen
#include "Nitrogen/Files.h"
#include "Nitrogen/OSStatus.h"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Div = Divergence;
	namespace O = Orion;
	
	
	class InfoMutator
	{
		public:
			virtual void operator()( CInfoPBRec& cInfo ) const = 0;
	};
	
	class TypeSetter : public InfoMutator
	{
		private:
			N::OSType itsType;
		
		public:
			TypeSetter( N::OSType type ) : itsType( type )  {}
			
			void operator()( CInfoPBRec& cInfo ) const
			{
				cInfo.hFileInfo.ioFlFndrInfo.fdType = itsType;
			}
	};
	
	class CreatorSetter : public InfoMutator
	{
		private:
			N::OSType itsCreator;
		
		public:
			CreatorSetter( N::OSType creator ) : itsCreator( creator )  {}
			
			void operator()( CInfoPBRec& cInfo ) const
			{
				cInfo.hFileInfo.ioFlFndrInfo.fdCreator = itsCreator;
			}
	};
	
	class InfoMutationApplier
	{
		private:
			CInfoPBRec& itsCInfo;
		
		public:
			InfoMutationApplier( CInfoPBRec& cInfo ) : itsCInfo( cInfo )  {}
			
			void operator()( const InfoMutator* mutator ) const  { mutator[0]( itsCInfo ); }
	};
	
	
	static std::vector< const InfoMutator* > gInfoMutators;
	
	
	static void ApplyInfoMutations( CInfoPBRec& cInfo )
	{
		std::for_each( gInfoMutators.begin(),
		               gInfoMutators.end(),
		               InfoMutationApplier( cInfo ) );
	}
	
	static void SetFileInfo( const FSSpec& file )
	{
		CInfoPBRec cInfo;
		
		N::FSpGetCatInfo( file, cInfo );
		
		ApplyInfoMutations( cInfo );
		
		cInfo.hFileInfo.ioNamePtr = const_cast< StringPtr >( file.name );
		cInfo.hFileInfo.ioDirID   = file.parID;
		
		N::PBSetCatInfoSync( cInfo );
	}
	
	
	static void CreatorOptor( N::OSType param )
	{
		static CreatorSetter setter( NN::Convert< N::OSType >( param ) );
		
		gInfoMutators.push_back( &setter );
	}
	
	static void TypeOptor( N::OSType param )
	{
		static TypeSetter setter( NN::Convert< N::OSType >( param ) );
		
		gInfoMutators.push_back( &setter );
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
		
		O::BindOptionTrigger< N::OSType >( "-c", std::ptr_fun( CreatorOptor ) );
		O::BindOptionTrigger< N::OSType >( "-t", std::ptr_fun( TypeOptor    ) );
		
		O::GetOptions( argc, argv );
		
		char const *const *freeArgs = O::FreeArguments();
		
		std::size_t argCount = O::FreeArgumentCount();
		
		while ( *freeArgs )
		{
			const char* pathname = *freeArgs++;
			
			FSSpec file = Div::ResolvePathToFSSpec( pathname );
			
			SetFileInfo( file );
		}
		
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

