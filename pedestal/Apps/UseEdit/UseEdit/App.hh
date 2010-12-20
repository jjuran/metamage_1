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
#include <boost/intrusive_ptr.hpp>

// Nitrogen
#ifndef NITROGEN_APPLEEVENTS_HH
#include "Nitrogen/AppleEvents.hh"
#endif

// MacFiles
#include "MacFiles/Classic.hh"
#include "MacFiles/Unicode.hh"

// Pedestal
#ifndef PEDESTAL_APPLICATION_HH
#include "Pedestal/Application.hh"
#endif
#ifndef PEDESTAL_WINDOW_HH
#include "Pedestal/Window.hh"
#endif


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
			typedef std::map< ::WindowRef, boost::intrusive_ptr< Document > > Map;
			
			Map itsMap;
			
			Map::const_iterator Find( UInt32 id ) const;
			Map::      iterator Find( UInt32 id );
			
			void ThrowIfNoSuchObject( Map::const_iterator it ) const;
		
		public:
			~DocumentContainer();
			
			const Document& GetDocumentByIndex( std::size_t index ) const;
			const Document& GetDocumentByID   ( UInt32      id    ) const;
			
			void StoreNewElement( const boost::intrusive_ptr< Document >& document );
			
			size_t CountElements() const  { return itsMap.size(); }
			
			bool ExistsElementByIndex( std::size_t index ) const  { return index <= CountElements(); }
			bool ExistsElementByID   ( UInt32      id    ) const;
			
			nucleus::owned< Nitrogen::AEDesc_Token > GetElementByIndex( std::size_t index ) const;
			nucleus::owned< Nitrogen::AEDesc_Token > GetElementByID   ( UInt32      id    ) const;
			
			void DeleteElementByIndex( std::size_t index );
			
			void DeleteElementByID( UInt32 id );
	};
	
	
	class App : public Pedestal::Application
	{
		public:
			App();
			~App();
	};
	
}

#endif

