/*	======
 *	App.hh
 *	======
 */

#ifndef USEEDIT_APP_HH
#define USEEDIT_APP_HH

// Standard C++
#include <map>

// Boost
#include <boost/shared_ptr.hpp>

// Nucleus
#include "Nucleus/Owned.h"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/AboutBox.hh"


namespace UseEdit
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Ped = Pedestal;
	
	
	struct FSSpec_Io_Details : public N::FSSpec_Io_Details
	{
		static const N::DescType typeFileSpec = N::typeFSS;
	};
	
	struct FSRef_Io_Details : public N::FSRef_Io_Details
	{
		static const N::DescType typeFileSpec = N::typeFSRef;
	};
	
#if TARGET_API_MAC_CARBON
	
	typedef FSRef_Io_Details Io_Details;
	
#else
	
	typedef FSSpec_Io_Details Io_Details;
	
#endif
	
	class Document;
	
	
	class DocumentContainer
	{
		private:
			typedef std::map< ::WindowRef, boost::shared_ptr< Document > > Map;
			
			Map itsMap;
			
			Map::const_iterator Find( UInt32 id ) const;
			Map::      iterator Find( UInt32 id );
			
			void ThrowIfNoSuchObject( Map::const_iterator it ) const;
		
		public:
			~DocumentContainer();
			
			const Document& GetDocumentByIndex( std::size_t index ) const;
			const Document& GetDocumentByID   ( UInt32      id    ) const;
			
			void StoreNewElement( Document* doc );
			
			size_t CountElements() const  { return itsMap.size(); }
			
			bool ExistsElementByIndex( std::size_t index ) const  { return index <= CountElements(); }
			bool ExistsElementByID   ( UInt32      id    ) const;
			
			NN::Owned< N::AEDesc_Token > GetElementByIndex( std::size_t index ) const;
			NN::Owned< N::AEDesc_Token > GetElementByID   ( UInt32      id    ) const;
			
			void DeleteElementByIndex( std::size_t index );
			
			void DeleteElementByID( UInt32 id );
	};
	
	class DocumentsOwner
	{
		private:
			DocumentContainer                             itsDocuments;
			boost::shared_ptr< Ped::WindowCloseHandler >  itsCloseHandler;
		
		public:
			DocumentsOwner();
			~DocumentsOwner();
			
			void CloseDocument( N::WindowRef window );
			
			DocumentContainer& Documents()  { return itsDocuments; }
			
			void NewWindow();
			void OpenDocument( const Io_Details::file_spec& file );
	};
	
	class App : public Ped::Application,
	            public Ped::AboutBoxOwner,
	            public DocumentsOwner
	{
		private:
			static App* theApp;
			
			typedef void (*AEEventHandlerProcPtr)( N::AppleEvent const&, N::AppleEvent&, App* );
			
			Ped::AboutHandler< App > itsAboutHandler;
			Ped::NewHandler  < App > itsNewHandler;
			NN::Owned< N::AEEventHandler > itsOpenDocsEventHandler;
			NN::Owned< N::AEEventHandler > itsCloseHandler;
			NN::Owned< N::AEEventHandler > itsCountHandler;
			NN::Owned< N::AEEventHandler > itsGetDataHandler;
			
			template < AEEventHandlerProcPtr proc >
			inline NN::Owned< N::AEEventHandler > InstallAppleEventHandler( N::AEEventClass  eventClass,
			                                                                N::AEEventID     eventID )
			{
				return N::AEInstallEventHandler< App*, proc >( eventClass, eventID, this );
			}
		
		public:
			static App& Get();
			
			App();
			~App();
	};
	
}

#endif

