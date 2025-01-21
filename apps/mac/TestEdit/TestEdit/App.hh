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


namespace TestEdit
{
	
	class DocumentContainer
	{
		public:
			nucleus::owned< Mac::AEDesc_Token > GetElementByIndex( std::size_t index ) const;
			nucleus::owned< Mac::AEDesc_Token > GetElementByID   ( UInt32      id    ) const;
	};
	
}

#endif
