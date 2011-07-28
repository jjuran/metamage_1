/*
	App.cc
	------
*/

// Standard C++
#include <algorithm>

// iota
#include "iota/convert_string.hh"

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Nitrogen/AEDataModel.hh"
#include "Nitrogen/AEObjects.hh"
#include "Nitrogen/CarbonEvents.hh"
#include "Nitrogen/MacWindows.hh"
#include "Nitrogen/Processes.hh"

// Iteration
#include "Iteration/AEDescListItemDatas.h"

// AEObjectModel
#include "AEObjectModel/AccessProperty.h"
#include "AEObjectModel/AEObjectModel.h"
#include "AEObjectModel/Count.h"
#include "AEObjectModel/GetData.h"
#include "AEObjectModel/GetObjectClass.h"

// Pedestal
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Commands.hh"

// UseEdit
#include "UseEdit/App.hh"
#include "UseEdit/Document.hh"


namespace UseEdit
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	static const Mac::DescType typeDocument = Mac::DescType( 'Doc ' );
	
}

namespace Mac
{
	
	template <> struct DescType_scribe< UseEdit::typeDocument > : DescType_scribe< typeWindowRef > {};
	
}

namespace UseEdit
{
	
	static DocumentContainer gDocuments;
	
	
	static void StoreNewDocument( Document* doc );
	
	
	static void CloseDocument( WindowRef window )
	{
		if ( Ped::Window* base = N::GetWRefCon( window ) )
		{
			base->Close( window );
		}
	}
	
	struct DocumentCloser
	{
		void operator()( WindowRef window ) const
		{
			CloseDocument( window );
		}
	};
	
	template < class Container >
	static void CloseDocuments( const Container& container )
	{
		std::for_each( container.begin(),
		               container.end(),
		               DocumentCloser() );
	}
	
	// Apple event handlers
	
	struct Close_AppleEvent
	{
		static void Handler( Mac::AppleEvent const&  event,
		                     Mac::AppleEvent&        reply )
		{
			n::owned< Mac::AEDesc_Token > token = N::AEResolve( N::AEGetParamDesc( event,
			                                                                       Mac::keyDirectObject ) );
			
			switch ( Mac::DescType( token.get().descriptorType ) )
			{
				case typeDocument:
					if ( WindowRef window = static_cast< ::WindowRef >( N::AEGetDescData< Mac::typePtr >( token, typeDocument ) ) )
					{
						CloseDocument( window );
					}
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
				
				StoreNewDocument( new Document( fileSpec ) );
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
			return N::AECreateDesc< Mac::typeBoolean, Mac::AEDesc_Token >( N::SameProcess( N::CurrentProcess(), N::GetFrontProcess() ) );
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
			return N::AECreateDesc< Mac::typeChar, Mac::AEDesc_Token >( "UseEdit" );
		}
		
		static void Install_Accessor()
		{
			N::RegisterPropertyAccessor( Mac::pName, Mac::typeNull, Accessor );
		}
	};
	
	static n::owned< Mac::AEDesc_Token > TokenForDocument( const Document& document )
	{
		return N::AECreateDesc( typeDocument, N::AECreateDesc< Mac::typePtr, Mac::AEDesc_Token >( document.GetWindowRef() ) );
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
				return gDocuments.GetElementByID( N::AEGetDescData< Mac::typeUInt32 >( keyData ) );
			}
			
			if ( keyForm == Mac::formAbsolutePosition )
			{
				std::size_t count = gDocuments.CountElements();
				
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
			UInt32 id = N::AEGetDescData< Mac::typeUInt32 >( containerToken, typeDocument );
			
			const Document& document = gDocuments.GetDocumentByID( id );
			
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
			return gDocuments.CountElements();
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
			AEDesc keyData = obj;
			
			keyData.descriptorType = typeUInt32;
			
			return N::AECreateObjectSpecifier( Mac::cDocument,
			                                   N::GetRootObjectSpecifier(),
			                                   Mac::formUniqueID,
			                                   static_cast< const Mac::AEDesc_Data& >( keyData ) );
		}
		
