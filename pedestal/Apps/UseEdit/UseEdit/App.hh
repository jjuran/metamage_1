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

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/AboutBox.hh"


namespace UseEdit
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace FS = FileSystem;
	namespace Ped = Pedestal;
	
	
	class Document;
	
	
	class DocumentContainer
	{
		private:
			typedef std::map< ::WindowRef, boost::shared_ptr< Document > > Map;
			
			Map map;
		
		public:
			~DocumentContainer();
			
			const Document& GetDocumentByIndex( std::size_t index ) const;
			const Document& GetDocumentByID   ( UInt32      id    ) const;
			
			void StoreNewElement( Document* doc );
			
			size_t CountElements() const  { return map.size(); }
			
			bool ExistsElementByIndex( std::size_t index ) const  { return index <= CountElements(); }
			bool ExistsElementByID   ( UInt32      id    ) const;
			
			NN::Owned< N::AEToken, N::AETokenDisposer > GetElementByIndex( std::size_t index ) const;
			NN::Owned< N::AEToken, N::AETokenDisposer > GetElementByID   ( UInt32      id    ) const;
			
			void DeleteElementByIndex( std::size_t index );
			
			void DeleteElementByID( UInt32 id );
	};
	
	class DocumentsOwner : private Ped::WindowClosure
	{
		private:
			DocumentContainer documents;
		
		public:
			~DocumentsOwner();
			
			bool RequestWindowClosure( N::WindowRef window );
			
			DocumentContainer& Documents()  { return documents; }
			
			void NewWindow();
			void OpenDocument( const FS::Spec& file );
	};
	
	class App : public Ped::Application,
	            public Ped::AboutBoxOwner,
	            public DocumentsOwner
	{
		private:
			static App* theApp;
			
			Ped::AboutHandler< App > aboutHandler;
			Ped::NewHandler  < App > newHandler;
			NN::Owned< N::AEEventHandler > myOpenDocsEventHandler;
			NN::Owned< N::AEEventHandler > myCountHandler;
			NN::Owned< N::AEEventHandler > myGetDataHandler;
		
		public:
			static App& Get();
			
			App();
			~App();
	};
	
}

#endif

