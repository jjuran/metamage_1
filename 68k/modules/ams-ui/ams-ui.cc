/*
	ams-ui.cc
	---------
*/

// Mac OS
#ifndef __TRAPS__
#include <Traps.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// POSIX
#include <unistd.h>

// command
#include "command/get_option.hh"

// ams-common
#include "callouts.hh"

// ams-ui
#include "BinaryDecimal.hh"
#include "Controls.hh"
#include "Dialogs.hh"
#include "Drag.hh"
#include "Icons.hh"
#include "IntlUtils.hh"
#include "Lists.hh"
#include "Menus.hh"
#include "options.hh"
#include "StandardFile.hh"
#include "StrUtils.hh"
#include "TextEdit.hh"
#include "Windows.hh"


#pragma exceptions off


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-ui"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


enum
{
	Opt_GNEtix = 'G',  // GetNextEvent throttle, in ticks
	
	Opt_last_byte = 255,
	
	Opt_debug_updates,
	Opt_live_ModalFilter,
};

static command::option options[] =
{
	{ "debug-updates",     Opt_debug_updates    },
	{ "live-modal",        Opt_live_ModalFilter },
	{ "live-modal-filter", Opt_live_ModalFilter },
	
	{ "gne-ticks",  Opt_GNEtix, command::Param_required },
	
	NULL,
};


BitMap IconBitmap : 0x0A0E;

void* toolbox_trap_table[] : 3 * 1024;

#define TBTRAP( Proc )  (toolbox_trap_table[ _##Proc & 0x03FF ] = &Proc##_patch)

enum
{
	_CouldDialog    = 0xA979,
	_FreeDialog     = 0xA97A,
	_CouldAlert     = 0xA989,
	_FreeAlert      = 0xA98A,
	
	_BeginUpdate    = _BeginUpDate,
	_CheckUpdate    = _CheckUpDate,
	_EndUpdate      = _EndUpDate,
	_GetItemMark    = _GetItmMark,
	_GetMenu        = _GetRMenu,
	_SetItemMark    = _SetItmMark,
	_SetItemStyle   = _SetItmStyle,
	_UpdateControls = _UpdtControl,
	_UpdateDialog   = _UpdtDialog,
};


static
void install_Windows()
{
	TBTRAP( ZoomWindow    );  // A83A
	
	TBTRAP( DrawGrowIcon  );  // A904
	TBTRAP( DragGrayRgn   );  // A905
	TBTRAP( NewString     );  // A906
	TBTRAP( SetString     );  // A907
	TBTRAP( ShowHide      );  // A908
	TBTRAP( CalcVis       );  // A909
	TBTRAP( CalcVBehind   );  // A90A
	TBTRAP( ClipAbove     );  // A90B
	TBTRAP( PaintOne      );  // A90C
	TBTRAP( PaintBehind   );  // A90D
	
	TBTRAP( GetWMgrPort   );  // A910
	TBTRAP( CheckUpdate   );  // A911
	TBTRAP( InitWindows   );  // A912
	TBTRAP( NewWindow     );  // A913
	TBTRAP( DisposeWindow );  // A914
	TBTRAP( ShowWindow    );  // A915
	TBTRAP( HideWindow    );  // A916
	TBTRAP( GetWRefCon    );  // A917
	TBTRAP( SetWRefCon    );  // A918
	TBTRAP( GetWTitle     );  // A919
	TBTRAP( SetWTitle     );  // A91A
	TBTRAP( MoveWindow    );  // A91B
	TBTRAP( HiliteWindow  );  // A91C
	
	TBTRAP( SizeWindow    );  // A91D
	TBTRAP( TrackGoAway   );  // A91E
	TBTRAP( SelectWindow  );  // A91F
	TBTRAP( BringToFront  );  // A920
	TBTRAP( SendBehind    );  // A921
	TBTRAP( BeginUpdate   );  // A922
	TBTRAP( EndUpdate     );  // A923
	TBTRAP( FrontWindow   );  // A924
	TBTRAP( DragWindow    );  // A925
	TBTRAP( DragTheRgn    );  // A926
	TBTRAP( InvalRgn      );  // A927
	TBTRAP( InvalRect     );  // A928
	TBTRAP( ValidRgn      );  // A929
	TBTRAP( ValidRect     );  // A92A
	
	TBTRAP( GrowWindow    );  // A92B
	TBTRAP( FindWindow    );  // A92C
	TBTRAP( CloseWindow   );  // A92D
	
	TBTRAP( GetNewWindow  );  // A9BD
}

