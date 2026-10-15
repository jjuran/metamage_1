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
#include "mac_config/upp-macros.hh"

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

// AEObjectModel
#include "AEObjectModel/AEObjectModel.hh"

// Pedestal
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Application.hh"
#include "Pedestal/Commands.hh"
#include "Pedestal/WindowStorage.hh"

// TestEdit
#include "TestEdit/Document.hh"
#include "TestEdit/Window.hh"


#define COUNT( a )  (sizeof (a) / sizeof *(a))

#define ARRAY_LEN( a )  a, (sizeof (a) / sizeof *(a))


static inline
OSStatus CopyWindowTitleAsAEDesc( WindowRef window, AEDesc* result )
{
	CFStringRef string;
	Str255      str;
	
	DescType    type;
	const void* data;
	Size        size;
	
	if ( TARGET_API_MAC_CARBON )
	{
		OSStatus err = CopyWindowTitleAsCFString( window, &string );
		
		if ( err )
		{
			return err;
		}
		
		if ( const UniChar* characters = CFStringGetCharactersPtr( string ) )
		{
			type = typeUnicodeText;
			data = characters;
			size = sizeof (UniChar) * CFStringGetLength( string );
			
			goto got_title;
		}
	}
	
	GetWTitle( window, str );
	
	type = typeChar;
	data = str + 1;
	size = str[ 0 ];
	
got_title:
	
	OSStatus err = AECreateDesc( type, data, size, result );
	
	if ( TARGET_API_MAC_CARBON )
	{
		CFRelease( string );
	}
	
	return err;
}

namespace Nitrogen
{
	
	static inline
	nucleus::owned< Mac::AEDesc_ObjectSpecifier > GetRootObjectSpecifier()
	{
		return nucleus::owned< Mac::AEDesc_ObjectSpecifier >();
	}
	
	static
	nucleus::owned< Mac::AEDesc_ObjectSpecifier >
	//
	AECreateObjectSpecifier( Mac::AEObjectClass                  objectClass,
	                         const Mac::AEDesc_ObjectSpecifier&  container,
	                         Mac::AEKeyForm                      keyForm,
	                         const Mac::AEDesc_Data&             keyData )
	{
		nucleus::owned< Mac::AERecord_Data > record = AECreateList( true );
		
		/*
			g++ 3 can't handle this:
			
			AEPutKeyPtr< Mac::keyAEDesiredClass >( record, objectClass );
			AEPutKeyPtr< Mac::keyAEKeyForm      >( record, keyForm     );
		*/
		
		AEPutKeyPtr( record,
		             Mac::keyAEDesiredClass,
		             Mac::typeType,
		             &objectClass,
		             sizeof objectClass );
		
		AEPutKeyPtr( record,
		             Mac::keyAEKeyForm,
		             Mac::typeEnumerated,
		             &keyForm,
		             sizeof keyForm );
		
		AEPutKeyDesc( record, Mac::keyAEKeyData,   keyData   );
		AEPutKeyDesc( record, Mac::keyAEContainer, container );
		
		return AECoerceDesc( record, Mac::typeObjectSpecifier );
	}
	
}

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
		n::owned< Mac::AEDesc_Token > containerToken = containerIsRoot ? n::owned< Mac::AEDesc_Token >()
		                                                               : N::AEResolve( containerObjSpec );
		
		// The kind of thing we're counting, e.g. 'file'
		DescType desiredClass;
		
		/*
			g++ 3 can't handle this:
			
			Mac::AEObjectClass desiredClass = N::AEGetParamPtr< Mac::keyAEObjectClass >( event );
		*/
		
		N::AEGetParamPtr( event,
		                  Mac::keyAEObjectClass,
		                  Mac::typeType,
		                  &desiredClass,
		                  sizeof desiredClass );
		
		std::size_t count;
		
		if ( containerToken.get().descriptorType == typeNull  &&  desiredClass == cDocument )
		{
			count = count_document_windows();
		}
		else
		{
			Mac::ThrowOSStatus( errAEEventNotHandled );
		}
		
		N::AEPutParamDesc( reply,
		                   Mac::keyDirectObject,
		                   N::AECreateDesc< Mac::typeUInt32 >( count ) );
	}
};

static
nucleus::owned< Mac::AEDesc_Data >
//
GetData( const Mac::AEDesc_Token&  obj,
         Mac::DescType             desiredType = Mac::typeWildCard );

