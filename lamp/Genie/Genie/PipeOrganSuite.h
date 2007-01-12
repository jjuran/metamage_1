/*	================
 *	PipeOrganSuite.h
 *	================
 */

#ifndef GENIE_PIPEORGANSUITE_H
#define GENIE_PIPEORGANSUITE_H

#include "Nitrogen/AEDataModel.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	static const N::AEEventClass kAEPipeOrganSuite = N::AEEventClass( '|gan' );
	
	static const N::AEEventID kAEExec = N::AEEventID( 'Exec' );
	
	static const N::AEKeyword keyCurrentWorkingDir = N::AEKeyword( 'CWD ' );
	static const N::AEKeyword keyRedirectedInput   = N::AEKeyword( '0<  ' );
	static const N::AEKeyword keyInlineInput       = N::AEKeyword( '0<< ' );
	static const N::AEKeyword keyRedirectedOutput  = N::AEKeyword( '1>  ' );
	static const N::AEKeyword keyAppendedOutput    = N::AEKeyword( '1>> ' );
	static const N::AEKeyword keyAppend            = N::AEKeyword( '>>  ' );
	static const N::AEKeyword keyPOSIXEnvironment  = N::AEKeyword( 'Envi' );
	
}

#endif

