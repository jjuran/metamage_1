/*
	Interax.cc
	----------
*/

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Standard C
#include <fcntl.h>

// Annex
#ifndef ANNEX_MACTYPES_H
#include "Annex/MacTypes.h"
#endif

// mac-config
#include "mac_config/upp-macros.hh"

// rasterlib
#include "raster/load.hh"

// plus
#include "plus/string.hh"

// amicus
#include "amicus/events.hh"
#include "amicus/raster_task.hh"

// aglow
#include "aglow/application.hh"


using raster::raster_desc;
using raster::raster_load;


static raster_load loaded_raster;

static
OSStatus chdir_FSRef( const FSRef& ref )
{
	OSStatus err;
	
	plus::string path;
	
	UInt32 maxPathSize = 64;
	
	do
	{
		maxPathSize *= 2;
		
		char* p = path.reset( maxPathSize );
		
		err = FSRefMakePath( &ref, (UInt8*) p, maxPathSize );
	}
	while ( err == pathTooLongErr  ||  err == buffersTooSmall );
	
	if ( err )
	{
		return err;
	}
	
	if ( int nok = chdir( path.c_str() ) )
	{
		return ioErr;  // FIXME
	}
	
	return noErr;
}

static bool running = false;

static
pascal OSErr handle_odoc_Apple_event( AppleEvent const* event,
                                      AppleEvent*       reply,
                                      SRefCon           refcon )
{
	OSErr err;
	
	if ( running )
	{
		return errAEEventNotHandled;  // fBusyErr?
	}
	
	FSRef opened_doc_file;
	
	(err = AEGetParamPtr( event,
	                     keyDirectObject,
	                     typeFSRef,
	                     NULL,
	                     &opened_doc_file,
	                     sizeof opened_doc_file,
	                     NULL ))
	||
	(err = chdir_FSRef( opened_doc_file ));
	
	if ( err )  return err;
	
	const char raster_path[] = "screen.skif";
	
	amicus::events_fd = open( "events.fifo", O_WRONLY | O_NONBLOCK );
	
	int raster_fd = open( raster_path, O_RDWR );
	
	loaded_raster = raster::open_raster( raster_fd, true );
	
	running = true;
	
	QuitApplicationEventLoop();
	
	return noErr;
}

DEFINE_UPP( AEEventHandler, handle_odoc_Apple_event )

int main( int argc, char** argv )
{
	amicus::initialize();
	
	OSStatus err;
	
	err = AEInstallEventHandler( kCoreEventClass,
	                             kAEOpenDocuments,
	                             UPP_ARG( handle_odoc_Apple_event ),
	                             0,
	                             false );
	
	RunApplicationEventLoop();
	
	using namespace amicus;
	
	if ( running )
	{
		raster_monitor monitored_raster( loaded_raster );
		
		const raster_load& load = loaded_raster;
		const raster_desc& desc = loaded_raster.meta->desc;
		
		run_event_loop( load, desc );
		
		open( UPDATE_FIFO, O_RDONLY | O_NONBLOCK );
	}
	
	return 0;
}
