/*	======
 *	App.hh
 *	======
 */

#ifndef USEEDIT_APP_HH
#define USEEDIT_APP_HH

// Standard C++
#include <map>

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/shared_ptr.hpp>

// MacFiles
#include "MacFiles/Classic.hh"
#include "MacFiles/Unicode.hh"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/Window.hh"


namespace UseEdit
{
	
	struct FSSpec_Io_Details : public Nitrogen::FSSpec_Io_Details
	{
		static const Nitrogen::DescType typeFileSpec = Nitrogen::typeFSS;
	};
	
	struct FSRef_Io_Details : public Nitrogen::FSRef_Io_Details
	{
		static const Nitrogen::DescType typeFileSpec = Nitrogen::typeFSRef;
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
			
			void StoreNewElement( const boost::shared_ptr< Document >& document );
			
			size_t CountElements() const  { return itsMap.size(); }
			
			bool ExistsElementByIndex( std::size_t index ) const  { return index <= CountElements(); }
			bool ExistsElementByID   ( UInt32      id    ) const;
			
			Nucleus::Owned< Nitrogen::AEDesc_Token > GetElementByIndex( std::size_t index ) const;
			Nucleus::Owned< Nitrogen::AEDesc_Token > GetElementByID   ( UInt32      id    ) const;
			
			void DeleteElementByIndex( std::size_t index );
			
			void DeleteElementByID( UInt32 id );
	};
	
	class DocumentsOwner
	{
		private:
			DocumentContainer                                  itsDocuments;
			boost::shared_ptr< Pedestal::WindowCloseHandler >  itsCloseHandler;
			
			void StoreNewDocument( Document* doc );
		
		public:
			DocumentsOwner();
			~DocumentsOwner();
			
			void CloseDocument( Nitrogen::WindowRef window );
			
			DocumentContainer& Documents()  { return itsDocuments; }
			
			void NewWindow();
			void OpenDocument( const Io_Details::file_spec& file );
	};
	
	
	class App : public Pedestal::Application
	{
		private:
			static App* theApp;
			
			typedef void (*AEEventHandlerProcPtr)( Nitrogen::AppleEvent const&, Nitrogen::AppleEvent&, App* );
			
			Nucleus::Owned< Nitrogen::AEEventHandler > itsOpenDocsHandler;
			Nucleus::Owned< Nitrogen::AEEventHandler > itsCloseHandler;
			Nucleus::Owned< Nitrogen::AEEventHandler > itsCountHandler;
			Nucleus::Owned< Nitrogen::AEEventHandler > itsGetDataHandler;
			
			template < AEEventHandlerProcPtr proc >
			inline
			Nucleus::Owned< Nitrogen::AEEventHandler >
			InstallAppleEventHandler( Nitrogen::AEEventClass  eventClass,
			                          Nitrogen::AEEventID     eventID )
			{
				return Nitrogen::AEInstallEventHandler< App*, proc >( eventClass, eventID, this );
			}
		
		public:
			static App& Get();
			
			App();
			~App();
	};
	
}

#endif

