/*
	file_save_dialog.hh
	-------------------
*/

#include "mac_app/file_save_dialog.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __APPLE__
#ifndef __NAVIGATION__
#include <Navigation.h>
#endif
#ifndef __STANDARDFILE__
#include <StandardFile.h>
#endif
#endif


namespace mac {
namespace app {

#if ! TARGET_API_MAC_CARBON

long file_save_dialog( StrArg prompt, StrArg name, HFS_callback proc )
{
	Point where = { 82, 82 };  // FIXME
	
	SFReply reply;
	
	SFPutFile( where, prompt, name, NULL, &reply );
	
	if ( ! reply.good )
	{
		return userCanceledErr;
	}
	
	return proc( reply.vRefNum, 0, reply.fName );
}

long file_save_dialog( StrArg prompt, StrArg name, FSSpec_callback proc )
{
	StandardFileReply reply;
	
	StandardPutFile( prompt, name, &reply );
	
	if ( ! reply.sfGood )
	{
		return userCanceledErr;
	}
	
	return proc( reply.sfFile );
}

#endif

long file_save_dialog( OSType type, OSType creator, FSRefName_callback proc )
{
	OSStatus err = 1;
	
#if ! __LP64__
	
	NavDialogRef nav;
	NavDialogCreationOptions options;
	
	err = NavGetDefaultDialogCreationOptions( &options );
	
	err = NavCreatePutFileDialog( &options, type, creator, NULL, 0, &nav );
	
	if ( err == noErr )
	{
		err = NavDialogRun( nav );
		
		if ( err == noErr )
		{
			NavReplyRecord reply;
			
			NavUserAction action = NavDialogGetUserAction( nav );
			
			switch ( action )
			{
				case kNavUserActionCancel:
				case kNavUserActionNone:
					err = userCanceledErr;
					break;
				
				default:
					reply.version = kNavReplyRecordVersion;
					
					err = NavDialogGetReply( nav, &reply );
					
					if ( err == noErr )
					{
						if ( reply.validRecord )
						{
							AEKeyword key;
							DescType type;
							FSRef parent;
							Size size;
							
							err = AEGetNthPtr( &reply.selection,
							                   1,
							                   typeFSRef,
							                   &key,
							                   &type,
							                   &parent,
							                   sizeof parent,
							                   &size );
							
							if ( err == noErr )
							{
								err = proc( parent, reply.saveFileName );
								
								if ( err == noErr )
								{
									err = NavCompleteSave( &reply, 0 );
								}
							}
						}
						
						NavDisposeReply( &reply );
					}
			}
		}
		
		NavDialogDispose( nav );
	}
	
#endif
	
	return err;
}

}
}
