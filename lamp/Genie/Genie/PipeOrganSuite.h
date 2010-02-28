/*	================
 *	PipeOrganSuite.h
 *	================
 */

#ifndef GENIE_PIPEORGANSUITE_H
#define GENIE_PIPEORGANSUITE_H

#include "Nitrogen/AEDataModel.hh"


namespace Genie
{
	
	static const Nitrogen::AEEventClass kAEPipeOrganSuite = Nitrogen::AEEventClass( '|gan' );
	
	static const Nitrogen::AEEventID kAEExec = Nitrogen::AEEventID( 'Exec' );
	
	static const Nitrogen::AEKeyword keyCurrentWorkingDir = Nitrogen::AEKeyword( 'CWD ' );
	static const Nitrogen::AEKeyword keyRedirectedInput   = Nitrogen::AEKeyword( '0<  ' );
	static const Nitrogen::AEKeyword keyInlineInput       = Nitrogen::AEKeyword( '0<< ' );
	static const Nitrogen::AEKeyword keyRedirectedOutput  = Nitrogen::AEKeyword( '1>  ' );
	static const Nitrogen::AEKeyword keyAppendedOutput    = Nitrogen::AEKeyword( '1>> ' );
	static const Nitrogen::AEKeyword keyAppend            = Nitrogen::AEKeyword( '>>  ' );
	static const Nitrogen::AEKeyword keyPOSIXEnvironment  = Nitrogen::AEKeyword( 'Envi' );
	
}

#endif

