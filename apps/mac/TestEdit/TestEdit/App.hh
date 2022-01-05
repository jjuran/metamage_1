/*
	App.hh
	------
*/

#ifndef TESTEDIT_APP_HH
#define TESTEDIT_APP_HH

// Nitrogen
#ifndef NITROGEN_APPLEEVENTS_HH
#include "Nitrogen/AppleEvents.hh"
#endif

// MacFiles
#include "MacFiles/Classic.hh"
#include "MacFiles/Unicode.hh"

// Pedestal
#ifndef PEDESTAL_WINDOW_HH
#include "Pedestal/Window.hh"
#endif


namespace TestEdit
{
	
	struct FSSpec_Io_Details : public Nitrogen::FSSpec_Io_Details
	{
		static const Mac::DescType typeFileSpec = Mac::typeFSS;
	};
	
	struct FSRef_Io_Details : public Nitrogen::FSRef_Io_Details
	{
		static const Mac::DescType typeFileSpec = Mac::typeFSRef;
	};
	
#if TARGET_API_MAC_CARBON
	
	typedef FSRef_Io_Details Io_Details;
	
#else
	
	typedef FSSpec_Io_Details Io_Details;
	
#endif
	
	
	class DocumentContainer
	{
		public:
			nucleus::owned< Mac::AEDesc_Token > GetElementByIndex( std::size_t index ) const;
			nucleus::owned< Mac::AEDesc_Token > GetElementByID   ( UInt32      id    ) const;
	};
	
}

#endif
