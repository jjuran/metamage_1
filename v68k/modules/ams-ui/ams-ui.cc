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

// ams-common
#include "callouts.hh"

// ams-ui
#include "BinaryDecimal.hh"
#include "Controls.hh"
#include "Dialogs.hh"
#include "Drag.hh"
#include "Icons.hh"
#include "Menus.hh"
#include "StrUtils.hh"
#include "TextEdit.hh"
#include "Windows.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PROGRAM  "ams-ui"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


BitMap IconBitmap : 0x0A0E;

void* toolbox_trap_table[] : 3 * 1024;

#define TBTRAP( Proc )  (toolbox_trap_table[ _##Proc & 0x03FF ] = &Proc##_patch)

enum
{
	_BeginUpdate = _BeginUpDate,
	_CheckUpdate = _CheckUpDate,
	_EndUpdate   = _EndUpDate,
	_SetItemMark = _SetItmMark,
	_GetMenu     = _GetRMenu,
	_UpdateControls = _UpdtControl,
};


static
void initialize_low_memory_globals()
{
	IconBitmap.rowBytes = 4;
	IconBitmap.bounds.top  = 0;
	IconBitmap.bounds.left = 0;
	IconBitmap.bounds.bottom = 32;
	IconBitmap.bounds.right  = 32;
}

static
void install_Windows()
{
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
	old_SysBeep = (SysBeep_ProcPtr) toolbox_trap_table[ _SysBeep & 0x03FF ];
	
	TBTRAP( InsMenuItem  );  // A826
	
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
	
	TBTRAP( SetItemMark  );  // A944
	TBTRAP( CheckItem    );  // A945
	TBTRAP( GetItem      );  // A946
	TBTRAP( SetItem      );  // A947
	
	TBTRAP( GetMHandle   );  // A949
	
	TBTRAP( PlotIcon     );  // A94B
	TBTRAP( FlashMenuBar );  // A94C
	TBTRAP( AddResMenu   );  // A94D
	
	TBTRAP( CountMItems  );  // A950
	
	TBTRAP( GetMenu      );  // A9BF
	TBTRAP( GetNewMBar   );  // A9C0
	
	TBTRAP( SysBeep      );  // A9C8
}

static void install_Controls()
{
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
	
	TBTRAP( SetCTitle      );  // A95F
	TBTRAP( GetCtlValue    );  // A960
	
	TBTRAP( SetCtlValue    );  // A963
	TBTRAP( SetMinCtl      );  // A964
	TBTRAP( SetMaxCtl      );  // A965
	TBTRAP( TestControl    );  // A966
	
	TBTRAP( TrackControl   );  // A968
	TBTRAP( DrawControls   );  // A969
	TBTRAP( GetCtlAction   );  // A96A
	TBTRAP( SetCtlAction   );  // A96B
	TBTRAP( FindControl    );  // A96C
	
	TBTRAP( GetNewControl  );  // A9BE
}

static void install_Dialogs()
{
	TBTRAP( InitDialogs  );  // A97B
	TBTRAP( GetNewDialog );  // A97C
	TBTRAP( NewDialog    );  // A97D
	TBTRAP( SelIText      );  // A97E
	TBTRAP( IsDialogEvent );  // A97F
	TBTRAP( DialogSelect  );  // A980
	
	TBTRAP( DrawDialog   );  // A981
	TBTRAP( CloseDialog  );  // A982
	
	TBTRAP( DisposeDialog );  // A983
	
	TBTRAP( FindDItem    );  // A984
	TBTRAP( Alert        );  // A985
	TBTRAP( StopAlert    );  // A986
	TBTRAP( NoteAlert    );  // A987
	TBTRAP( CautionAlert );  // A988
	
	TBTRAP( ParamText    );  // A98B
	
	TBTRAP( GetDItem     );  // A98D
	TBTRAP( SetDItem     );  // A98E
	TBTRAP( SetIText     );  // A98F
	TBTRAP( GetIText     );  // A990
	TBTRAP( ModalDialog  );  // A991
}

static void install_TextEdit()
{
	TBTRAP( TEInit    );  // A9CC
	TBTRAP( TEDispose );  // A9CD
	TBTRAP( TETextBox );  // A9CE
	TBTRAP( TESetText );  // A9CF
	
	TBTRAP( TESetSelect  );  // A9D1
	TBTRAP( TENew        );  // A9D2
	TBTRAP( TEUpdate     );  // A9D3
	TBTRAP( TEClick      );  // A9D4
	
	TBTRAP( TEActivate   );  // A9D8
	TBTRAP( TEDeactivate );  // A9D9
	TBTRAP( TEIdle       );  // A9DA
	
	TBTRAP( TEKey        );  // A9DC
}

static void install_Packages()
{
	TBTRAP( Pack7 );  // A9EE
}

static
void install_IconUtilities()
{
	TBTRAP( GetIcon      );  // A9BB
	
	TBTRAP( IconDispatch );  // ABC9
}

int main( int argc, char** argv )
{
	if ( argc > 0 )
	{
		char* const* args = ++argv;
		
		if ( *args != NULL )
		{
			WARN( "no arguments allowed" );
			
			_exit( 1 );
		}
	}
	
	initialize_low_memory_globals();
	
	install_Windows();
	install_Menus();
	install_Controls();
	install_TextEdit();
	install_Dialogs();
	
	install_IconUtilities();
	
	install_Packages();
	
	module_suspend();  // doesn't return
}
