/*
	App.cc
	------
*/

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// iota
#include "iota/convert_string.hh"

// mac-config
#include "mac_config/apple-events.hh"

// mac-sys-utils
#include "mac_sys/current_process.hh"
#include "mac_sys/is_front_process.hh"

// mac-app-utils
#include "mac_app/documents.hh"
#include "mac_app/file_open_dialog.hh"

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Nitrogen/AEDataModel.hh"
#include "Nitrogen/AEObjects.hh"
#include "Nitrogen/AppleEvents.hh"
#include "Nitrogen/MacWindows.hh"

// AEObjectModel
#include "AEObjectModel/AccessProperty.hh"
#include "AEObjectModel/AEObjectModel.hh"
#include "AEObjectModel/Count.hh"
#include "AEObjectModel/GetData.hh"
#include "AEObjectModel/GetObjectClass.hh"

// Pedestal
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Application.hh"
#include "Pedestal/Commands.hh"
#include "Pedestal/WindowStorage.hh"

// TestEdit
#include "TestEdit/Document.hh"


#define ARRAY_LEN( a )  a, (sizeof (a) / sizeof *(a))


namespace TestEdit
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	static const Mac::DescType typeDocument = Mac::DescType( 'Doc ' );
	
}

namespace Mac
{
	
	template <> struct DescType_scribe< TestEdit::typeDocument > : DescType_scribe< typeWindowRef > {};
	
}

namespace TestEdit
{

static inline
long id_of_window( WindowRef window )
{
	return (unsigned long) window >> 3;
}

static
bool is_document_window( WindowRef window )
{
	// Our document windows are resizable; the About box isn't.
	
	return Ped::get_window_attributes( window ) & kWindowResizableAttribute;
}

static
WindowRef get_this_or_next_document_window( WindowRef window )
{
	while ( window != NULL )
	{
		if ( is_document_window( window ) )
		{
			break;
		}
		
		window = GetNextWindow( window );
	}
	
	return window;
}

static
WindowRef get_first_document_window()
{
	// Document windows should be visible.
	
	return get_this_or_next_document_window( FrontWindow() );
}

static
WindowRef get_next_document_window( WindowRef window )
{
	return get_this_or_next_document_window( GetNextWindow( window ) );
}

static
unsigned count_document_windows()
{
	unsigned n = 0;
	
	WindowRef window = get_first_document_window();
	
	while ( window != NULL )
	{
		++n;
		
		window = get_next_document_window( window );
	}
	
	return n;
}

static
WindowRef get_nth_document_window( unsigned i )
{
	unsigned n = 0;
	
	WindowRef window = get_first_document_window();
	
	while ( window != NULL )
	{
		if ( ++n == i )
		{
			break;
		}
		
		window = get_next_document_window( window );
	}
	
	return window;
}

static
WindowRef get_document_window_by_id( long id )
{
	WindowRef window = get_first_document_window();
	
	while ( window != NULL )
	{
		if ( id_of_window( window ) == id )
		{
			break;
		}
		
		window = get_next_document_window( window );
	}
	
	return window;
}


static
void DocumentClosed( WindowRef window )
{
	Document* doc = (Document*) GetWRefCon( window );
	
	delete doc;
}

static
void CloseDocuments( const AEDescList& list )
{
	long count = N::AECountItems( list );
	
	for ( long i = 1;  i <= count;  ++i )  // one-based
	{
		WindowRef window = N::AEGetNthPtr< typeDocument >( list, i );
		
		Ped::close_window( window );
	}
}

// Apple event handlers

struct Close_AppleEvent
{
	static void Handler( Mac::AppleEvent const&  event,
	                     Mac::AppleEvent&        reply )
	{
		n::owned< Mac::AEDesc_Token > token = N::AEResolve( N::AEGetParamDesc( event,
		                                                                       Mac::keyDirectObject ) );
		
		switch ( token.get().descriptorType )
		{
			case typeDocument:
				Ped::close_window( N::AEGetDescData< typeDocument >( token ) );
				break;
			
			case Mac::typeAEList:
				CloseDocuments( ( token ) );
				break;
			
			default:
				Mac::ThrowOSStatus( errAEEventNotHandled );
				break;
		}
	}
	
