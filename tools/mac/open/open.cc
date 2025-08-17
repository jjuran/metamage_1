/*
	open.cc
	-------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// command
#include "command/get_option.hh"

// gear
#include "gear/quad.hh"

// mac-sys-utils
#include "mac_sys/current_process.hh"
#include "mac_sys/errno_from_mac_error.hh"
#include "mac_sys/is_front_process.hh"

// mac-file-utils
#include "mac_file/desktop.hh"

// mac-proc-utils
#include "mac_proc/find_process.hh"
#include "mac_proc/launch_application.hh"

// mac-relix-utils
#include "mac_relix/FSRef_from_path.hh"
#include "mac_relix/FSSpec_from_path.hh"

// Nitrogen
#include "Nitrogen/AEDataModel.hh"
#include "Nitrogen/AEInteraction.hh"
#include "Nitrogen/Str.hh"

// Orion
#include "Orion/Main.hh"


using namespace command::constants;

#if __LP64__

typedef FSRef FSObj;

enum
{
	typeFSObj = typeFSRef,
};

#else

typedef FSSpec FSObj;

enum
{
	typeFSObj = typeFSS,
};

#endif

enum
{
	Option_app_name      = 'a',
	Option_system_editor = 'e',  // TextEdit on OS X
	Option_user_editor   = 't',  // via LaunchServices on OS X
	
	Option_last_byte = 255,
	
	Option_app_sig,
	
	Option_activate,
	Option_HFS_paths,
};

static command::option options[] =
{
	{ "app", Option_app_name, Param_required },
	{ "sig", Option_app_sig, Param_required },
	
	{ "edit",    Option_user_editor   },
	{ "sysedit", Option_system_editor },
	
	{ "activate", Option_activate  },
	{ "actv",     Option_activate  },
	{ "hfs",      Option_HFS_paths },
	{ "mac",      Option_HFS_paths },
	
	{ NULL }
};

static const char* gAppNameToOpenIn = NULL;
static const char* gAppSigToOpenIn  = NULL;

static bool gOpenInEditor    = false;
static bool gActivate        = false;
static bool gUseHFSPathnames = false;

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_app_name:
				gAppNameToOpenIn = command::global_result.param;
				break;
			
			case Option_app_sig:
				gAppSigToOpenIn = command::global_result.param;
				break;
			
			case Option_user_editor:
			case Option_system_editor:
				gOpenInEditor = true;
				break;
			
			case Option_activate:
				gActivate = true;
				break;
			
			case Option_HFS_paths:
				gUseHFSPathnames = true;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	using mac::sys::Error;
	using mac::sys::errno_from_mac_error;
	
	
	enum
	{
		sigFinder = 'MACS'
	};
	
	
#if ! __LP64__
	
	static
	Error ResolvePathname( FSSpec& file, const char* path, bool hfs )
	{
		using mac::relix::FSSpec_from_existing_path;
		
		Error err = hfs ? (Error) FSMakeFSSpec( 0, 0, N::Str255( path ), &file )
		                : FSSpec_from_existing_path( path, file );
		
		return err;
	}
	
#endif
	
	static inline
	Error ResolvePathname( FSRef& file, const char* path, bool hfs )
	{
		using mac::relix::FSRef_from_path;
		
		return FSRef_from_path( path, file );
	}
	
	static
	n::owned< Mac::AEDesc_Data > CoerceFSObjToAliasDesc( const FSObj& item )
	{
		OSErr err;
		
		Mac::AEDesc_Data result;
		
		err = AECoercePtr( typeFSObj, &item, sizeof item, typeAlias, &result );
		
		if ( ! TARGET_API_MAC_CARBON  &&  err == errAECoercionFail )
		{
			/*
				Older systems don't provide FSSpec->alias
				coercion, so do it manually.
			*/
			
			AliasHandle alias;
			
		#if ! __LP64__
			
			err = NewAlias( NULL, &item, &alias );
			
		#endif
			
			if ( err == noErr )
			{
				result.descriptorType = typeAlias;
				result.dataHandle     = (AEDataStorage) alias;
			}
		}
		
		Mac::ThrowOSStatus( err );
		
		return n::owned< Mac::AEDesc_Data >::seize( result );
	}
	
	static n::owned< Mac::AppleEvent > MakeOpenDocsEvent( const Mac::AEDescList_Data&  items,
	                                                      const ProcessSerialNumber&   psn )
	{
		OSErr           err;
		AEAddressDesc   addr;
		Mac::AppleEvent event;
		
		err = AECreateDesc( typeProcessSerialNumber, &psn, sizeof psn, &addr );
		
		if ( err == noErr )
		{
			err = AECreateAppleEvent( kCoreEventClass,
			                          kAEOpenDocuments,
			                          &addr,
			                          kAutoGenerateReturnID,
			                          kAnyTransactionID,
			                          &event );
			
			AEDisposeDesc( &addr );
			
			if ( err == noErr )
			{
				err = AEPutParamDesc( &event,
				                      keyDirectObject,
				                      &items );
				
				if ( err )
				{
					AEDisposeDesc( &event );
				}
			}
		}
		
		Mac::ThrowOSStatus( err );
		
		return n::owned< Mac::AppleEvent >::seize( event );
	}
	
	static void OpenItemsWithRunningApp( const Mac::AEDescList_Data&  items,
	                                     const ProcessSerialNumber&   psn )
	{
		if ( gActivate )
		{
			SetFrontProcess( &psn );
		}
		
		N::AESend( MakeOpenDocsEvent( items, psn ),
		           Mac::kAENoReply | Mac::kAECanInteract );
		
		if ( gActivate )
		{
			using mac::sys::current_process;
			using mac::sys::is_front_process;
			
			while ( ! is_front_process( current_process() ) )
			{
				sleep( 0 );
			}
		}
	}
	
	static
	void LaunchApplicationWithDocsToOpen( const FSObj&                 app,
	                                      const Mac::AEDescList_Data&  items )
	{
		static ProcessSerialNumber no_process = {};
		
		OSErr  err;
		AEDesc params;
		
		n::owned< Mac::AppleEvent > event = MakeOpenDocsEvent( items, no_process );
		
		err = AECoerceDesc( &event.get(), typeAppParameters, &params );
		
		if ( err == noErr )
		{
			Handle h;
			
			if ( TARGET_API_MAC_CARBON )
			{
				Size size = AEGetDescDataSize( &params );
				
				if ( (h = NewHandle( size )) )
				{
					AEGetDescData( &params, *h, size );
				}
				
				AEDisposeDesc( &params );
			}
			else
			{
				h = (Handle) params.dataHandle;
			}
			
			if ( h )
			{
				HLock( h );
				
				AppParameters* params = (AppParameters*) *h;
				
				mac::proc::launch_application( app, params );
				
				DisposeHandle( h );
			}
		}
	}
	
	
	static
	OSType DefaultTextFileCreator()
	{
	#ifdef __RELIX__
		
		const char* path = "/sys/type/text/.~DEFAULT";
		
		int fd = open( path, O_RDONLY );
		
		if ( fd >= 0 )
		{
			OSType result;
			
			ssize_t n_read = read( fd, &result, sizeof result );
			
			close( fd );
			
			if ( n_read == sizeof result )
			{
				return result;
			}
		}
		
		/*
			If we get here, either the file /sys/type/text/.~DEFAULT
			got removed from MacRelix without updating open, or we're so
			resource-constrained that the open() failed, in which case
			reporting the error will be similarly dicey.
			
			Either way, fall back to 'ttxt'.
		*/
		
	#endif
		
		return 'ttxt';
	}
	
	static
	OSType SignatureOfAppForOpening()
	{
		if ( gOpenInEditor )
		{
			// User has specified default text editor.
			// Check MAC_EDITOR_SIGNATURE environment variable first.
			// If set, it must be a four-character code.
			
			if ( const char* macEditorSignature = getenv( "MAC_EDITOR_SIGNATURE" ) )
			{
				if ( strlen( macEditorSignature ) == sizeof 'quad' )
				{
					return gear::decode_quad( macEditorSignature );
				}
				
				// Treat a malformed quad value the same as no value.  Move on.
			}
			
			// No MAC_EDITOR_SIGNATURE; use the MacRelix default.
			
			return DefaultTextFileCreator();
		}
		
		if ( gAppSigToOpenIn != NULL )
		{
			// User has specified an application by its signature
			
			if ( strlen( gAppSigToOpenIn ) != sizeof 'quad' )
			{
				Mac::ThrowOSStatus( paramErr );
			}
			
			return gear::decode_quad( gAppSigToOpenIn );
		}
		
		// Otherwise, give everything to the Finder.
		
		return sigFinder;
	}
	
	static void OpenItemsUsingOptions( const Mac::AEDescList_Data& items )
	{
		// we either have a pathname or signature for the app.
		// if pathname, resolve to FSSpec and check if it's running.
		// if running, send AE and return
		// if signature, check if it's running.
		// if it's running, send it odoc and return
		// if not, lookup in DT
		// launch with app parameters
		
		// ways to specify app:
		// --app: pathname
		// --sig: signature
		// -e or -t: either?
		// default: sig
		
		FSObj appFile;
		
		if ( gAppNameToOpenIn != NULL )
		{
			// User has specified an application by its pathname
			
			using mac::relix::FSObj_from_existing_path;
			
			// Resolve to FSSpec
			OSErr err = FSObj_from_existing_path( gAppNameToOpenIn, appFile );
			
			Mac::ThrowOSStatus( err );
			
			// Find it if running
			ProcessSerialNumber psn;
			
			if ( mac::proc::find_process( psn, appFile ) == noErr )
			{
				// The app is already running -- send it an odoc event
				OpenItemsWithRunningApp( items, psn );
				
				// We're done
				return;
			}
			
			// No such process
			// appFile is already set
		}
		else
		{
			// Look up by signature.
			
			// Pick a signature
			OSType signature = SignatureOfAppForOpening();
			
			// Find it if running
			ProcessSerialNumber psn;
			
			OSErr err = mac::proc::find_process( psn, signature );
			
			if ( err == noErr )
			{
				// The app is already running -- send it an odoc event
				OpenItemsWithRunningApp( items, psn );
				
				// We're done
				return;
			}
			
			if ( err == procNotFound )
			{
			#if ! __LP64__
				
				err = mac::file::get_desktop_APPL( appFile, signature );
				
			#endif
			}
			
			Mac::ThrowOSStatus( err );
		}
		
		LaunchApplicationWithDocsToOpen( appFile, items );
	}
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
	#if __LP64__
		
		if ( gUseHFSPathnames )
		{
			fprintf( stderr, "open: HFS pathnames are unsupported in 64-bit\n" );
			
			return 50;
		}
		
	#endif
		
		n::owned< Mac::AEDescList_Data > items = N::AECreateList< Mac::AEDescList_Data >( false );
		
		for ( char const *const *it = args;  *it != NULL;  ++it )
		{
			const char* pathname = *it;
			
			FSObj item;
			
			Error err = ResolvePathname( item, pathname, gUseHFSPathnames );
			
			if ( err )
			{
				int errnum = is_errno( err ) ? errno_from_muxed( err )
				                             : errno_from_mac_error( err );
				
				if ( errnum > 0 )
				{
					fprintf( stderr, "open: %s: %s\n", pathname, strerror( errnum ) );
				}
				else
				{
					fprintf( stderr, "open: %s: OSErr %d\n", pathname, err );
				}
				
				continue;
			}
			
			N::AEPutDesc( items, 0, CoerceFSObjToAliasDesc( item ) );
		}
		
		OpenItemsUsingOptions( items );
		
		return 0;
	}

}
