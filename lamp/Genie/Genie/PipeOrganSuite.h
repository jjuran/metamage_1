/*	================
 *	PipeOrganSuite.h
 *	================
 *	
 *	No implementation necessary.
 */

#pragma once


namespace Genie {
	
	enum { kAEPipeOrganSuite = '|gan' };
	enum {
		kAEExec = 'Exec',
		keyCurrentWorkingDir = 'CWD ',
		keyRedirectedInput = '0<  ',
		keyInlineInput = '0<< ',
		keyRedirectedOutput = '1>  ',
		keyAppendedOutput = '1>> ',
		keyAppend = '>>  ',
		keyPOSIXEnvironment = 'Envi'
		
	};
	
}

