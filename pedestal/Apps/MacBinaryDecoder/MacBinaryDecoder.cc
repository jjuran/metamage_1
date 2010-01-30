/*	===================
 *	MacBinaryDecoder.cc
 *	===================
 */

// Mac OS
#include <Sound.h>

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Nitrogen/AEDataModel.h"
#include "Nitrogen/AERegistry.h"

// Iteration
#include "Iteration/AEDescListItemDatas.h"

// Arcana
#include "MacBinary.hh"

// MacBinaryDecoder
#include "MacBinaryDecoder.hh"

// Pedestal
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Commands.hh"


namespace MacBinaryDecoder
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Ped = Pedestal;
	
	
	using N::keyDirectObject;
	using N::typeAEList;
	using N::kCoreEventClass;
	using N::kAEOpenDocuments;
	
	App* App::theApp = NULL;
	
	
	static void Decode( Io_Details::stream input, const N::FSDirSpec& destDir )
	{
		MacBinary::Decoder decoder( destDir );
		
		const std::size_t blockSize = 4096;
		
		char data[ blockSize ];
		
		std::size_t totalBytes = 0;
		
		try
		{
			while ( std::size_t bytes = io::read( input, data, blockSize ) )
			{
				decoder.Write( data, bytes );
				
				totalBytes += bytes;
			}
		}
		catch ( const MacBinary::InvalidMacBinaryHeader& )
		{
			//std::fprintf( stderr, "Invalid MacBinary header somewhere past offset %x\n", totalBytes );
			::SysBeep( 30 );
		}
	}
	
	namespace
	{
		
		// Apple event handlers
		
		void HandleOpenDocumentsAppleEvent( const N::AppleEvent&  appleEvent,
											N::AppleEvent&        reply,
											App   *               app )
		{
			ASSERT( app != NULL );
			
			typedef N::AEDescList_ItemDataValue_Container< Io_Details::typeFileSpec > Container;
			typedef Container::const_iterator const_iterator;
			
			NN::Owned< N::AEDescList_Data > docList = N::AEGetParamDesc( appleEvent,
			                                                             keyDirectObject,
			                                                             typeAEList );
			
			Container listData = N::AEDescList_ItemDataValues< Io_Details::typeFileSpec >( docList );
			
			for ( const_iterator it = listData.begin();  it != listData.end();  ++it )
			{
				Io_Details::file_spec fileSpec = *it;
				
				app->OpenDocument( fileSpec );
			}
			
		}
		
	}
	
	
	App& App::Get()
	{
		ASSERT( theApp != NULL );
		
		return *theApp;
	}
	
	static bool About( Ped::CommandCode )
	{
		Ped::ShowAboutBox();
		
		return true;
	}
	
	App::App()
	: 
		itsOpenDocsEventHandler( N::AEInstallEventHandler< App*, HandleOpenDocumentsAppleEvent >( kCoreEventClass,
		                                                                                          kAEOpenDocuments,
		                                                                                          this ) )
	{
		ASSERT( theApp == NULL );
		
		theApp = this;
		
		SetCommandHandler( Ped::kCmdAbout, &About );
	}
	
	App::~App()
	{
	}
	
	void App::OpenDocument( const Io_Details::file_spec& file )
	{
		N::FSDirSpec parent = NN::Convert< N::FSDirSpec >( io::get_preceding_directory( file ) );
		
		Decode( io::open_for_reading( file ), parent );
	}
	
}

int main( void )
{
	MacBinaryDecoder::App app;
	
	return app.Run();
}

