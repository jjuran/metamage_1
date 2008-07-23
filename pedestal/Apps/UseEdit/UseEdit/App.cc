/*	======
 *	App.cc
 *	======
 */

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/AEDataModel.h"
#include "Nitrogen/AERegistry.h"

// Iteration
#include "Iteration/AEDescListItemDatas.h"

// AEObjectModel
#include "AEObjectModel/AccessProperty.h"
#include "AEObjectModel/AEObjectModel.h"
#include "AEObjectModel/Count.h"
#include "AEObjectModel/GetData.h"
#include "AEObjectModel/GetObjectClass.h"

// UseEdit
#include "UseEdit/App.hh"
#include "UseEdit/Document.hh"


namespace UseEdit
{
	
	namespace N = Nitrogen;
	
	using N::keyDirectObject;
	using N::keyAEObjectClass;
	using N::typeWildCard;
	using N::typePtr;
	using N::typeUInt32;
	using N::typeAEList;
	using N::typeChar;
	using N::typeNull;
	using N::typeAERecord;
	using N::formUniqueID;
	using N::formAbsolutePosition;
	using N::kCoreEventClass;
	using N::kAEOpenDocuments;
	using N::kAECoreSuite;
	using N::kAEClose;
	using N::kAECountElements;
	using N::kAEGetData;
	using N::cProperty;
	using N::cDocument;
	using N::pName;
	using N::pIsFrontProcess;
	
	App* App::theApp = NULL;
	
	
	static const N::DescType typeDocument = N::DescType( 'Doc ' );
	
	
	namespace
	{
		
		// Apple event handlers
		
		void HandleCloseAppleEvent( const N::AppleEvent&  appleEvent,
		                            N::AppleEvent&        reply,
		                            App*                  app )
		{
			ASSERT( app != NULL );
			
			NN::Owned< N::AEDesc_Token > token = N::AEResolve( N::AEGetParamDesc( appleEvent,
			                                                                      keyDirectObject ) );
			
			switch ( N::DescType( token.Get().descriptorType ) )
			{
				case typeDocument:
					if ( N::WindowRef window = static_cast< ::WindowRef >( N::AEGetDescData< typePtr >( token, typeDocument ) ) )
					{
						if ( Ped::WindowBase* base = N::GetWRefCon( window ) )
						{
							base->Close( window );
						}
					}
					break;
				
				default:
					throw N::ErrAEEventNotHandled();
					break;
			}
		}
		
		void HandleCountAppleEvent( const N::AppleEvent&  appleEvent,
		                            N::AppleEvent&        reply,
		                            App*                  app )
		{
			ASSERT( app != NULL );
			
			NN::Owned< N::AEDesc_ObjectSpecifier > containerObjSpec = N::AEGetParamDesc( appleEvent,
			                                                                             keyDirectObject );
			
			bool containerIsRoot = containerObjSpec.Get().descriptorType == typeNull;
			
			// AEResolve can't handle a null descriptor.
			NN::Owned< N::AEDesc_Token > containerToken = containerIsRoot ? N::GetRootToken()
			                                                              : N::AEResolve( containerObjSpec );
			// The kind of container of the things we're counting, e.g. 'folder'
			N::AEObjectClass containerClass = N::GetObjectClass( containerToken );
			
			// The kind of thing we're counting, e.g. 'file'
			N::AEObjectClass desiredClass = N::AEGetParamPtr< N::keyAEObjectClass >( appleEvent );
			
			std::size_t count = N::Count( desiredClass, containerClass, containerToken );
			
			N::AEPutParamDesc( reply,
			                   keyDirectObject,
			                   N::AECreateDesc< N::AEDesc_Data, typeUInt32 >( count ) );
		}
		
		void HandleGetDataAppleEvent( const N::AppleEvent&  appleEvent,
		                              N::AppleEvent&        reply,
		                              App*                  app )
		{
			ASSERT( app != NULL );
			
			N::AEPutParamDesc( reply,
			                   keyDirectObject,
			                   N::GetData( N::AEResolve( N::AEGetParamDesc( appleEvent,
			                                                                keyDirectObject ) ) ) );
		}
		
		void HandleOpenDocumentsAppleEvent( const N::AppleEvent&  appleEvent,
											N::AppleEvent&        reply,
											App*                  app )
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
		
