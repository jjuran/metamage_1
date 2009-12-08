/*	=========
 *	Faults.hh
 *	=========
 */

#ifndef GENIE_FAULTS_HH
#define GENIE_FAULTS_HH


namespace Genie
{
	
	void InstallExceptionHandlers();
	
	// Defined in Process.cc.
	void DeliverFatalSignal( int signo );
	
}

#endif

