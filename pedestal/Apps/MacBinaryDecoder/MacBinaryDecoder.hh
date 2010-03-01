/*	===================
 *	MacBinaryDecoder.hh
 *	===================
 */

#ifndef MACBINARYDECODER_HH
#define MACBINARYDECODER_HH

// MacFiles
#include "MacFiles/Classic.hh"
#include "MacFiles/Unicode.hh"

// Pedestal
#include "Pedestal/Application.hh"


namespace MacBinaryDecoder
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
	
	
	class App : public Pedestal::Application
	{
		private:
			static App* theApp;
			
			nucleus::owned< Nitrogen::AEEventHandler > itsOpenDocsEventHandler;
		
		public:
			static App& Get();
			
			App();
			~App();
			
			void OpenDocument( const Io_Details::file_spec& file );
	};
	
}

#endif

