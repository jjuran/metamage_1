/*	==========
 *	SetFile.cc
 *	==========
 */

// Standard C++
#include <functional>
#include <vector>

// iota
#include "iota/decimal.hh"
#include "iota/quad.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/perror.hh"

// Nitrogen
#include "Nitrogen/Files.hh"
#include "Nitrogen/OSStatus.hh"

// Divergence
#include "Divergence/Utilities.hh"

// OSErrno
#include "OSErrno/OSErrno.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
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
			SignatureSetter( const char* param, OSType FInfo::*field )
			:
				itsCode ( iota::decode_quad( param ) ),
				itsField( field                      )
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
	
	
	static void CreatorOptor( const char* param )
	{
		static SignatureSetter setter( param, &FInfo::fdCreator );
		
		gInfoMutators.push_back( &setter );
	}
	
	static void TypeOptor( const char* param )
	{
		static SignatureSetter setter( param, &FInfo::fdType );
		
		gInfoMutators.push_back( &setter );
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		o::bind_option_trigger_with_param( "-c", std::ptr_fun( CreatorOptor ) );
		o::bind_option_trigger_with_param( "-t", std::ptr_fun( TypeOptor    ) );
		
		o::get_options( argc, argv );
		
		char const *const *free_args = o::free_arguments();
		
		int exit_status = 0;
		
		while ( *free_args )
		{
			const char* pathname = *free_args++;
			
			try
			{
				SetFileInfo( Div::ResolvePathToFSSpec( pathname ) );
			}
			catch ( const N::OSStatus& err )
			{
				plus::var_string status = "OSStatus ";
				
				status += iota::inscribe_decimal( err );
				
				const int errnum = OSErrno::ErrnoFromOSStatus( err );
				
				p7::perror( "SetFile", pathname, status );
				p7::perror( "SetFile", pathname, errnum );
				
				exit_status = 1;
			}
		}
		
		return exit_status;
	}
	
}

