/*	================
 *	ThrowOSStatus.hh
 *	================
 */

#ifndef MACIO_THROWOSSTATUS_HH
#define MACIO_THROWOSSTATUS_HH

// Nitrogen
#include "Nitrogen/OSStatus.hh"


namespace MacIO
{
	
	struct Throw_All
	{
		typedef void Result;
		
		static void HandleOSStatus( ::OSStatus err )
		{
			Nitrogen::ThrowOSStatus( err );
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
			
			Nitrogen::ThrowOSStatus( err );
			
			return true;
		}
	};
	
	typedef Throw_Except< eofErr > Return_EOF;
	typedef Throw_Except< fnfErr > Return_FNF;
	
}

#endif

