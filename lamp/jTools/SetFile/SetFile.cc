/*	==========
 *	SetFile.cc
 *	==========
 */

// Standard C++
#include <functional>
#include <vector>

// Nitrogen
#include "Nitrogen/Files.hh"
#include "Nitrogen/OSStatus.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Div = Divergence;
	namespace o = orion;
	
	
	class InfoMutator
	{
		public:
			virtual void operator()( CInfoPBRec& cInfo ) const = 0;
	};
	
	
	class SignatureSetter : public InfoMutator
	{
		private:
			OSType          itsCode;
			OSType  FInfo::*itsField;
		
		public:
			SignatureSetter( N::OSType code, OSType FInfo::*field )
			:
				itsCode ( code  ),
				itsField( field )
			{
			}
			
			void operator()( CInfoPBRec& cInfo ) const
			{
				FInfo& fInfo = cInfo.hFileInfo.ioFlFndrInfo;
				
				fInfo.*itsField = itsCode;
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
		static SignatureSetter setter( param, &FInfo::fdCreator );
		
		gInfoMutators.push_back( &setter );
	}
	
	static void TypeOptor( N::OSType param )
	{
		static SignatureSetter setter( param, &FInfo::fdType );
		
		gInfoMutators.push_back( &setter );
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
		
		o::bind_option_trigger< N::OSType >( "-c", std::ptr_fun( CreatorOptor ) );
		o::bind_option_trigger< N::OSType >( "-t", std::ptr_fun( TypeOptor    ) );
		
		o::get_options( argc, argv );
		
		char const *const *free_args = o::free_arguments();
		
		while ( *free_args )
		{
			const char* pathname = *free_args++;
			
			FSSpec file = Div::ResolvePathToFSSpec( pathname );
			
			SetFileInfo( file );
		}
		
		return EXIT_SUCCESS;
	}
	
}

