// QuickDraw.cp

#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/QuickDraw.h"
#endif

#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif

#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif

#ifndef NITROGEN_OSSTATUS_H
#include "Nitrogen/OSStatus.h"
#endif

namespace Nitrogen
  {
   Point GetPortPenSize( CGrafPtr port )
     {
      Point result;
      ::GetPortPenSize( port, &result );
      return result;
     }

	Owned<RgnHandle> NewRgn(void) {
		OnlyOnce<RegisterQuickDrawErrors> ();
		RgnHandle result = ::NewRgn ();
		if ( result == NULL )
			throw MemFullErr ();
		return Owned<RgnHandle>::Seize ( result );
		}
	
	Owned<RgnHandle> CopyRgn ( RgnHandle srcRgn ) {
		Owned<RgnHandle> result = Owned<RgnHandle>::Seize ( ::NewRgn ());
		::CopyRgn ( srcRgn, result );
		return result;
		}
  
	void RegisterQuickDrawErrors () {
		RegisterOSStatus< memFullErr >();
		}
  
  }