	static void Install_Handler()
	{
		N::AEInstallEventHandler< Handler >( Mac::kAECoreSuite,
		                                     Mac::kAEClose ).release();
	}
};

struct Count_AppleEvent
{
	static void Handler( Mac::AppleEvent const&  event,
	                     Mac::AppleEvent&        reply )
	{
		n::owned< Mac::AEDesc_ObjectSpecifier > containerObjSpec = N::AEGetParamDesc( event,
		                                                                              Mac::keyDirectObject );
		
		bool containerIsRoot = containerObjSpec.get().descriptorType == typeNull;
		
		// AEResolve can't handle a null descriptor.
		n::owned< Mac::AEDesc_Token > containerToken = containerIsRoot ? N::GetRootToken()
		                                                               : N::AEResolve( containerObjSpec );
		// The kind of container of the things we're counting, e.g. 'folder'
		Mac::AEObjectClass containerClass = N::GetObjectClass( containerToken );
		
		// The kind of thing we're counting, e.g. 'file'
		Mac::AEObjectClass desiredClass;
		
		/*
			g++ 3 can't handle this:
			
			Mac::AEObjectClass desiredClass = N::AEGetParamPtr< Mac::keyAEObjectClass >( event );
		*/
		
		N::AEGetParamPtr( event,
		                  Mac::keyAEObjectClass,
		                  Mac::typeType,
		                  &desiredClass,
		                  sizeof desiredClass );
		
		std::size_t count = N::Count( desiredClass, containerClass, containerToken );
		
		N::AEPutParamDesc( reply,
		                   Mac::keyDirectObject,
		                   N::AECreateDesc< Mac::typeUInt32 >( count ) );
	}
	
	static void Install_Handler()
	{
		N::AEInstallEventHandler< Handler >( Mac::kAECoreSuite,
		                                     Mac::kAECountElements ).release();
	}
};

struct GetData_AppleEvent
{
	static void Handler( Mac::AppleEvent const&  event,
	                     Mac::AppleEvent&        reply )
	{
		N::AEPutParamDesc( reply,
		                   Mac::keyDirectObject,
		                   N::GetData( N::AEResolve( N::AEGetParamDesc( event,
		                                                                Mac::keyDirectObject ) ) ) );
	}
	
	static void Install_Handler()
	{
		N::AEInstallEventHandler< Handler >( Mac::kAECoreSuite,
		                                     Mac::kAEGetData ).release();
	}
};

// Object accessors

struct AppFrontmost_Property
{
	static n::owned< Mac::AEDesc_Token > Accessor( Mac::AEPropertyID         propertyID,
	                                               const Mac::AEDesc_Token&  containerToken,
	                                               Mac::AEObjectClass        containerClass )
	{
		using mac::sys::current_process;
		using mac::sys::is_front_process;
		
		const bool frontmost = is_front_process( current_process() );
		
		using Mac::typeBoolean;
		using Mac::AEDesc_Token;
		
		return N::AECreateDesc< typeBoolean, AEDesc_Token >( frontmost );
	}
	
	static void Install_Accessor()
	{
		N::RegisterPropertyAccessor( Mac::pIsFrontProcess, Mac::typeNull, Accessor );
	}
};

struct AppName_Property
{
	static n::owned< Mac::AEDesc_Token > Accessor( Mac::AEPropertyID         propertyID,
	                                               const Mac::AEDesc_Token&  containerToken,
	                                               Mac::AEObjectClass        containerClass )
	{
		return N::AECreateDesc< Mac::typeChar, Mac::AEDesc_Token >( "TestEdit" );
	}
	