		// Object accessors
		
		NN::Owned< N::AEDesc_Token > AccessAppFrontmost( N::AEPropertyID         propertyID,
	                                                     const N::AEDesc_Token&  containerToken,
	                                                     N::AEObjectClass        containerClass )
		{
			
			return N::AECreateDesc< N::AEDesc_Token, N::typeBoolean >( N::SameProcess( N::CurrentProcess(), N::GetFrontProcess() ) );
		}
		
		NN::Owned< N::AEDesc_Token > AccessAppName( N::AEPropertyID         propertyID,
	                                                const N::AEDesc_Token&  containerToken,
	                                                N::AEObjectClass        containerClass )
		{
			
			return N::AECreateDesc< N::AEDesc_Token, typeChar >( "UseEdit" );
		}
		
		static NN::Owned< N::AEDesc_Token > TokenForDocument( const Document& document )
		{
			return N::AECreateDesc( typeDocument, N::AECreateDesc< N::AEDesc_Token, typePtr >( document.GetWindowRef() ) );
		}
		
		NN::Owned< N::AEDesc_Token > AccessDocument( N::AEObjectClass        desiredClass,
	                                                 const N::AEDesc_Token&  containerToken,
	                                                 N::AEObjectClass        containerClass,
	                                                 N::AEEnumerated         keyForm,
	                                                 const N::AEDesc_Data&   keyData,
	                                                 N::RefCon )
		{
			const DocumentContainer& docs( App::Get().Documents() );
			
			if ( keyForm == formUniqueID )
			{
				return docs.GetElementByID( N::AEGetDescData< typeUInt32 >( keyData ) );
			}
			
			if ( keyForm == formAbsolutePosition )
			{
				std::size_t count = docs.CountElements();
				
				UInt32 index = N::ComputeAbsoluteIndex( keyData, count );
				
				if ( index > 0 )
				{
					return docs.GetElementByIndex( index );
				}
				
				// All documents
				NN::Owned< N::AEDescList_Token > list = N::AECreateList< N::AEDescList_Token >( false );
				
				for ( UInt32 i = 1;  i <= count;  ++i )
				{
					N::AEPutDesc( list,
					              0,
					              docs.GetElementByIndex( i ).Get() );
				}
				
				return list;
			}
			
			// Unsupported key form
			throw N::ErrAEEventNotHandled();
			
			return NN::Owned< N::AEDesc_Token >();
		}
		
		NN::Owned< N::AEDesc_Token > AccessDocName( N::AEPropertyID         propertyID,
	                                                const N::AEDesc_Token&  containerToken,
	                                                N::AEObjectClass        containerClass )
		{
			UInt32 id = N::AEGetDescData< typeUInt32 >( containerToken, typeDocument );
			
			const Document& document = App::Get().Documents().GetDocumentByID( id );
			
			return N::AECreateDesc< N::AEDesc_Token, typeChar >( document.GetName() );
		}
		
		// Count
		
		std::size_t CountDocuments( N::AEObjectClass        desiredClass,
		                            N::AEObjectClass        containerClass,
		                            const N::AEDesc_Token&  containerToken )
		{
			return App::Get().Documents().CountElements();
		}
		
		// Get data
		
		NN::Owned< N::AEDesc_Data > GetLiteralData( const N::AEDesc_Token& obj, N::DescType /*desiredType*/ )
		{
			return N::AEDuplicateDesc< N::AEDesc_Data >( obj );
		}
		
