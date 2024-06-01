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

// mac-config
#include "mac_config/upp-macros.hh"


namespace mac {
namespace app {

#if ! TARGET_API_MAC_CARBON

static short selection_end;

static
pascal
Boolean filter_proc( DialogRef dialog, EventRecord* event, short* itemHit )
{
	if ( selection_end > 0  &&  event->what == updateEvt )
	{
		SelectDialogItemText( dialog, putName, 0, selection_end );
		
		selection_end = 0;
	}
	
	return false;
}

long file_save_dialog( StrArg prompt, StrArg name, HFS_Proc proc, int ext_len )
{
	DEFINE_UPP( ModalFilter, filter_proc )
	
	selection_end = name[ 0 ] - ext_len;
	
	Point where = { 82, 82 };  // FIXME
	
	const short id = putDlgID;
	
	SFReply reply;
	
	SFPPutFile( where, prompt, name, NULL, &reply, id, UPP_ARG( filter_proc ) );
	
	if ( ! reply.good )
	{
		return userCanceledErr;
	}
	
	return proc( reply.vRefNum, 0, reply.fName );
}

long file_save_dialog( StrArg prompt, StrArg name, FSS_Proc proc, int ext_len )
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
