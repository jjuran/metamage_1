/*	======
 *	App.cc
 *	======
 */

// Debug
#include "debug/assert.hh"

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

// Pedestal
#include "Pedestal/AboutBox.hh"
#include "Pedestal/Commands.hh"

// UseEdit
#include "UseEdit/App.hh"
#include "UseEdit/Document.hh"


namespace UseEdit
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Ped = Pedestal;
	
	
	App* App::theApp = NULL;
	
	
	static const N::DescType typeDocument = N::DescType( 'Doc ' );
	
	
	static DocumentsOwner gDocuments;
	
	
	namespace
	{
		
		// Apple event handlers
		
		void HandleCloseAppleEvent( const N::AppleEvent&  appleEvent,
		                            N::AppleEvent&        reply,
		                            App*                  app )
		{
			ASSERT( app != NULL );
			
			NN::Owned< N::AEDesc_Token > token = N::AEResolve( N::AEGetParamDesc( appleEvent,
			                                                                      N::keyDirectObject ) );
			
			switch ( N::DescType( token.Get().descriptorType ) )
			{
				case typeDocument:
					if ( N::WindowRef window = static_cast< ::WindowRef >( N::AEGetDescData< N::typePtr >( token, typeDocument ) ) )
					{
						if ( Ped::Window* base = N::GetWRefCon( window ) )
						{
							base->Close( window );
						}
					}
					break;
				
				default:
					N::ThrowOSStatus( errAEEventNotHandled );
					break;
			}
		}
		
		void HandleCountAppleEvent( const N::AppleEvent&  appleEvent,
		                            N::AppleEvent&        reply,
		                            App*                  app )
		{
			ASSERT( app != NULL );
			
			NN::Owned< N::AEDesc_ObjectSpecifier > containerObjSpec = N::AEGetParamDesc( appleEvent,
			                                                                             N::keyDirectObject );
			
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
			                   N::keyDirectObject,
			                   N::AECreateDesc< N::typeUInt32 >( count ) );
		}
		
		void HandleGetDataAppleEvent( const N::AppleEvent&  appleEvent,
		                              N::AppleEvent&        reply,
		                              App*                  app )
		{
			ASSERT( app != NULL );
			
			N::AEPutParamDesc( reply,
			                   N::keyDirectObject,
			                   N::GetData( N::AEResolve( N::AEGetParamDesc( appleEvent,
			                                                                N::keyDirectObject ) ) ) );
		}
		
		void HandleOpenDocumentsAppleEvent( const N::AppleEvent&  appleEvent,
											N::AppleEvent&        reply,
											App*                  app )
		{
			ASSERT( app != NULL );
			
			typedef N::AEDescList_ItemDataValue_Container< Io_Details::typeFileSpec > Container;
			typedef Container::const_iterator const_iterator;
			
			NN::Owned< N::AEDescList_Data > docList = N::AEGetParamDesc( appleEvent,
			                                                             N::keyDirectObject,
			                                                             N::typeAEList );
			
			Container listData = N::AEDescList_ItemDataValues< Io_Details::typeFileSpec >( docList );
			
			for ( const_iterator it = listData.begin();  it != listData.end();  ++it )
			{
				Io_Details::file_spec fileSpec = *it;
				
				gDocuments.OpenDocument( fileSpec );
			}
			
		}
		
		// Object accessors
		
		NN::Owned< N::AEDesc_Token > AccessAppFrontmost( N::AEPropertyID         propertyID,
	                                                     const N::AEDesc_Token&  containerToken,
	                                                     N::AEObjectClass        containerClass )
		{
			
			return N::AECreateDesc< N::typeBoolean, N::AEDesc_Token >( N::SameProcess( N::CurrentProcess(), N::GetFrontProcess() ) );
		}
		
		NN::Owned< N::AEDesc_Token > AccessAppName( N::AEPropertyID         propertyID,
	                                                const N::AEDesc_Token&  containerToken,
	                                                N::AEObjectClass        containerClass )
		{
			
			return N::AECreateDesc< N::typeChar, N::AEDesc_Token >( "UseEdit" );
		}
		
		static NN::Owned< N::AEDesc_Token > TokenForDocument( const Document& document )
		{
			return N::AECreateDesc( typeDocument, N::AECreateDesc< N::typePtr, N::AEDesc_Token >( document.GetWindowRef() ) );
		}
		
		NN::Owned< N::AEDesc_Token > AccessDocument( N::AEObjectClass        desiredClass,
	                                                 const N::AEDesc_Token&  containerToken,
	                                                 N::AEObjectClass        containerClass,
	                                                 N::AEEnumerated         keyForm,
	                                                 const N::AEDesc_Data&   keyData,
	                                                 N::RefCon )
		{
			const DocumentContainer& docs( gDocuments.Documents() );
			
			if ( keyForm == N::formUniqueID )
			{
				return docs.GetElementByID( N::AEGetDescData< N::typeUInt32 >( keyData ) );
			}
			
			if ( keyForm == N::formAbsolutePosition )
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
					              docs.GetElementByIndex( i ) );
				}
				
				return list;
			}
			
			// Unsupported key form
			N::ThrowOSStatus( errAEEventNotHandled );
			
			return NN::Owned< N::AEDesc_Token >();
		}
		
		NN::Owned< N::AEDesc_Token > AccessDocName( N::AEPropertyID         propertyID,
	                                                const N::AEDesc_Token&  containerToken,
	                                                N::AEObjectClass        containerClass )
		{
			UInt32 id = N::AEGetDescData< N::typeUInt32 >( containerToken, typeDocument );
			
			const Document& document = gDocuments.Documents().GetDocumentByID( id );
			
			return N::AECreateDesc< N::typeChar, N::AEDesc_Token >( document.GetName() );
		}
		
		// Count
		
		std::size_t CountDocuments( N::AEObjectClass        desiredClass,
		                            N::AEObjectClass        containerClass,
		                            const N::AEDesc_Token&  containerToken )
		{
			return gDocuments.Documents().CountElements();
		}
		
		// Get data
		
		NN::Owned< N::AEDesc_Data > GetLiteralData( const N::AEDesc_Token& obj, N::DescType /*desiredType*/ )
		{
			return N::AEDuplicateDesc( obj );
		}
		
		NN::Owned< N::AEDesc_Data > GetDocument( const N::AEDesc_Token& obj, N::DescType /*desiredType*/ )
		{
			N::AEDesc keyData = obj;
			
			keyData.descriptorType = typeUInt32;
			
			return N::AECreateObjectSpecifier( N::cDocument,
			                                   N::GetRootObjectSpecifier(),
			                                   N::formUniqueID,
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
			N::ThrowOSStatus( errAENoSuchObject );
		}
	}
	
	
	const Document& DocumentContainer::GetDocumentByIndex( std::size_t index ) const
	{
		if ( !ExistsElementByIndex( index ) )
		{
			N::ThrowOSStatus( errAENoSuchObject );
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
	
	void DocumentContainer::StoreNewElement( const boost::shared_ptr< Document >& document )
	{
		itsMap[ document->GetWindowRef() ] = document;
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
			N::ThrowOSStatus( errAENoSuchObject );
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
				gDocuments.CloseDocument( window );
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
	
	void DocumentsOwner::StoreNewDocument( Document* doc )
	{
		boost::shared_ptr< Document > document( doc );
		
		document->GetWindow().SetCloseHandler( itsCloseHandler );
		
		itsDocuments.StoreNewElement( document );
	}
	
	void DocumentsOwner::NewWindow()
	{
		StoreNewDocument( new Document );
	}
	
	void DocumentsOwner::OpenDocument( const Io_Details::file_spec& file )
	{
		StoreNewDocument( new Document( file ) );
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
	
	static bool NewDocument( Ped::CommandCode )
	{
		gDocuments.NewWindow();
		
		return true;
	}
	
	App::App()
	: 
		itsOpenDocsHandler( InstallAppleEventHandler< HandleOpenDocumentsAppleEvent >( N::kCoreEventClass, N::kAEOpenDocuments ) ),
		itsCloseHandler   ( InstallAppleEventHandler< HandleCloseAppleEvent   >( N::kAECoreSuite, N::kAEClose         ) ),
		itsCountHandler   ( InstallAppleEventHandler< HandleCountAppleEvent   >( N::kAECoreSuite, N::kAECountElements ) ),
		itsGetDataHandler ( InstallAppleEventHandler< HandleGetDataAppleEvent >( N::kAECoreSuite, N::kAEGetData       ) )
	{
		ASSERT( theApp == NULL );
		
		theApp = this;
		
		SetCommandHandler( Ped::kCmdAbout, &About       );
		SetCommandHandler( Ped::kCmdNew,   &NewDocument );
		
		// Initialize the Object Support Library.
		N::AEObjectInit();
		
		// List multiplexor, e.g. for 'get name of every window'
		N::AEInstallObjectAccessor< N::DispatchAccessToList >( N::AEObjectClass( typeWildCard ), N::typeAEList ).Release();
		
		// Property accessors
		N::AEInstallObjectAccessor< N::DispatchPropertyAccess >( N::cProperty, N::typeNull  ).Release();
		N::AEInstallObjectAccessor< N::DispatchPropertyAccess >( N::cProperty, typeDocument ).Release();
		
		// Document accessor
		N::AEInstallObjectAccessor< AccessDocument >( N::cDocument, N::typeNull ).Release();
		
		// Set up AEObjectModel
		N::AESetObjectCallbacks();
		
		// Count documents
		N::RegisterCounter( N::cDocument, N::typeNull, CountDocuments );
		
		// Literal data tokens
		N::RegisterDataGetter( N::typeBoolean,  GetLiteralData );
		N::RegisterDataGetter( N::typeChar,     GetLiteralData );
		N::RegisterDataGetter( N::typeAERecord, GetLiteralData );
		
		// Specify a document given a token
		N::RegisterDataGetter( typeDocument, GetDocument );
		
		// Name of app
		N::RegisterPropertyAccessor( N::pName,           N::typeNull, AccessAppName );
		N::RegisterPropertyAccessor( N::pIsFrontProcess, N::typeNull, AccessAppFrontmost );
		
		// Name of document
		N::RegisterPropertyAccessor( N::pName, typeDocument, AccessDocName );
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

