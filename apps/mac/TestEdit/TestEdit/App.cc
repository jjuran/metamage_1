/*
	App.cc
	------
*/

// iota
#include "iota/convert_string.hh"

// mac-sys-utils
#include "mac_sys/current_process.hh"
#include "mac_sys/is_front_process.hh"

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Nitrogen/AEDataModel.hh"
#include "Nitrogen/AEObjects.hh"
#include "Nitrogen/CarbonEvents.hh"
#include "Nitrogen/MacWindows.hh"

// Iteration
#include "Iteration/AEDescListItemDatas.hh"

// AEObjectModel
#include "AEObjectModel/AccessProperty.hh"
#include "AEObjectModel/AEObjectModel.hh"
#include "AEObjectModel/Count.hh"
#include "AEObjectModel/GetData.hh"
#include "AEObjectModel/GetObjectClass.hh"

// Pedestal
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Commands.hh"
#include "Pedestal/WindowStorage.hh"

// TestEdit
#include "TestEdit/App.hh"
#include "TestEdit/Document.hh"


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
	
	static DocumentContainer gDocuments;
	
	
	static void StoreNewDocument( Document* doc );
	
	
	static
	void DocumentClosed( WindowRef window )
	{
		Document* doc = (Document*) GetWRefCon( window );
		
		delete doc;
	}
	
	template < class Container >
	static void CloseDocuments( const Container& container )
	{
		typedef typename Container::const_iterator Iter;
		
		for ( Iter it = container.begin();  it != container.end();  ++it )
		{
			Ped::close_window( *it );
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
					CloseDocuments( N::AEDescList_ItemDataValues< typeDocument >( token ) );
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
			Mac::AEObjectClass desiredClass = N::AEGetParamPtr< Mac::keyAEObjectClass >( event );
			
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
	
	template < class File >
	static inline
	void OpenDocument( const File& file )
	{
		StoreNewDocument( new Document( file ) );
	}
	
	struct OpenDocuments_AppleEvent
	{
		static void Handler( Mac::AppleEvent const&  event,
		                     Mac::AppleEvent&        reply )
		{
			typedef N::AEDescList_ItemDataValue_Container< Io_Details::typeFileSpec > Container;
			typedef Container::const_iterator const_iterator;
			
			n::owned< Mac::AEDescList_Data > docList = N::AEGetParamDesc( event,
			                                                              Mac::keyDirectObject,
			                                                              Mac::typeAEList );
			
			Container listData = N::AEDescList_ItemDataValues< Io_Details::typeFileSpec >( docList );
			
			for ( const_iterator it = listData.begin();  it != listData.end();  ++it )
			{
				Io_Details::file_spec fileSpec = *it;
				
				OpenDocument( fileSpec );
			}
		}
		
		static void Install_Handler()
		{
			N::AEInstallEventHandler< Handler >( Mac::kCoreEventClass,
			                                     Mac::kAEOpenDocuments ).release();
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
				return gDocuments.GetElementByID( N::AEGetDescData< Mac::typeSInt32 >( keyData ) );
			}
			
			if ( keyForm == Mac::formAbsolutePosition )
			{
				std::size_t count = count_document_windows();
				
				UInt32 index = N::ComputeAbsoluteIndex( keyData, count );
				
				if ( index > 0 )
				{
					return gDocuments.GetElementByIndex( index );
				}
				
				// All documents
				n::owned< Mac::AEDescList_Token > list = N::AECreateList< Mac::AEDescList_Token >( false );
				
				for ( UInt32 i = 1;  i <= count;  ++i )
				{
					N::AEPutDesc( list,
					              0,
					              gDocuments.GetElementByIndex( i ) );
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
			
			const Document& document = *(Document*) GetWRefCon( window );
			
			return N::AECreateDesc< Mac::typeChar, Mac::AEDesc_Token >( iota::convert_string< n::string >( document.GetName() ) );
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
	
	
	n::owned< Mac::AEDesc_Token > DocumentContainer::GetElementByIndex( std::size_t index ) const
	{
		return token_for_document_window( get_nth_document_window( index ) );
	}
	
	n::owned< Mac::AEDesc_Token > DocumentContainer::GetElementByID( UInt32 id ) const
	{
		return token_for_document_window( get_document_window_by_id( id ) );
	}
	
	
	static void StoreNewDocument( Document* doc )
	{
		N::SetWRefCon( doc->GetWindowRef(), doc );
		
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
	
	App::App()
	{
		using Ped::apple_events_present;
		
		SetCommandHandler( Ped::kCmdAbout, &About       );
		SetCommandHandler( Ped::kCmdNew,   &NewDocument );
		
		if ( TARGET_CPU_68K  &&  ! apple_events_present )
		{
			return;
		}
		
		OpenDocuments_AppleEvent::Install_Handler();
		
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
		
		Document_Token::Install_DataGetter();
		
		AppName_Property     ::Install_Accessor();
		AppFrontmost_Property::Install_Accessor();
		
		DocName_Property::Install_Accessor();
	}
	
}

int main( void )
{
	TestEdit::App app;
	
	return app.Run();
}