static
void install_Menus()
{
	TBTRAP( InsMenuItem  );  // A826
	
	TBTRAP( GetItemCmd   );  // A84E
	TBTRAP( SetItemCmd   );  // A84F
	
	TBTRAP( SetWindowPic );  // A92E
	TBTRAP( GetWindowPic );  // A92F
	TBTRAP( InitMenus    );  // A930
	TBTRAP( NewMenu      );  // A931
	TBTRAP( DisposeMenu  );  // A932
	TBTRAP( AppendMenu   );  // A933
	TBTRAP( ClearMenuBar );  // A934
	TBTRAP( InsertMenu   );  // A935
	TBTRAP( DeleteMenu   );  // A936
	TBTRAP( DrawMenuBar  );  // A937
	TBTRAP( HiliteMenu   );  // A938
	TBTRAP( EnableItem   );  // A939
	TBTRAP( DisableItem  );  // A93A
	TBTRAP( GetMenuBar   );  // A93B
	TBTRAP( SetMenuBar   );  // A93C
	TBTRAP( MenuSelect   );  // A93D
	TBTRAP( MenuKey      );  // A93E
	
	TBTRAP( SetItmIcon   );  // A940
	
	TBTRAP( SetItemStyle );  // A942
	TBTRAP( GetItemMark  );  // A943
	TBTRAP( SetItemMark  );  // A944
	TBTRAP( CheckItem    );  // A945
	TBTRAP( GetItem      );  // A946
	TBTRAP( SetItem      );  // A947
	
	TBTRAP( GetMHandle   );  // A949
	
	TBTRAP( PlotIcon     );  // A94B
	TBTRAP( FlashMenuBar );  // A94C
	TBTRAP( AddResMenu   );  // A94D
	
	TBTRAP( CountMItems  );  // A950
	
	TBTRAP( DelMenuItem  );  // A952
	
	TBTRAP( OpenDeskAcc  );  // A9B6
	
	TBTRAP( GetMenu      );  // A9BF
	TBTRAP( GetNewMBar   );  // A9C0
}

static void install_Controls()
{
	TBTRAP( GetCVariant    );  // A809
	
	TBTRAP( UpdateControls );  // A953
	TBTRAP( NewControl     );  // A954
	TBTRAP( DisposeControl );  // A955
	TBTRAP( KillControls   );  // A956
	TBTRAP( ShowControl    );  // A957
	TBTRAP( HideControl    );  // A958
	TBTRAP( MoveControl    );  // A959
	TBTRAP( GetCRefCon     );  // A95A
	TBTRAP( SetCRefCon     );  // A95B
	TBTRAP( SizeControl    );  // A95C
	TBTRAP( HiliteControl  );  // A95D
	TBTRAP( GetCTitle      );  // A95E
	TBTRAP( SetCTitle      );  // A95F
	TBTRAP( GetCtlValue    );  // A960
	TBTRAP( GetMinCtl      );  // A961
	TBTRAP( GetMaxCtl      );  // A962
	TBTRAP( SetCtlValue    );  // A963
	TBTRAP( SetMinCtl      );  // A964
	TBTRAP( SetMaxCtl      );  // A965
	TBTRAP( TestControl    );  // A966
	
	TBTRAP( TrackControl   );  // A968
	TBTRAP( DrawControls   );  // A969
	TBTRAP( GetCtlAction   );  // A96A
	TBTRAP( SetCtlAction   );  // A96B
	TBTRAP( FindControl    );  // A96C
	TBTRAP( Draw1Control   );  // A96D
	TBTRAP( GetNewControl  );  // A9BE
}