	static void Install_Accessor()
	{
		N::RegisterPropertyAccessor( Mac::pName, Mac::typeNull, Accessor );
	}
};

static
n::owned< Mac::AEDesc_Token > token_for_document_window( WindowRef window )
{
	if ( window == NULL )
	{
		Mac::ThrowOSStatus( errAENoSuchObject );
	}
	
	return N::AECreateDesc( typeDocument, N::AECreateDesc< Mac::typePtr, Mac::AEDesc_Token >( window ) );
}

static
n::owned< Mac::AEDesc_Token > window_name_token( WindowRef window )
{
	if ( TARGET_API_MAC_CARBON )
	{
		n::owned< CFStringRef > title = N::CopyWindowTitleAsCFString( window );
		
		if ( const UniChar* characters = CFStringGetCharactersPtr( title ) )
		{
			CFIndex length = CFStringGetLength( title );
			
			Size size = length * sizeof (UniChar);
			
			return N::AECreateDesc< Mac::AEDesc_Token >( Mac::typeUnicodeText,
			                                             characters,
			                                             size );
		}
	}
	
	Str255 title;
	
	GetWTitle( window, title );
	
	return N::AECreateDesc< Mac::AEDesc_Token >( Mac::typeChar,
	                                             title + 1,
	                                             title[ 0 ] );
}

static
n::owned< Mac::AEDesc_Token > get_nth_document_token( std::size_t index )
{
	return token_for_document_window( get_nth_document_window( index ) );
}

static
n::owned< Mac::AEDesc_Token > get_document_token_by_id( UInt32 id )
{
	return token_for_document_window( get_document_window_by_id( id ) );
}


struct Document_Element
{
	static n::owned< Mac::AEDesc_Token > Accessor( Mac::AEObjectClass        desiredClass,
	                                               const Mac::AEDesc_Token&  containerToken,
	                                               Mac::AEObjectClass        containerClass,
	                                               Mac::AEKeyForm            keyForm,
	                                               const Mac::AEDesc_Data&   keyData )
	{
		if ( keyForm == Mac::formUniqueID )
		{
			return get_document_token_by_id( N::AEGetDescData< Mac::typeSInt32 >( keyData ) );
		}
		
		if ( keyForm == Mac::formAbsolutePosition )
		{
			std::size_t count = count_document_windows();
			
			UInt32 index = N::ComputeAbsoluteIndex( keyData, count );
			
			if ( index > 0 )
			{
				return get_nth_document_token( index );
			}
			
			// All documents
			n::owned< Mac::AEDescList_Token > list = N::AECreateList< Mac::AEDescList_Token >( false );
			
			for ( UInt32 i = 1;  i <= count;  ++i )
			{
				N::AEPutDesc( list,
				              0,
				              get_nth_document_token( i ) );
			}
			
			return list;
		}
		
		// Unsupported key form
		Mac::ThrowOSStatus( errAEEventNotHandled );
		
		return n::owned< Mac::AEDesc_Token >();
	}
	
	static void Install_Accessor()
	{
		N::AEInstallObjectAccessor< Accessor >( Mac::cDocument, Mac::typeNull ).release();
	}
};

struct DocName_Property
{
	static n::owned< Mac::AEDesc_Token > Accessor( Mac::AEPropertyID         propertyID,
	                                               const Mac::AEDesc_Token&  containerToken,
	                                               Mac::AEObjectClass        containerClass )
	{
		WindowRef window = N::AEGetDescData< typeDocument >( containerToken );
		
		return window_name_token( window );
	}
	
	static void Install_Accessor()
	{
		N::RegisterPropertyAccessor( Mac::pName, typeDocument, Accessor );
	}
};

// Count

struct Documents_Count
{
	static std::size_t Get( Mac::AEObjectClass        desiredClass,
	                        Mac::AEObjectClass        containerClass,
	                        const Mac::AEDesc_Token&  containerToken )
	{
		return count_document_windows();
	}
	
	static void Install()
	{
		N::RegisterCounter( Mac::cDocument, Mac::typeNull, Get );
	}
};

// Get data

struct LiteralData_Token
{
	static n::owned< Mac::AEDesc_Data > Get( const Mac::AEDesc_Token&  obj,
	                                         Mac::DescType             desiredType )
	{
		return N::AEDuplicateDesc( obj );
	}
	
	static void Install_DataGetter( Mac::DescType type )
	{
		N::RegisterDataGetter( type, Get );
	}
};

struct Document_Token
{
	static n::owned< Mac::AEDesc_Data > Get( const Mac::AEDesc_Token&  obj,
	                                         Mac::DescType             desiredType )
	{
		const WindowRef window = N::AEGetDescData< typeDocument >( obj );
		
		const long id = id_of_window( window );
		
		n::owned< Mac::AEDesc_Data > keyData = N::AECreateDesc< Mac::typeSInt32 >( id );
		
		return N::AECreateObjectSpecifier( Mac::cDocument,
		                                   N::GetRootObjectSpecifier(),
		                                   Mac::formUniqueID,
		                                   keyData );
	}
	
