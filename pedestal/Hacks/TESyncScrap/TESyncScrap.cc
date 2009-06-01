/*
	
	TESyncScrap
	
	Joshua Juran
	
*/

// Mac OS
#include <Resources.h>
#include <Scrap.h>
#include <TextEdit.h>

// Silver
#include "Silver/Install.hh"
#include "Silver/Patch.hh"
#include "Silver/Procs.hh"
#include "Silver/Traps.hh"


#pragma exceptions off


namespace Ag = Silver;


namespace
{
	
	static OSErr FlushScrap()
	{
		OSErr err = ZeroScrap();
		
		if ( err == noErr )
		{
			err = TEToScrap();
		}
		
		return err;
	}
	
	void TECut_Patch( TEHandle hTE, Ag::TECutProcPtr nextHandler )
	{
		nextHandler( hTE );
		
		FlushScrap();
	}
	
	#define TECopy_Patch TECut_Patch
	/*
	// TECopy_Patch is the same as TECut_Patch
	
	void TECopy_Patch( TEHandle hTE, Ag::TECopyProcPtr nextHandler )
	{
		nextHandler( hTE );
		
		FlushScrap();
	}
	*/
	
	void TEPaste_Patch( TEHandle hTE, Ag::TEPasteProcPtr nextHandler )
	{
		OSErr err = TEFromScrap();
		
		nextHandler( hTE );
	}
	
	
	void TEInit_Patch( Ag::TEInitProcPtr nextHandler )
	{
		const Handle hack = ::Get1NamedResource( 'Hack', "\p" "TESyncScrap" );
		
		if ( hack != NULL )
		{
			::ReleaseResource( hack );
		}
		else
		{
			Ag::TrapPatch< _TECopy,  TECopy_Patch  >::Install();
			Ag::TrapPatch< _TECut,   TECut_Patch   >::Install();
			Ag::TrapPatch< _TEPaste, TEPaste_Patch >::Install();
		}
		
		nextHandler();
	}
	
}


static OSErr Installer()
{
	Ag::TrapPatch< _TEInit, TEInit_Patch >::Install();
	
	return noErr;
}

int main()
{
	return Ag::Install( Installer );
}