static void install_Dialogs()
{
	TBTRAP( HideDItem );  // A827
	TBTRAP( ShowDItem );  // A828
	
	TBTRAP( UpdateDialog  );  // A978
	TBTRAP( CouldDialog   );  // A979
	TBTRAP( FreeDialog    );  // A97A
	TBTRAP( InitDialogs   );  // A97B
	TBTRAP( GetNewDialog  );  // A97C
	TBTRAP( NewDialog     );  // A97D
	TBTRAP( SelIText      );  // A97E
	TBTRAP( IsDialogEvent );  // A97F
	TBTRAP( DialogSelect  );  // A980
	TBTRAP( DrawDialog    );  // A981
	TBTRAP( CloseDialog   );  // A982
	TBTRAP( DisposeDialog );  // A983
	TBTRAP( FindDItem     );  // A984
	TBTRAP( Alert         );  // A985
	TBTRAP( StopAlert     );  // A986
	TBTRAP( NoteAlert     );  // A987
	TBTRAP( CautionAlert  );  // A988
	TBTRAP( CouldAlert    );  // A989
	TBTRAP( FreeAlert     );  // A98A
	TBTRAP( ParamText     );  // A98B
	TBTRAP( ErrorSound    );  // A98C
	TBTRAP( GetDItem      );  // A98D
	TBTRAP( SetDItem      );  // A98E
	TBTRAP( SetIText      );  // A98F
	TBTRAP( GetIText      );  // A990
	TBTRAP( ModalDialog   );  // A991
}

static void install_TextEdit()
{
	TBTRAP( TEDispatch   );  // A83D
	TBTRAP( TEStyleNew   );  // A83E
	
	TBTRAP( TEInit       );  // A9CC
	TBTRAP( TEDispose    );  // A9CD
	TBTRAP( TETextBox    );  // A9CE
	TBTRAP( TESetText    );  // A9CF
	TBTRAP( TECalText    );  // A9D0
	TBTRAP( TESetSelect  );  // A9D1
	TBTRAP( TENew        );  // A9D2
	TBTRAP( TEUpdate     );  // A9D3
	TBTRAP( TEClick      );  // A9D4
	TBTRAP( TECopy       );  // A9D5
	TBTRAP( TECut        );  // A9D6
	TBTRAP( TEDelete     );  // A9D7
	TBTRAP( TEActivate   );  // A9D8
	TBTRAP( TEDeactivate );  // A9D9
	TBTRAP( TEIdle       );  // A9DA
	TBTRAP( TEPaste      );  // A9DB
	TBTRAP( TEKey        );  // A9DC
	TBTRAP( TEScroll     );  // A9DD
	TBTRAP( TEInsert     );  // A9DE
	TBTRAP( TESetJust    );  // A9DF
}

static void install_Packages()
{
	TBTRAP( Pack0 );  // A9E7
	
	TBTRAP( Pack3 );  // A9EA
	
	TBTRAP( Pack6 );  // A9ED
	TBTRAP( Pack7 );  // A9EE
}

static
void install_IconUtilities()
{
	TBTRAP( IconDispatch );  // ABC9
}

static
char* const* get_options( char** argv )
{
	using command::global_result;
	
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		switch ( opt )
		{
			case Opt_debug_updates:
				debug_updates = true;
				break;
			
			case Opt_live_ModalFilter:
				live_ModalFilter = true;
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

int main( int argc, char** argv )
{
	if ( argc > 0 )
	{
		char* const* args = get_options( argv );
		
		if ( *args != NULL )
		{
			WARN( "no arguments allowed" );
			
			_exit( 1 );
		}
	}
	
	install_Windows();
	install_Menus();
	install_Controls();
	install_TextEdit();
	install_Dialogs();
	
	install_IconUtilities();
	
	install_Packages();
	
	module_suspend();  // doesn't return
}