		NN::Owned< N::AEDesc_Data > GetDocument( const N::AEDesc_Token& obj, N::DescType /*desiredType*/ )
		{
			N::AEDesc keyData = obj;
			
			keyData.descriptorType = typeUInt32;
			
			return N::AECreateObjectSpecifier( cDocument,
			                                   N::GetRootObjectSpecifier(),
			                                   formUniqueID,
			                                   static_cast< const N::AEDesc_Data& >( keyData ) );
		}
		
	}
	
	
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
			throw N::ErrAENoSuchObject();
		}
	}
	
	
	const Document& DocumentContainer::GetDocumentByIndex( std::size_t index ) const
	{
		if ( !ExistsElementByIndex( index ) )
		{
			throw N::ErrAENoSuchObject();
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
	
	void DocumentContainer::StoreNewElement( Document* doc )
	{
		itsMap[ doc->GetWindowRef() ] = boost::shared_ptr< Document >( doc );
	}
	
	bool DocumentContainer::ExistsElementByID( UInt32 id ) const
	{
		return Find( id ) != itsMap.end();
	}
	
	NN::Owned< N::AEDesc_Token > DocumentContainer::GetElementByIndex( std::size_t index ) const
	{
		return TokenForDocument( GetDocumentByIndex( index ) );
	}
	
	NN::Owned< N::AEDesc_Token > DocumentContainer::GetElementByID( UInt32 id ) const
	{
		return TokenForDocument( GetDocumentByID( id ) );
	}
	
	void DocumentContainer::DeleteElementByIndex( std::size_t index )
	{
		if ( !ExistsElementByIndex( index ) )
		{
			throw N::ErrAENoSuchObject();
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
			void operator()( N::WindowRef window ) const
			{
				App::Get().CloseDocument( window );
			}
	};
	
	
	
	DocumentsOwner::DocumentsOwner() : itsCloseHandler( new DocumentCloseHandler() )
	{
	}
	
	DocumentsOwner::~DocumentsOwner()
	{
	}
	
	void DocumentsOwner::CloseDocument( N::WindowRef window )
	{
		itsDocuments.DeleteElementByID( reinterpret_cast< UInt32 >( ::WindowRef( window ) ) );
	}
	
	void DocumentsOwner::NewWindow()
	{
		Document* doc = new Document( itsCloseHandler );
		
		itsDocuments.StoreNewElement( doc );
	}
	
	void DocumentsOwner::OpenDocument( const Io_Details::file_spec& file )
	{
		Document* doc = new Document( itsCloseHandler, file );
		
		itsDocuments.StoreNewElement( doc );
	}
	
	App& App::Get()
	{
		ASSERT( theApp != NULL );
		
		return *theApp;
	}
	
	App::App()
	: 
		itsAboutHandler( *this ),
		itsNewHandler  ( *this ),
		itsOpenDocsEventHandler( InstallAppleEventHandler< HandleOpenDocumentsAppleEvent >( kCoreEventClass, kAEOpenDocuments ) ),
		itsCloseHandler  ( InstallAppleEventHandler< HandleCloseAppleEvent   >( kAECoreSuite, kAEClose         ) ),
		itsCountHandler  ( InstallAppleEventHandler< HandleCountAppleEvent   >( kAECoreSuite, kAECountElements ) ),
		itsGetDataHandler( InstallAppleEventHandler< HandleGetDataAppleEvent >( kAECoreSuite, kAEGetData       ) )
	{
		ASSERT( theApp == NULL );
		
		theApp = this;
		
		RegisterMenuItemHandler( 'abou', &itsAboutHandler );
		RegisterMenuItemHandler( 'new ', &itsNewHandler   );
		
		// Initialize the Object Support Library.
		N::AEObjectInit();
		
		// List multiplexor, e.g. for 'get name of every window'
		N::AEInstallObjectAccessor< N::DispatchAccessToList >( N::AEObjectClass( typeWildCard ), typeAEList ).Release();
		
		// Property accessors
		N::AEInstallObjectAccessor< N::DispatchPropertyAccess >( cProperty, typeNull     ).Release();
		N::AEInstallObjectAccessor< N::DispatchPropertyAccess >( cProperty, typeDocument ).Release();
		
		// Document accessor
		N::AEInstallObjectAccessor< AccessDocument >( cDocument, typeNull ).Release();
		
		// Set up AEObjectModel
		N::AESetObjectCallbacks();
		
		// Count documents
		N::RegisterCounter( cDocument, typeNull, CountDocuments );
		
		// Literal data tokens
		N::RegisterDataGetter( typeChar,     GetLiteralData );
		N::RegisterDataGetter( typeAERecord, GetLiteralData );
		
		// Specify a document given a token
		N::RegisterDataGetter( typeDocument, GetDocument );
		
		// Name of app
		N::RegisterPropertyAccessor( pName,           typeNull, AccessAppName );
		N::RegisterPropertyAccessor( pIsFrontProcess, typeNull, AccessAppFrontmost );
		
		// Name of document
		N::RegisterPropertyAccessor( pName, typeDocument, AccessDocName );
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