		static void Install_DataGetter()
		{
			N::RegisterDataGetter( typeDocument, Get );
		}
	};
	
	
	DocumentContainer::~DocumentContainer()
	{
	}
	
	
	inline DocumentContainer::Map::const_iterator DocumentContainer::Find( UInt32 id ) const
	{
		Map::const_iterator it = itsMap.find( reinterpret_cast< ::WindowRef >( id ) );
		
		return it;
	}
	
	inline DocumentContainer::Map::iterator DocumentContainer::Find( UInt32 id )
	{
		Map::iterator it = itsMap.find( reinterpret_cast< ::WindowRef >( id ) );
		
		return it;
	}
	
	inline void DocumentContainer::ThrowIfNoSuchObject( Map::const_iterator it ) const
	{
		if ( it == itsMap.end() )
		{
			Mac::ThrowOSStatus( errAENoSuchObject );
		}
	}
	
	
	const Document& DocumentContainer::GetDocumentByIndex( std::size_t index ) const
	{
		if ( !ExistsElementByIndex( index ) )
		{
			Mac::ThrowOSStatus( errAENoSuchObject );
		}
		
		Map::const_iterator it = itsMap.begin();
		
		std::advance( it, index - 1 );
		
		return *it->second.get();
	}
	
	const Document& DocumentContainer::GetDocumentByID( UInt32 id ) const
	{
		Map::const_iterator it = Find( id );
		
		ThrowIfNoSuchObject( it );
		
		return *it->second.get();
	}
	
	void DocumentContainer::StoreNewElement( const boost::intrusive_ptr< Document >& document )
	{
		itsMap[ document->GetWindowRef() ] = document;
	}
	
	bool DocumentContainer::ExistsElementByID( UInt32 id ) const
	{
		return Find( id ) != itsMap.end();
	}
	
	n::owned< Mac::AEDesc_Token > DocumentContainer::GetElementByIndex( std::size_t index ) const
	{
		return TokenForDocument( GetDocumentByIndex( index ) );
	}
	
	n::owned< Mac::AEDesc_Token > DocumentContainer::GetElementByID( UInt32 id ) const
	{
		return TokenForDocument( GetDocumentByID( id ) );
	}
	
	void DocumentContainer::DeleteElementByIndex( std::size_t index )
	{
		if ( !ExistsElementByIndex( index ) )
		{
			Mac::ThrowOSStatus( errAENoSuchObject );
		}
		
		Map::iterator it = itsMap.begin();
		
		std::advance( it, index - 1 );
		
		itsMap.erase( it );
	}
	
	void DocumentContainer::DeleteElementByID( UInt32 id )
	{
		Map::iterator it = Find( id );
		
		ThrowIfNoSuchObject( it );
		
		itsMap.erase( it );
	}
	
	
	class DocumentCloseHandler : public Ped::WindowCloseHandler
	{
		public:
			void operator()( WindowRef window ) const
			{
				gDocuments.DeleteElementByID( (UInt32) window );  // reinterpret_cast
			}
	};
	
	static boost::intrusive_ptr< Pedestal::WindowCloseHandler > gDocumentCloseHandler = new DocumentCloseHandler;
	
	
	static void StoreNewDocument( Document* doc )
	{
		boost::intrusive_ptr< Document > document( doc );
		
		document->GetWindow().SetCloseHandler( gDocumentCloseHandler );
		
		gDocuments.StoreNewElement( document );
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
		SetCommandHandler( Ped::kCmdAbout, &About       );
		SetCommandHandler( Ped::kCmdNew,   &NewDocument );
		
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
	
	App::~App()
	{
	}
	
}

int main( void )
{
	UseEdit::App app;
	
	return app.Run();
}