static
nucleus::owned< Mac::AEDesc_Data >
//
GetDataFromTokenList( const Mac::AEDescList_Token& obj, Mac::DescType desiredType )
{
	using N::AECreateList;
	using N::AECountItems;
	using N::AEPutDesc;
	using N::AEDesc_Cast;
	using N::AEGetNthDesc;
	
	nucleus::owned< Mac::AEDescList_Data > list = AECreateList< Mac::AEDesc_Data >( false );
	
	UInt32 count = AECountItems( obj );
	
	for ( UInt32 i = 1;  i <= count;  ++i )
	{
		AEPutDesc( list,
		           0,
		           GetData( AEDesc_Cast< const Mac::AEDesc_Token >( AEGetNthDesc( obj, i ).get() ),
		                    desiredType ).get() );
	}
	
	return list;
}

static
nucleus::owned< Mac::AEDesc_Data >
//
GetDocumentObjSpec( const Mac::AEDesc_Token& obj )
{
	const WindowRef window = N::AEGetDescData< typeDocument >( obj );
	
	const long id = id_of_window( window );
	
	n::owned< Mac::AEDesc_Data > keyData = N::AECreateDesc< Mac::typeSInt32 >( id );
	
	return N::AECreateObjectSpecifier( Mac::cDocument,
	                                   N::GetRootObjectSpecifier(),
	                                   Mac::formUniqueID,
	                                   keyData );
}

static
nucleus::owned< Mac::AEDesc_Data >
//
GetData( const Mac::AEDesc_Token& obj, Mac::DescType desiredType )
{
	switch ( obj.descriptorType )
	{
		case typeNull:
			return N::GetRootObjectSpecifier();
		
		case typeAEList:
			return GetDataFromTokenList( obj, desiredType );
		
		case typeBoolean:
		case typeChar:
		case typeType:
		case typeAERecord:
		case typeUnicodeText:
			return N::AEDuplicateDesc( obj );
		
		case typeDocument:
			return GetDocumentObjSpec( obj );
		
		default:
			Mac::ThrowOSStatus( errAEEventNotHandled );
	}
	
	// not reached
	return nucleus::owned< Mac::AEDesc_Data >();
}

struct GetData_AppleEvent
{
	static void Handler( Mac::AppleEvent const&  event,
	                     Mac::AppleEvent&        reply )
	{
		N::AEPutParamDesc( reply,
		                   Mac::keyDirectObject,
		                   GetData( N::AEResolve( N::AEGetParamDesc( event,
		                                                             Mac::keyDirectObject ) ) ) );
	}
};

// Object accessors

static
n::owned< Mac::AEDesc_Token > is_frontmost()
{
	using mac::sys::current_process;
	using mac::sys::is_front_process;
	
	const bool frontmost = is_front_process( current_process() );
	
	using Mac::typeBoolean;
	using Mac::AEDesc_Token;
	
	return N::AECreateDesc< typeBoolean, AEDesc_Token >( frontmost );
}

static
n::owned< Mac::AEDesc_Token > app_name()
{
	return N::AECreateDesc< Mac::typeChar, Mac::AEDesc_Token >( "TestEdit" );
}

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
			
			UInt32 index = ComputeAbsoluteIndex( keyData, count );
			
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
};

static
n::owned< Mac::AEDesc_Token > doc_name( const Mac::AEDesc_Token& containerToken )
{
	WindowRef window = N::AEGetDescData< typeDocument >( containerToken );
	
	Mac::AEDesc_Token desc;
	
	OSStatus err = CopyWindowTitleAsAEDesc( window, &desc );
	
	Mac::ThrowOSStatus( err );
	
	return n::owned< Mac::AEDesc_Token >::seize( desc );
}


static bool About( Ped::CommandCode )
{
	Ped::ShowAboutBox();
	
	return true;
}

