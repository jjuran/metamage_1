/*	===================
 *	MacBinaryDecoder.cc
 *	===================
 */

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/AEDataModel.h"
#include "Nitrogen/AERegistry.h"

// Iteration
#include "Iteration/AEDescListItemDatas.h"

// Arcana
#include "MacBinary.hh"

// MacBinaryDecoder
#include "MacBinaryDecoder.hh"


namespace Nucleus
{
	
	template <> struct Converter< Nitrogen::FSDirSpec, Nitrogen::FSRef > : public std::unary_function< Nitrogen::FSRef, Nitrogen::FSDirSpec >
	{
		Nitrogen::FSDirSpec operator()( const Nitrogen::FSRef& dir ) const
		{
			return Convert< Nitrogen::FSDirSpec >( Convert< FSRef >( dir ) );
		}
	};
	
}

namespace MacBinaryDecoder
{
	
	namespace N = Nitrogen;
	
	using N::keyDirectObject;
	using N::typeAEList;
	using N::kCoreEventClass;
	using N::kAEOpenDocuments;
	
	App* App::theApp = NULL;
	
	
	static void Decode( Io_Details::stream input, const N::FSDirSpec& destDir )
	{
		MacBinary::Decoder decoder( destDir );
		
		char data[ 4096 ];
		
		std::size_t totalBytes = 0;
		
		try
		{
			while ( true )
			{
				std::size_t bytes = io::read( input, data, 4096 );
				
				if ( bytes == 0 )
				{
					// Not reached -- exception thrown instead
					break;
				}
				else if ( bytes < 0 )
				{
					// Not reached -- exception thrown instead
					//std::fprintf( stderr, "macbin: Read failure: %s\n", strerror( errno ) );
					//O::ThrowExitStatus( 1 );
				}
				
				decoder.Write( data, bytes );
				
				totalBytes += bytes;
			}
		}
		catch ( const io::end_of_input& )
		{
			// We're done
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
		
		void HandleOpenDocumentsAppleEvent( const AppleEvent&  appleEvent,
											AppleEvent&        reply,
											App*               app )
		{
			ASSERT( app != NULL );
			
			typedef N::AEDescList_ItemDataValue_Container< Io_Details::typeFileSpec > Container;
			typedef Container::const_iterator const_iterator;
			
			NN::Owned< N::AEDescList > docList = N::AEGetParamDesc( appleEvent,
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
	
	App::App()
	: 
		itsAboutHandler( *this ),
		itsOpenDocsEventHandler( N::AEInstallEventHandler< App*, HandleOpenDocumentsAppleEvent >( kCoreEventClass,
		                                                                                          kAEOpenDocuments,
		                                                                                          this ) )
	{
		ASSERT( theApp == NULL );
		
		theApp = this;
		
		RegisterMenuItemHandler( 'abou', &itsAboutHandler );
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

