/*
	file_open_dialog.hh
	-------------------
*/

#include "mac_app/file_open_dialog.hh"

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

static
OSStatus NavServices_get_file( const OSType* types, int n, AEDesc& result )
{
	OSStatus err = 1;
	
#if ! __LP64__
	
	NavDialogRef nav;
	
	NavTypeListHandle typesH;
	
	const Size size = sizeof (NavTypeList) + (n - 1) * sizeof (OSType);
	
	typesH = (NavTypeListHandle) NewHandleClear( size );
	
	typesH[0]->osTypeCount = n;
	
	BlockMoveData( types, typesH[0]->osType, n * sizeof (OSType) );
	
	NavDialogCreationOptions options;
	
	err = NavGetDefaultDialogCreationOptions( &options );
	
	err = NavCreateGetFileDialog( &options, typesH, NULL, NULL, NULL, 0, &nav );
	
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
							err = AEDuplicateDesc( &reply.selection, &result );
						}
						
						NavDisposeReply( &reply );
					}
			}
		}
		
		NavDialogDispose( nav );
	}
	
	DisposeHandle( (Handle) typesH );
	
#endif
	
	return err;
}

template < ::DescType desiredType, class FileSpec >
static
OSStatus open_AEDescList( const AEDesc& list, long (*proc)(const FileSpec&) )
{
	OSErr err;
	
	long count;
	err = AECountItems( &list, &count );
	
	if ( err == noErr )
	{
		for ( ;  count > 0;  --count )
		{
			AEKeyword key;
			DescType type;
			FileSpec file;
			Size size;
			
			err = AEGetNthPtr( &list,
			                   count,
			                   desiredType,
			                   &key,
			                   &type,
			                   &file,
			                   sizeof file,
			                   &size );
			
			if ( err )
			{
				return err;
			}
			
			if ( OSStatus err = proc( file ) )
			{
				return err;
			}
		}
	}
	
	return err;
}

long file_open_dialog( const OSType* types, int n, HFS_callback proc )
{
#if ! TARGET_API_MAC_CARBON
	
	Point where = { 82, 82 };  // FIXME
	
	SFReply reply;
	
	SFGetFile( where, NULL, NULL, n, types, NULL, &reply );
	
	if ( ! reply.good )
	{
		return userCanceledErr;
	}
	
	return proc( reply.vRefNum, 0, reply.fName );
	
#endif
	
	return 0;
}

long file_open_dialog( const OSType* types, int n, FSSpec_callback proc )
{
#if ! TARGET_API_MAC_CARBON
	
	StandardFileReply reply;
	
	StandardGetFile( NULL, n, types, &reply );
	
	if ( ! reply.sfGood )
	{
		return userCanceledErr;
	}
	
	return proc( reply.sfFile );
	
#endif
	
	AEDesc list;
	
	OSStatus err = NavServices_get_file( types, n, list );
	
	if ( err )
	{
		return err;
	}
	
#if ! __LP64__
	
	err = open_AEDescList< typeFSS >( list, proc );
	
#endif
	
	AEDisposeDesc( &list );
	
	return err;
}

long file_open_dialog( const OSType* types, int n, FSRef_callback proc )
{
	AEDesc list;
	
	OSStatus err = NavServices_get_file( types, n, list );
	
	if ( err )
	{
		return err;
	}
	
	err = open_AEDescList< typeFSRef >( list, proc );
	
	AEDisposeDesc( &list );
	
	return err;
}

}
}
