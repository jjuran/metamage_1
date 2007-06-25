/*	===================
 *	MacBinaryDecoder.hh
 *	===================
 */

#ifndef MACBINARYDECODER_HH
#define MACBINARYDECODER_HH

// Nucleus
#include "Nucleus/Owned.h"

// Pedestal
#include "Pedestal/Application.hh"
#include "Pedestal/AboutBox.hh"


namespace MacBinaryDecoder
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
	
	
	class App : public Ped::Application,
	            public Ped::AboutBoxOwner
	{
		private:
			static App* theApp;
			
			Ped::AboutHandler< App > itsAboutHandler;
			NN::Owned< N::AEEventHandler > itsOpenDocsEventHandler;
		
		public:
			static App& Get();
			
			App();
			~App();
			
			void OpenDocument( const Io_Details::file_spec& file );
	};
	
}

#endif

