/*	===================
 *	GenieExecHandler.cc
 *	===================
 */

#include "Genie/ExecHandler.hh"

// Nitrogen
#include "Nitrogen/AEDataModel.h"
#include "Nitrogen/OSStatus.h"

// Pedestal
#include "Pedestal/ApplicationContext.hh"
#include "Pedestal/Application.hh"

// Genie
#include "Genie/PipeOrganSuite.h"


// FIXME:  Replace this with a userspace interface, in the manner of Berkeley sockets.

namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	void GenieExecHandler::AppleEventHandler( const AppleEvent& appleEvent, AppleEvent& reply, GenieExecHandler* handler )
	{
		handler->HandleAppleEvent( appleEvent, reply );
	}
	
	GenieExecHandler::GenieExecHandler()
	:
		N::Owned< N::AEEventHandler >
		(
			N::AEInstallEventHandler< GenieExecHandler*, AppleEventHandler >( kAEPipeOrganSuite,
			                                                                  kAEExec,
			                                                                  this )
		)
	{
		//MakeNoteToTouchParam(keyDirectObject);
		//MakeNoteToTouchParam(keyCurrentWorkingDir);
		//MakeNoteToTouchParam(keyRedirectedInput);
		//MakeNoteToTouchParam(keyInlineInput);
		//MakeNoteToTouchParam(keyRedirectedOutput);
		//MakeNoteToTouchParam(keyAppendedOutput);
		//MakeNoteToTouchParam(keyAppend);
	}
	
	static N::FSDirSpec GetCWD(const AEDesc& appleEvent)
	{
		try
		{
			return N::Convert< N::FSDirSpec >( N::AEGetParamPtr< typeFSS >( appleEvent,
			                                                                keyCurrentWorkingDir ) );
		}
		catch ( N::ErrAEDescNotFound )
		{
			return N::FSDirSpec();
		}
	}
	
	void GenieExecHandler::HandleAppleEvent( const AEDesc& appleEvent, AEDesc& outReply )
	{
		int returnValue = -1;
		
		N::FSDirSpec cwd = GetCWD( appleEvent );
		
		// Currently, we're expecting a string or a list of strings.
		
		if ( N::AESizeOfParam( appleEvent, keyDirectObject ).typeCode != typeAEList )
		{
			std::string str = N::AEGetParamPtr< typeChar >( appleEvent, keyDirectObject );
			
			returnValue = ExecString(str, cwd);
		} 
		else
		{
			throw N::ErrAEEventNotHandled();
			
			N::Owned< N::AEDescList > list = N::AEGetParamDesc( appleEvent, keyDirectObject, typeWildCard );
			
			std::vector< std::string > argVec;
			
			long argCount = N::AECountItems( list );
			
			// Load the arg vector
			for ( int index = 1;  index <= argCount;  index++ )
			{
				argVec.push_back( N::AEGetNthPtr< typeChar >( list, index ) );
			}
			
			// Attach our flow director to the output console to redirect output.
			
			returnValue = ExecArgList(argVec);
		}
		// Put the return value into the reply.
		//N::AEPutParamPtr<typeSInt32>( outReply, keyDirectObject, returnValue );
	}
	
	int GenieExecHandler::ExecArgList( const std::vector< std::string >& argVec )
	{
		int result = 0;
		
	#if 0
		// Echo the input line to the console.  Use stderr, not stdout.
		if ( argCount > 0 )
		{
			fprintf( stderr, "$ " );
			fprintf( stderr, "%s", static_cast< const char * >( inArgVec[ 0 ] ) );
			
			for ( index = 1;  index < argCount;  ++index )
			{
				fprintf( stderr, " %s", static_cast< const char * >( inArgVec[ index ] ) );
			}
			
			fprintf( stderr, "\n" );
		}
	#endif
		
		//int pid = gProcessTable[GenieProcess::kRootProcessPID].ForkExec(inArgVec);
		
		// FIXME:  We're assuming an immediate result
		//result = gProcessTable[pid].Result();
		
		return result;
	}
	
	int GenieExecHandler::ExecString( const std::string& cmd, const N::FSDirSpec& cwd )
	{
		/*
		int pid = ( new GenieShell( GenieProcess::kRootProcessPID, cmd ) )->ProcessID();
		gProcessTable[ pid ].ChangeDirectory( cwd );
		
		// FIXME:  We're assuming an immediate result
		int result = gProcessTable[ pid ].Result();
		
		return result;
		*/
		
		throw N::ErrAEEventNotHandled();
	}
	
}

