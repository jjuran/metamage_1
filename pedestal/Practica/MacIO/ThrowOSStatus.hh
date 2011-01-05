/*	================
 *	ThrowOSStatus.hh
 *	================
 */

#ifndef MACIO_THROWOSSTATUS_HH
#define MACIO_THROWOSSTATUS_HH

// Nitrogen
#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
#endif


namespace MacIO
{
	
	struct Throw_All
	{
		typedef void Result;
		
		static void HandleOSStatus( ::OSStatus err )
		{
			Mac::ThrowOSStatus( err );
		}
	};
	
	template < ::OSStatus exception >
	struct Throw_Except
	{
		typedef bool Result;
		
		static bool HandleOSStatus( ::OSStatus err )
		{
			if ( err == exception )
			{
				return false;
			}
			
			Mac::ThrowOSStatus( err );
			
			return true;
		}
	};
	
	typedef Throw_Except< eofErr > Return_EOF;
	typedef Throw_Except< fnfErr > Return_FNF;
	
}

#endif