static bool NewDocument( Ped::CommandCode )
{
	NewWindow();
	
	return true;
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
		NewWindow( file );
		
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

static const DescType app_properties[] =
{
	pClass,
	pName,
	pIsFrontProcess,
};

static const DescType doc_properties[] =
{
	pClass,
	pName,
};

static
n::owned< Mac::AEDesc_Token >
//
new_property_token( const Mac::AEDesc_Token&  containerToken,
                    DescType                  containerClass,
                    DescType                  propertyID )
{
	switch ( containerClass )
	{
		case typeNull:
			switch ( propertyID )
			{
				case pClass:
					return N::AECreateDesc< Mac::typeType, Mac::AEDesc_Token >( Mac::DescType( cApplication ) );
				
				case pName:
					return app_name();
				
				case pIsFrontProcess:
					return is_frontmost();
				
				default:
					break;
			}
			break;
		
		case cDocument:
			switch ( propertyID )
			{
				case pName:
					return doc_name( containerToken );
				
				default:
					break;
			}
			break;
		
		default:
			break;
	}
	
	if ( propertyID == pClass )
	{
		return N::AECreateDesc< Mac::typeType, Mac::AEDesc_Token >( Mac::DescType( containerClass ) );
	}
	
	Mac::ThrowOSStatus( errAENoSuchObject );
	
	return n::owned< Mac::AEDesc_Token >();
}

static
n::owned< Mac::AEDesc_Token >
//
access_all_properties( const Mac::AEDesc_Token&  containerToken,
                       Mac::AEObjectClass        containerClass,
                       const DescType*           p,
                       unsigned                  n_properties )
{
	typedef Mac::AEDesc_Token Token;
	
	n::owned< Token > result = N::AECreateList< Token >( true );
	
	/*
		For each property belonging to the container
		object class, create a token corresponding
		to that property of the container object.
		
		The newly created token is stored in a scoped
		type that will automatically destroy it fully.
		After adding it the list, we need to have the
		AEDesc disposed, but token disposal shouldn't
		occur yet.  Therefore, disclaim the ownership
		and call AEDisposeDesc() explicitly.
		
		If adding the token to the list fails, then
		AEPutDesc() will throw and the token will be
		fully destroyed.  If any failure in the loop
		occurs, the partially built token list will
		be fully destroyed as well.
	*/
	
	while ( n_properties-- > 0 )
	{
		AEKeyword prop = *p++;
		
		n::owned< Token > token = new_property_token( containerToken,
		                                              containerClass,
		                                              prop );
		
		N::AEPutKeyDesc( result, (Mac::AEKeyword) prop, token );
		
		AEDesc desc = token.release();
		
		AEDisposeDesc( &desc );
	}
	
	return result;
}

n::owned< Mac::AEDesc_Token >
//
DispatchPropertyAccess( Mac::AEObjectClass        desiredClass,
                        const Mac::AEDesc_Token&  containerToken,
                        Mac::AEObjectClass        containerClass,
                        Mac::AEKeyForm            keyForm,
                        const Mac::AEDesc_Data&   keyData );

n::owned< Mac::AEDesc_Token >
//
DispatchPropertyAccess( Mac::AEObjectClass        desiredClass,
                        const Mac::AEDesc_Token&  containerToken,
                        Mac::AEObjectClass        containerClass,
                        Mac::AEKeyForm            keyForm,
                        const Mac::AEDesc_Data&   keyData )
{
	if ( keyData.descriptorType == typeAbsoluteOrdinal )
	{
		const DescType* p;
		unsigned        n;
		
		switch ( containerClass | 0 )
		{
			case typeNull:
				p =        app_properties;
				n = COUNT( app_properties );
				break;
			
			case cDocument:
				p =        doc_properties;
				n = COUNT( doc_properties );
				break;
			
			default:
				Mac::ThrowOSStatus( errAEEventNotHandled );
		}
		
		return access_all_properties( containerToken, containerClass, p, n );
	}
	
	DescType propertyID = N::AEGetDescData< Mac::typeType >( keyData );
	
	return new_property_token( containerToken, containerClass, propertyID );
}

DEFINE_UPP( OSLDisposeToken, OSLDisposeToken )

static
void SetUpAppleEvents()
{
	OSErr err;
	
	N::AEInstallEventHandler< Close_AppleEvent  ::Handler >( Mac::kAECoreSuite, Mac::kAEClose         ).release();
	N::AEInstallEventHandler< Count_AppleEvent  ::Handler >( Mac::kAECoreSuite, Mac::kAECountElements ).release();
	N::AEInstallEventHandler< GetData_AppleEvent::Handler >( Mac::kAECoreSuite, Mac::kAEGetData       ).release();
	
	// Initialize the Object Support Library.
	N::AEObjectInit();
	
	// List multiplexor, e.g. for 'get name of every window'
	N::AEInstallObjectAccessor< DispatchAccessToList >( Mac::AEObjectClass( typeWildCard ), Mac::typeAEList ).release();
	
	// Property accessors
	N::AEInstallObjectAccessor< DispatchPropertyAccess >( Mac::cProperty, Mac::typeNull ).release();
	N::AEInstallObjectAccessor< DispatchPropertyAccess >( Mac::cProperty, typeDocument  ).release();
	
	N::AEInstallObjectAccessor< Document_Element::Accessor >( Mac::cDocument, Mac::typeNull ).release();
	
	err = AESetObjectCallbacks( (OSLCompareUPP     ) NULL,
	                            (OSLCountUPP       ) NULL,
	                            UPP_ARG( OSLDisposeToken ),
	                            (OSLGetMarkTokenUPP) NULL,
	                            (OSLMarkUPP        ) NULL,
	                            (OSLAdjustMarksUPP ) NULL,
	                            (OSLGetErrDescUPP  ) NULL );
	
	Mac::ThrowOSStatus( err );
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
