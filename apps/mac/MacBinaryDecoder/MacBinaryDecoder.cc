/*
	MacBinaryDecoder.cc
	-------------------
*/

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Mac/Sound/Functions/SysBeep.hh"

#include "Nitrogen/AEDataModel.hh"

// Iteration
#include "Iteration/AEDescListItemDatas.hh"

// Arcana
#include "MacBinary.hh"

// MacBinaryDecoder
#include "MacBinaryDecoder.hh"

// Pedestal
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Commands.hh"


namespace MacBinaryDecoder
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	using Mac::keyDirectObject;
	using Mac::typeAEList;
	using Mac::kCoreEventClass;
	using Mac::kAEOpenDocuments;
	
	
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
			Mac::SysBeep();
		}
	}
	
	static void OpenDocument( const Io_Details::file_spec& file )
	{
		N::FSDirSpec parent = n::convert< N::FSDirSpec >( io::get_preceding_directory( file ) );
		
		Decode( io::open_for_reading( file ), parent );
	}
	
	// Apple event handlers
	
	// Template parameters must have extern linkage
	void HandleOpenDocumentsAppleEvent( const Mac::AppleEvent&  appleEvent,
	                                    Mac::AppleEvent&        reply );
	
	void HandleOpenDocumentsAppleEvent( const Mac::AppleEvent&  appleEvent,
	                                    Mac::AppleEvent&        reply )
	{
		typedef N::AEDescList_ItemDataValue_Container< Io_Details::typeFileSpec > Container;
		typedef Container::const_iterator const_iterator;
		
		n::owned< Mac::AEDescList_Data > docList = N::AEGetParamDesc( appleEvent,
		                                                              keyDirectObject,
		                                                              Mac::typeAEList );
		
		Container listData = N::AEDescList_ItemDataValues< Io_Details::typeFileSpec >( docList );
		
		for ( const_iterator it = listData.begin();  it != listData.end();  ++it )
		{
			Io_Details::file_spec fileSpec = *it;
			
			OpenDocument( fileSpec );
		}
		
	}
	
	
	static bool About( Ped::CommandCode )
	{
		Ped::ShowAboutBox();
		
		return true;
	}
	
	App::App()
	{
		N::AEInstallEventHandler< HandleOpenDocumentsAppleEvent >( kCoreEventClass,
		                                                           kAEOpenDocuments ).release();
		
		SetCommandHandler( Ped::kCmdAbout, &About );
	}
	
}

int main( void )
{
	MacBinaryDecoder::App app;
	
	return app.Run();
}