	static void Install_DataGetter()
	{
		N::RegisterDataGetter( typeDocument, Get );
	}
};


static void StoreNewDocument( Document* doc )
{
	SetWRefCon( doc->GetWindowRef(), (SRefCon) doc );
	
	try
	{
		Ped::set_window_closed_proc( doc->GetWindowRef(), &DocumentClosed );
	}
	catch ( ... )
	{
		delete doc;
	}
}

static bool About( Ped::CommandCode )
{
	Ped::ShowAboutBox();
	
	return true;
}

static bool NewDocument( Ped::CommandCode )
{
	StoreNewDocument( new Document );
	
	return true;
}

template < class File >
static inline
void OpenDocument( const File& file )
{
	StoreNewDocument( new Document( file ) );
}

#if TARGET_API_MAC_CARBON
	
	typedef FSRef FileSpec;
	
#else
	
	typedef FSSpec FileSpec;
	
#endif

static
long file_opener( const FileSpec& file )
{
	try
	{
		OpenDocument( file );
		
		return noErr;
	}
	catch ( const Mac::OSStatus& err )
	{
		return err;
	}
	catch ( ... )
	{
		return errAEEventNotHandled;
	}
}

static
long HFS_file_opener( short vRefNum, long dirID, const Byte* name )
{
#if ! TARGET_API_MAC_CARBON
	
	FSSpec file;
	
	file.vRefNum = vRefNum;
	file.parID   = dirID;
	
	BlockMoveData( name, file.name, 1 + name[ 0 ] );
	
	return file_opener( file );
	
#endif
	
	return 0;
}

static const OSType file_open_types[] = { 'TEXT' };

static
bool FileOpenDialog( Ped::CommandCode )
{
	using mac::app::file_open_dialog;
	using Ped::apple_events_present;
	
	if ( apple_events_present )
	{
		file_open_dialog( ARRAY_LEN( file_open_types ), &file_opener );
	}
	else
	{
		file_open_dialog( ARRAY_LEN( file_open_types ), &HFS_file_opener );
	}
	
	return true;
}

static
void SetUpAppleEvents()
{
	Close_AppleEvent  ::Install_Handler();
	Count_AppleEvent  ::Install_Handler();
	GetData_AppleEvent::Install_Handler();
	
	// Initialize the Object Support Library.
	N::AEObjectInit();
	
	// List multiplexor, e.g. for 'get name of every window'
	N::AEInstallObjectAccessor< N::DispatchAccessToList >( Mac::AEObjectClass( typeWildCard ), Mac::typeAEList ).release();
	
	// Property accessors
	N::AEInstallObjectAccessor< N::DispatchPropertyAccess >( Mac::cProperty, Mac::typeNull ).release();
	N::AEInstallObjectAccessor< N::DispatchPropertyAccess >( Mac::cProperty, typeDocument  ).release();
	
	Document_Element::Install_Accessor();
	
	// Set up AEObjectModel
	N::AESetObjectCallbacks();
	
	Documents_Count::Install();
	
	LiteralData_Token::Install_DataGetter( Mac::typeBoolean  );
	LiteralData_Token::Install_DataGetter( Mac::typeChar     );
	LiteralData_Token::Install_DataGetter( Mac::typeAERecord );
	
	if ( TARGET_API_MAC_CARBON )
	{
		LiteralData_Token::Install_DataGetter( Mac::typeUnicodeText );
	}
	
	Document_Token::Install_DataGetter();
	
	AppName_Property     ::Install_Accessor();
	AppFrontmost_Property::Install_Accessor();
	
	DocName_Property::Install_Accessor();
}

}

int main( void )
{
	using namespace TestEdit;
	
	Ped::Application app;
	
	const bool apple_events_present =
		CONFIG_APPLE_EVENTS  &&
			(CONFIG_APPLE_EVENTS_GRANTED  ||
				Ped::apple_events_present);
	
	if ( apple_events_present )
	{
		mac::app::open_documents_with( &file_opener );
		
		SetUpAppleEvents();
	}
	else
	{
		mac::app::open_documents_with( &HFS_file_opener );
	}
	
	SetCommandHandler( Ped::kCmdAbout, &About          );
	SetCommandHandler( Ped::kCmdNew,   &NewDocument    );
	SetCommandHandler( Ped::kCmdOpen,  &FileOpenDialog );
	
	return app.Run();
}
