/*	================
 *	PipeOrganSuite.h
 *	================
 *	
 *	No implementation necessary.
 */

#pragma once

#include "Nitrogen/AEDataModel.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	enum { kAEPipeOrganSuite = '|gan' };
	enum
	{
		kAEExec = 'Exec'
	};
	
	static const N::AEKeyword keyCurrentWorkingDir = N::AEKeyword( 'CWD ' );
	static const N::AEKeyword keyRedirectedInput   = N::AEKeyword( '0<  ' );
	static const N::AEKeyword keyInlineInput       = N::AEKeyword( '0<< ' );
	static const N::AEKeyword keyRedirectedOutput  = N::AEKeyword( '1>  ' );
	static const N::AEKeyword keyAppendedOutput    = N::AEKeyword( '1>> ' );
	static const N::AEKeyword keyAppend            = N::AEKeyword( '>>  ' );
	static const N::AEKeyword keyPOSIXEnvironment  = N::AEKeyword( 'Envi' );
	
}

