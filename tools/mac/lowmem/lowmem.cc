/*
	lowmem.cc
	---------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

// mac-sys-utils
#include "mac_sys/has/lowmem.hh"

// gear
#include "gear/hexadecimal.hh"
#include "gear/inscribe_decimal.hh"


#define COUNT( v )  (sizeof v / sizeof *v)

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define str_len( s )  s, strlen( s )


enum
{
	none,
	x8,
	x16,
	x32,
	ptr,
	i8,
	i16,
	i32,
	u8,
	u16,
	u32,
	str,
};

struct global
{
	short addr;
	short type;
	
	const char* name;
};

static const global global_defs[] =
{
	{ 0x0100, x16, "MonkeyLives" },
	{ 0x0102, u16, "ScrVRes"     },
	{ 0x0104, u16, "ScrHRes"     },
	{ 0x0106, u16, "ScreenRow"   },
	{ 0x0108, ptr, "MemTop"      },
	{ 0x010C, ptr, "BufPtr"      },
	{ 0x0110, ptr, "StkLowPt"    },
	{ 0x0114, ptr, "HeapEnd"     },
	{ 0x0118, ptr, "TheZone"     },
	{ 0x011C, ptr, "UTableBase"  },
	{ 0x0120, ptr, "MacJmp"      },
	{ 0x0124, ptr, "DskRtnAdr"   },
	{ 0x0128, ptr, "PollRtnAdr"  },
	{ 0x012C, x8,  "DskVerify"   },
	{ 0x012D, x8,  "LoadTrap"    },
	{ 0x012E, x8,  "MmInOK"      },
	{ 0x012F, x8,  "CPUFlag"     },
	{ 0x0130, ptr, "ApplLimit"   },
	{ 0x0134, ptr, "SonyVars"    },
	{ 0x0138, x16, "PWMValue"    },
	{ 0x013A, ptr, "PollStack"   },
	{ 0x013E, ptr, "PollProc"    },
	{ 0x0142, i16, "DskErr"      },
	{ 0x0144, x16, "SysEvtMask"  },
	{ 0x0146, ptr, "SysEvtBuf"   },
	
	{ 0x014A, x16, "EventQueue.qFlags" },
	{ 0x014C, ptr, "EventQueue.qHead"  },
	{ 0x0150, ptr, "EventQueue.qTail"  },
	
	{ 0x0154, u16, "EvtBufCnt"   },
	{ 0x0156, u32, "RandSeed"    },
	{ 0x015A, x16, "SysVersion"  },
	{ 0x015C, x8,  "SEvtEnb"     },
	{ 0x015D, x8,  "DSWndUpdate" },
	{ 0x015E, x8,  "FontFlag"    },
	{ 0x015F, x8,  "<filler>"    },
	
	{ 0x0160, x16, "VBLQueue.qFlags" },
	{ 0x0162, ptr, "VBLQueue.qHead"  },
	{ 0x0166, ptr, "VBLQueue.qTail"  },
	
	{ 0x016A, u32, "Ticks"       },
	{ 0x016E, u32, "MBTicks"     },
	{ 0x0172, x8,  "MBState"     },
	{ 0x0173, u8,  "Tocks"       },
	
	{ 0x0174, x32, "KeyMap[ 0 ]" },
	{ 0x0178, x32, "KeyMap[ 1 ]" },
	{ 0x017C, x32, "KeyMap[ 2 ]" },
	{ 0x0180, x32, "KeyMap[ 3 ]" },
	
	{ 0x0184, x16, "KeyLast"     },
	{ 0x0186, u32, "KeyTime"     },
	{ 0x018A, u32, "KeyRepTime"  },
	{ 0x018E, u16, "KeyThresh"   },
	{ 0x0190, u16, "KeyRepThresh"},
	
	{ 0x0192, ptr, "Lvl1DT[ 0 ]" },
	{ 0x0196, ptr, "Lvl1DT[ 1 ]" },
	{ 0x019A, ptr, "Lvl1DT[ 2 ]" },
	{ 0x019E, ptr, "Lvl1DT[ 3 ]" },
	{ 0x01A2, ptr, "Lvl1DT[ 4 ]" },
	{ 0x01A6, ptr, "Lvl1DT[ 5 ]" },
	{ 0x01AA, ptr, "Lvl1DT[ 6 ]" },
	{ 0x01AE, ptr, "Lvl1DT[ 7 ]" },
	
	{ 0x01B2, ptr, "Lvl2DT[ 0 ]" },
	{ 0x01B6, ptr, "Lvl2DT[ 1 ]" },
	{ 0x01BA, ptr, "Lvl2DT[ 2 ]" },
	{ 0x01BE, ptr, "Lvl2DT[ 3 ]" },
	{ 0x01C2, ptr, "Lvl2DT[ 4 ]" },
	{ 0x01C6, ptr, "Lvl2DT[ 5 ]" },
	{ 0x01CA, ptr, "Lvl2DT[ 6 ]" },
	{ 0x01CE, ptr, "Lvl2DT[ 7 ]" },
	
	{ 0x01D2, u16, "UnitNtryCnt" },
	{ 0x01D4, ptr, "VIA"         },
	{ 0x01D8, ptr, "SCCRd"       },
	{ 0x01DC, ptr, "SCCWr"       },
	{ 0x01E0, ptr, "IWM"         },
	
	{ 0x01E4, x32, "scratch20[ 0 ]" },
	{ 0x01E8, x32, "scratch20[ 1 ]" },
	{ 0x01EC, x32, "scratch20[ 2 ]" },
	{ 0x01F0, x32, "scratch20[ 3 ]" },
	{ 0x01F4, x32, "scratch20[ 4 ]" },
	
	{ 0x01F8, x8,  "SPValid"     },
	{ 0x01F9, x8,  "SPATalkA"    },
	{ 0x01FA, x8,  "SPATalkB"    },
	{ 0x01FB, x8,  "SPConfig"    },
	{ 0x01FC, x16, "SPPortA"     },
	{ 0x01FE, x16, "SPPortB"     },
	{ 0x0200, x32, "SPAlarm"     },
	{ 0x0204, u16, "SPFont"      },
	{ 0x0206, x8,  "SPKbd"       },
	{ 0x0207, x8,  "SPPrint"     },
	{ 0x0208, u8,  "SPVolCtl"    },
	{ 0x0209, x8,  "SPClikCaret" },
	{ 0x020A, x8,  "SPMisc"      },
	{ 0x020B, x8,  "SPMisc2"     },
	{ 0x020C, x32, "Time"        },
	{ 0x0210, i16, "BootDrive"   },
	{ 0x0212, x16, "JShell"      },
	{ 0x0214, i16, "SFSaveDisk"  },
	{ 0x0216, x16, "<unknown>"   },
	{ 0x0218, x8,  "KbdLast"     },
	{ 0x0219, x8,  "<unknown>"   },
	{ 0x021A, ptr, "JKybdTask"   },
	{ 0x021E, x8,  "KbdType"     },
	{ 0x021F, x8,  "AlarmState"  },
	{ 0x0220, i16, "MemErr"      },
	
	{ 0x0222, x32, "DiskVars[  0 ]" },
	{ 0x0226, x32, "DiskVars[  1 ]" },
	{ 0x022A, x32, "DiskVars[  2 ]" },
	{ 0x022E, x32, "DiskVars[  3 ]" },
	{ 0x0232, x32, "DiskVars[  4 ]" },
	{ 0x0236, x32, "DiskVars[  5 ]" },
	{ 0x023A, x32, "DiskVars[  6 ]" },
	{ 0x023E, x32, "DiskVars[  7 ]" },
	{ 0x0242, x32, "DiskVars[  8 ]" },
	{ 0x0246, x32, "DiskVars[  9 ]" },
	{ 0x024A, x32, "DiskVars[ 10 ]" },
	{ 0x024E, x32, "DiskVars[ 11 ]" },
	{ 0x0252, x32, "DiskVars[ 12 ]" },
	{ 0x0256, x32, "DiskVars[ 13 ]" },
	{ 0x025A, x32, "DiskVars[ 14 ]" },
	
	{ 0x025E, x16, "FlEvtMask"   },
	{ 0x0260, x8,  "SdVolume"    },
	{ 0x0261, x8,  "SdEnable"    },
	{ 0x0262, ptr, "SoundPtr"    },
	{ 0x0266, ptr, "SoundBase"   },
	
	{ 0x026A, ptr, "SoundVBL.qLink"    },
	{ 0x026E, x16, "SoundVBL.qType"    },
	{ 0x0270, ptr, "SoundVBL.vblAddr"  },
	{ 0x0274, i16, "SoundVBL.vblCount" },
	{ 0x0276, i16, "SoundVBL.vblPhase" },
	
	{ 0x0278, x16, "<filler>"    },
	{ 0x027A, ptr, "SoundDCE"    },
	{ 0x027E, x8,  "SoundActive" },
	{ 0x027F, u8,  "SoundLevel"  },
	{ 0x0280, u16, "CurPitch"    },
	{ 0x0282, x32, "SoundLast"   },
	{ 0x0286, x32, "<unknown>"   },
	{ 0x028A, x32, "<unknown>"   },
	{ 0x028E, x16, "ROM85"       },
	{ 0x0290, x8,  "PortAUse"    },
	{ 0x0291, x8,  "PortBUse"    },
	
	{ 0x0292, x32, "<unknown>"   },
	{ 0x0296, x32, "<unknown>"   },
	{ 0x029A, ptr, "JGNEFilter"  },
	{ 0x029E, ptr, "Key1Trans"   },
	{ 0x02A2, ptr, "Key2Trans"   },
	{ 0x02A6, ptr, "SysZone"     },
	{ 0x02AA, ptr, "ApplZone"    },
	{ 0x02AE, ptr, "ROMBase"     },
	{ 0x02B2, ptr, "RAMBase"     },
	{ 0x02B6, ptr, "BasicGlob"   },
	{ 0x02BA, ptr, "DSAlertTab"  },
	
	{ 0x02BE, ptr, "ExtStsDT[ 0 ]" },
	{ 0x02C2, ptr, "ExtStsDT[ 1 ]" },
	{ 0x02C6, ptr, "ExtStsDT[ 2 ]" },
	{ 0x02CA, ptr, "ExtStsDT[ 3 ]" },
	
	{ 0x02CE, x8,  "SCCASts"     },
	{ 0x02CF, x8,  "SCCBSts"     },
	{ 0x02D0, x32, "<unknown>"   },
	{ 0x02D4, x32, "<unknown>"   },
	{ 0x02D8, ptr, "ABusVars"    },
	{ 0x02DC, x32, "<unknown>"   },
	{ 0x02E0, str, "FinderName"  },
	{ 0x02F0, u32, "DoubleTime"  },
	{ 0x02F4, u32, "CaretTime"   },
	{ 0x02F8, x8,  "ScrDmpEnb"   },
	{ 0x02F9, x8,  "ScrDmpType"  },
	{ 0x02FA, x16, "<unknown>"   },
	{ 0x02FC, u32, "BufTgFNum"   },
	{ 0x0300, x16, "BufTgFFlg"   },
	{ 0x0302, u16, "BufTgFBkNum" },
	{ 0x0304, x32, "BufTgDate"   },
	
	{ 0x0308, x16, "DrvQHdr.qFlags" },
	{ 0x030A, ptr, "DrvQHdr.qHead"  },
	{ 0x030E, ptr, "DrvQHdr.qTail"  },
	
	{ 0x0312, ptr, "PWMBuf2"     },
	{ 0x0316, x32, "HpChk"       },
	{ 0x031A, x32, "Lo3Bytes"    },
	{ 0x031E, x32, "MinStack"    },
	{ 0x0322, x32, "DefltStack"  },
	{ 0x0326, x16, "MMDefFlags"  },
	{ 0x0328, ptr, "GZRootHnd"   },
	{ 0x032C, ptr, "GZRootPtr"   },
	{ 0x0330, ptr, "GZMoveHnd"   },
	{ 0x0334, ptr, "DSDrawProc"  },
	{ 0x0338, ptr, "EjectNotify" },
	{ 0x033C, ptr, "IAZNotify"   },
	
	{ 0x0340, u16, "CurDB"       },
	{ 0x0342, u16, "NxtDB"       },
	{ 0x0344, u16, "MaxDB"       },
	{ 0x0346, x8,  "FlushOnly"   },
	{ 0x0347, x8,  "RegRsrc"     },
	{ 0x0348, x8,  "FLckUnlck"   },
	{ 0x0349, x8,  "FrcSync"     },
	{ 0x034A, x8,  "NewMount"    },
	{ 0x034B, x8,  "NoEject"     },
	{ 0x034C, u16, "DrMstrBlk"   },
	{ 0x034E, ptr, "FCBSPtr"     },
	{ 0x0352, ptr, "DefVCBPtr"   },
	
	{ 0x0356, x16, "VCBQHdr.qFlags" },
	{ 0x0358, ptr, "VCBQHdr.qHead"  },
	{ 0x035C, ptr, "VCBQHdr.qTail"  },
	
	{ 0x0360, x16, "FSQHdr.qFlags" },
	{ 0x0362, ptr, "FSQHdr.qHead"  },
	{ 0x0366, ptr, "FSQHdr.qTail"  },
	
	{ 0x036A, ptr, "HFSStkTop"   },
	{ 0x036E, ptr, "HFSStkPtr"   },
	{ 0x0372, ptr, "WDCBsPtr"    },
	{ 0x0376, x8,  "HFSFlags"    },
	{ 0x0377, x8,  "CacheFlag"   },
	{ 0x0378, ptr, "SysBMCPtr"   },
	{ 0x037C, ptr, "SysVolCPtr"  },
	{ 0x0380, ptr, "SysCtlCPtr"  },
	{ 0x0384, i16, "DefVRefNum"  },
	{ 0x0386, ptr, "PMSPPtr"     },
	{ 0x038A, x32, "<unknown>"   },
	{ 0x038E, x32, "<unknown>"   },
	{ 0x0392, i16, "HFSDSErr"    },
	
	{ 0x0394, x32, "CacheVars?"  },
	{ 0x0398, i16, "CurDirStore" },
	{ 0x039A, x16, "<unknown>"   },
	{ 0x039C, x32, "CacheCom"    },
	{ 0x03A0, x16, "<unknown>"   },
	{ 0x03A2, i16, "ErCode"      },
	
	{ 0x03A4, ptr, "Params.qLink" },
	{ 0x03A8, i16, "Params.qType" },
	{ 0x03AA, x16, "Params.ioTrap" },
	{ 0x03AC, ptr, "Params.ioCmdAddr" },
	{ 0x03B0, ptr, "Params.ioCompletion" },
	{ 0x03B4, i16, "Params.ioResult" },
	{ 0x03B6, ptr, "Params.ioNamePtr" },
	{ 0x03BA, i16, "Params.ioVRefNum" },
	{ 0x03BC, i16, "Params.ioRefNum" },
	{ 0x03BE, i8,  "Params.ioVersNum" },
	{ 0x03BF, x8,  "Params.ioPermssn" },
	{ 0x03C0, ptr, "Params.ioMisc" },
	{ 0x03C4, ptr, "Params.ioBuffer" },
	{ 0x03C8, i32, "Params.ioReqCount" },
	{ 0x03CC, i32, "Params.ioActCount" },
	{ 0x03D0, x16, "Params.ioPosMode" },
	{ 0x03D2, i32, "Params.ioPosOffset" },
	
	{ 0x03D6, x32, "FSTemp8[ 0 ]" },
	{ 0x03DA, x32, "FSTemp8[ 1 ]" },
	
	{ 0x03DE, i16, "FSIOErr"     },
	{ 0x03E0, x16, "<unknown>"   },
	{ 0x03E2, ptr, "FSQueueHook" },
	{ 0x03E6, ptr, "ExtFSHook"   },
	{ 0x03EA, ptr, "DskSwtchHook"},
	{ 0x03EE, ptr, "ReqstVol"    },
	{ 0x03F2, ptr, "ToExtFS"     },
	{ 0x03F6, i16, "FSFCBLen"    },
	
	{ 0x03F8, x32, "DSAlertRect[ 0 ]" },
	{ 0x03FC, x32, "DSAlertRect[ 1 ]" },
	
	{ 0x0800, ptr, "JHideCursor" },
	{ 0x0804, ptr, "JShowCursor" },
	{ 0x0808, ptr, "JShieldCursor"},
	{ 0x080C, ptr, "JScrnAddr"   },
	{ 0x0810, ptr, "JScrnSize"   },
	{ 0x0814, ptr, "JInitCrsr"   },
	{ 0x0818, ptr, "JSetCrsr"    },
	{ 0x081C, ptr, "JCrsrObscure"},
	{ 0x0820, ptr, "JUpdateProc" },
	{ 0x0824, ptr, "ScrnBase"    },
	{ 0x0828, x32, "MTemp"       },
	{ 0x082C, x32, "RawMouse"    },
	{ 0x0830, x32, "Mouse"       },
	
	{ 0x0834, x32, "CrsrPin[ 0 ]" },
	{ 0x0838, x32, "CrsrPin[ 1 ]" },
	
	{ 0x083C, x32, "CrsrRect[ 0 ]" },
	{ 0x0840, x32, "CrsrRect[ 1 ]" },
	
	{ 0x0844, x32, "TheCrsr.data[ 0 ]" },
	{ 0x0848, x32, "TheCrsr.data[ 1 ]" },
	{ 0x084C, x32, "TheCrsr.data[ 2 ]" },
	{ 0x0850, x32, "TheCrsr.data[ 3 ]" },
	{ 0x0854, x32, "TheCrsr.data[ 4 ]" },
	{ 0x0858, x32, "TheCrsr.data[ 5 ]" },
	{ 0x085C, x32, "TheCrsr.data[ 6 ]" },
	{ 0x0860, x32, "TheCrsr.data[ 7 ]" },
	
	{ 0x0864, x32, "TheCrsr.mask[ 0 ]" },
	{ 0x0868, x32, "TheCrsr.mask[ 1 ]" },
	{ 0x086C, x32, "TheCrsr.mask[ 2 ]" },
	{ 0x0870, x32, "TheCrsr.mask[ 3 ]" },
	{ 0x0874, x32, "TheCrsr.mask[ 4 ]" },
	{ 0x0878, x32, "TheCrsr.mask[ 5 ]" },
	{ 0x087C, x32, "TheCrsr.mask[ 6 ]" },
	{ 0x0880, x32, "TheCrsr.mask[ 7 ]" },
	
	{ 0x0884, x32, "TheCrsr.hotspot" },
	
	{ 0x0888, ptr, "CrsrAddr"    },
	{ 0x088C, x32, "CrsrSave"    },
	{ 0x0890, x32, "<unknown>"   },
	{ 0x0894, x32, "<unknown>"   },
	{ 0x0898, x32, "<unknown>"   },
	{ 0x089C, x32, "<unknown>"   },
	{ 0x08A0, x32, "<unknown>"   },
	{ 0x08A4, x32, "MainDevice"  },
	{ 0x08A8, x32, "DeviceList"  },
	{ 0x08AC, x32, "<unknown>"   },
	
	{ 0x08B0, x32, "QDColors?"   },
	{ 0x08B4, x32, "QDColors?"   },
	{ 0x08B8, x32, "QDColors?"   },
	{ 0x08BC, x32, "QDColors?"   },
	{ 0x08C0, x32, "QDColors?"   },
	{ 0x08C4, x32, "QDColors?"   },
	{ 0x08C8, x32, "QDColors?"   },
	
	{ 0x08CC, x8,  "CrsrVis"     },
	{ 0x08CD, x8,  "CrsrBusy"    },
	{ 0x08CE, x8,  "CrsrNew"     },
	{ 0x08CF, x8,  "CrsrCouple"  },
	{ 0x08D0, i16, "CrsrState"   },
	{ 0x08D2, x8,  "CrsrObscure" },
	{ 0x08D3, x8,  "CrsrScale"   },
	{ 0x08D4, x16, "<unknown>"   },
	{ 0x08D6, x32, "MouseMask"   },
	{ 0x08DA, x32, "MouseOffset" },
	{ 0x08DE, x16, "JournalFlag" },
	{ 0x08E0, ptr, "JSwapFont"   },
	{ 0x08E4, ptr, "WidthListHand"},
	{ 0x08E8, i16, "JournalRef"  },
	{ 0x08EA, x16, "<unknown>"   },
	{ 0x08EC, x16, "CrsrThresh"  },
	{ 0x08EE, ptr, "JCrsrTask"   },
	{ 0x08F2, x8,  "WWExist"     },
	{ 0x08F3, x8,  "DExist"      },
	{ 0x08F4, ptr, "JFetch"      },
	{ 0x08F8, ptr, "JStash"      },
	{ 0x08FC, ptr, "JIODone"     },
	{ 0x0900, i16, "CurApRefNum" },
	{ 0x0902, x8,  "LaunchFlag"  },
	{ 0x0903, x8,  "<unknown>"   },
	{ 0x0904, ptr, "CurrentA5"   },
	{ 0x0908, ptr, "CurStackBase"},
	{ 0x090C, x32, "<unknown>"   },
	{ 0x0910, str, "CurApName"   },
	{ 0x0930, ptr, "SaveSegHandle"},
	{ 0x0934, i16, "CurJTOffset" },
	{ 0x0936, i16, "CurPageOption"},
	{ 0x0938, x16, "HiliteMode"  },
	
	{ 0x093A, x32, "LoaderPBlock[ 0 ]"},
	{ 0x093E, x32, "LoaderPBlock[ 1 ]"},
	{ 0x0942, x16, "LoaderPBlock[ 2 ]"},
	
	{ 0x0944, i16, "PrintErr"    },
	{ 0x0946, x32, "<unknown>"   },
	{ 0x094A, x32, "<unknown>"   },
	{ 0x094E, x32, "<unknown>"   },
	{ 0x0952, x16, "<unknown>"   },
	{ 0x0954, x32, "LastPGlobal" },
	{ 0x0958, x32, "<unknown>"   },
	{ 0x095C, x32, "<unknown>"   },
	{ 0x0960, u32, "scrapSize"   },
	{ 0x0964, ptr, "scrapHandle" },
	{ 0x0968, i16, "scrapCount"  },
	{ 0x096A, i16, "scrapState"  },
	{ 0x096C, ptr, "scrapName"   },
	{ 0x0970, str, "scrapTag"    },
	{ 0x0980, ptr, "RomFont0"    },
	{ 0x0984, i16, "ApFontID"    },
	{ 0x0986, x8,  "GotStrike"   },
	{ 0x0987, u8,  "FMDefaultSize"},
	{ 0x0988, u16, "CurFMFamily" },
	{ 0x098A, u16, "CurFMSize"   },
	{ 0x098C, x8,  "CurFMFace"   },
	{ 0x098D, x8,  "CurFMNeedBits"},
	{ 0x098E, i16, "CurFMDevice" },
	{ 0x0990, x32, "CurFMNumer"  },
	{ 0x0994, x32, "CurFMDenom"  },
	{ 0x0998, i16, "FOutError"   },
	{ 0x099A, ptr, "FOutFontHandle"},
	
	// FOutBold
	// ...
	// FMStyleTab
	
	{ 0x09CE, x32, "ToolScratch[ 0 ]" },
	{ 0x09D2, x32, "ToolScratch[ 1 ]" },
	
	{ 0x09D6, ptr, "WindowList"  },
	{ 0x09DA, x16, "SaveUpdate"  },
	{ 0x09DC, x16, "PaintWhite"  },
	{ 0x09DE, ptr, "WMgrPort"    },
	{ 0x09E2, ptr, "DeskPort"    },
	{ 0x09E6, ptr, "OldStructure"},
	{ 0x09EA, ptr, "OldContent"  },
	{ 0x09EE, ptr, "GrayRgn"     },
	{ 0x09F2, ptr, "SaveVisRgn"  },
	{ 0x09F6, ptr, "DragHook"    },
	
	{ 0x09FA, x32, "TempRect[ 0 ]" },
	{ 0x09FE, x32, "TempRect[ 1 ]" },
	
	{ 0x0A02, x32, "OneOne"      },
	{ 0x0A06, x32, "MinusOne"    },
	{ 0x0A0A, i16, "TopMenuItem" },
	{ 0x0A0C, i16, "AtMenuBottom"},
	
	{ 0x0A0E, ptr, "IconBitmap.baseAddr" },
	{ 0x0A12, u16, "IconBitmap.rowBytes" },
	{ 0x0A14, x32, "IconBitmap.bounds.topLeft"  },
	{ 0x0A18, x32, "IconBitmap.bounds.botRight" },
	
	{ 0x0A1C, ptr, "MenuList"    },
	{ 0x0A20, x16, "MBarEnable"  },
	{ 0x0A22, i16, "CurDeKind"   },
	{ 0x0A24, u16, "MenuFlash"   },
	{ 0x0A26, i16, "TheMenu"     },
	{ 0x0A28, ptr, "SavedHandle" },
	{ 0x0A2C, ptr, "MBarHook"    },
	{ 0x0A30, ptr, "MenuHook"    },
	
	{ 0x0A34, x32, "DragPattern[ 0 ]" },
	{ 0x0A38, x32, "DragPattern[ 1 ]" },
	
	{ 0x0A3C, x32, "DeskPattern[ 0 ]" },
	{ 0x0A40, x32, "DeskPattern[ 1 ]" },
	
	{ 0x0A44, x16, "DragFlag"    },
	{ 0x0A46, ptr, "CurDragAction"},
	
	{ 0x0A4A, x16, "FPState[ 0 ]" },
	{ 0x0A4C, x32, "FPState[ 1 ]" },
	
	{ 0x0A50, ptr, "TopMapHndl"  },
	{ 0x0A54, ptr, "SysMapHndl"  },
	{ 0x0A58, i16, "SysMap"      },
	{ 0x0A5A, i16, "CurMap"      },
	{ 0x0A5C, x16, "ResReadOnly" },
	{ 0x0A5E, x16, "ResLoad"     },
	{ 0x0A60, i16, "ResErr"      },
	{ 0x0A62, x8,  "TaskLock"    },
	{ 0x0A63, x8,  "FScaleDisable"},
	{ 0x0A64, ptr, "CurActivate" },
	{ 0x0A68, ptr, "CurDeactive" },
	{ 0x0A6C, ptr, "DeskHook"    },
	{ 0x0A70, ptr, "TEDoText"    },
	{ 0x0A74, ptr, "TERecal"     },
	
	{ 0x0A78, x32, "ApplScratch[ 0 ]" },
	{ 0x0A7C, x32, "ApplScratch[ 1 ]" },
	{ 0x0A80, x32, "ApplScratch[ 2 ]" },
	
	{ 0x0A84, ptr, "GhostWindow" },
	{ 0x0A88, ptr, "CloseOrnHook"},
	{ 0x0A8C, ptr, "ResumeProc"  },
	{ 0x0A90, ptr, "SaveProc"    },
	{ 0x0A94, ptr, "SaveSP"      },
	{ 0x0A98, i16, "ANumber"     },
	{ 0x0A9A, i16, "ACount"      },
	{ 0x0A9C, ptr, "DABeeper"    },
	
	{ 0x0A9C, ptr, "DAStrings[ 0 ]" },
	{ 0x0AA0, ptr, "DAStrings[ 1 ]" },
	{ 0x0AA4, ptr, "DAStrings[ 2 ]" },
	{ 0x0AA8, ptr, "DAStrings[ 3 ]" },
	
	{ 0x0AB0, i16, "TEScrpLengt" },
	{ 0x0AB2, x16, "<unknown>"   },
	{ 0x0AB4, ptr, "TEScrpHandl" },
	
	{ 0x0AB8, ptr, "AppPacks[ 0 ]" },
	{ 0x0ABC, ptr, "AppPacks[ 1 ]" },
	{ 0x0AC0, ptr, "AppPacks[ 2 ]" },
	{ 0x0AC4, ptr, "AppPacks[ 3 ]" },
	{ 0x0AC8, ptr, "AppPacks[ 4 ]" },
	{ 0x0ACC, ptr, "AppPacks[ 5 ]" },
	{ 0x0AD0, ptr, "AppPacks[ 6 ]" },
	{ 0x0AD4, ptr, "AppPacks[ 7 ]" },
	{ 0x0AD8, str, "SysResName"  },
	{ 0x0AE8, x32, "<unknown>"   },
	{ 0x0AEC, ptr, "AppParmHandle"},
	{ 0x0AF0, i16, "DSErrCode"   },
	{ 0x0AF2, ptr, "ResErrProc"  },
	{ 0x0AF6, ptr, "TEWdBreak"   },
	{ 0x0AFA, i16, "DlgFont"     },
	{ 0x0AFC, x32, "LastTGlobal" },
	{ 0x0B00, x32, "TrapAgain"   },
	{ 0x0B04, x16, "<unknown>"   },
	{ 0x0B06, ptr, "ROMMapHndl"  },
	{ 0x0B0A, ptr, "PWMBuf1"     },
	{ 0x0B0E, x16, "BootMask"    },
	{ 0x0B10, ptr, "WidthPtr"    },
	{ 0x0B14, ptr, "AtalkHk1"    },
	{ 0x0B18, ptr, "AtalkHk2"    },
	{ 0x0B1C, x32, "<unknown>"   },
	{ 0x0B20, x16, "<unknown>"   },
	{ 0x0B22, x16, "HwCfgFlags"  },
	{ 0x0B24, x16, "<unknown>"   },
	{ 0x0B26, x32, "<unknown>"   },
	{ 0x0B2A, ptr, "WidthTabHandle"},
	{ 0x0B2E, x16, "<unknown>"   },
	{ 0x0B30, x32, "<unknown>"   },
	{ 0x0B34, i16, "BtDskRfn"    },
	
	{ 0x0B36, x32, "BootTmp8[ 0 ]" },
	{ 0x0B3A, x32, "BootTmp8[ 1 ]" },
	{ 0x0B3E, x8,  "<unknown>"   },
	{ 0x0B3F, x8,  "T1Arbitrate" },
	{ 0x0B40, x32, "<unknown>"   },
	{ 0x0B44, x32, "<unknown>"   },
	{ 0x0B48, x32, "<unknown>"   },
	{ 0x0B4C, x32, "<unknown>"   },
	{ 0x0B50, x32, "<unknown>"   },
	
	{ 0x0B54, i16, "MenuDisable resID" },
	{ 0x0B56, i16, "MenuDisable item"  },
	
	{ 0x0B58, x32, "<unknown>"   },
	{ 0x0B5C, x32, "<unknown>"   },
	{ 0x0B60, x32, "<unknown>"   },
	{ 0x0B64, x32, "<unknown>"   },
	{ 0x0B68, x32, "<unknown>"   },
	{ 0x0B6C, x32, "<unknown>"   },
	{ 0x0B70, x32, "<unknown>"   },
	{ 0x0B74, x32, "<unknown>"   },
	{ 0x0B78, x32, "<unknown>"   },
	{ 0x0B7C, x32, "<unknown>"   },
	{ 0x0B80, x32, "<unknown>"   },
	{ 0x0B84, x32, "<unknown>"   },
	{ 0x0B88, x32, "<unknown>"   },
	{ 0x0B8C, x32, "<unknown>"   },
	{ 0x0B90, x32, "<unknown>"   },
	{ 0x0B94, x32, "<unknown>"   },
	{ 0x0B98, x32, "<unknown>"   },
	{ 0x0B9C, x16, "<unknown>"   },
	{ 0x0B9E, x8,  "RomMapInsert"},
	{ 0x0B9F, x8,  "TmpResLoad"  },
	{ 0x0BA0, x32, "IntlSpec"    },
	{ 0x0BA4, x16, "<unknown>"   },
	{ 0x0BA6, i16, "SysFontFam"  },
	{ 0x0BA8, i16, "SysFontSize" },
	{ 0x0BAA, i16, "MBarHeight"  },
	{ 0x0BAC, x32, "<unknown>"   },
	{ 0x0BB0, x32, "<unknown>"   },
	{ 0x0BB4, x32, "<unknown>"   },
	{ 0x0BB8, x32, "<unknown>"   },
	{ 0x0BBC, x32, "<unknown>"   },
	{ 0x0BC0, x16, "NewUnused"   },
	{ 0x0BC2, ptr, "LastFOND"    },
	
	{ 0x0BC6, x16, "<unknown>"   },
	{ 0x0BC8, x32, "<unknown>"   },
	{ 0x0BCC, x32, "<unknown>"   },
	{ 0x0BD0, x32, "<unknown>"   },
	{ 0x0BD4, x32, "<unknown>"   },
	{ 0x0BD8, x32, "<unknown>"   },
	{ 0x0BDC, x32, "<unknown>"   },
	{ 0x0BE0, x32, "<unknown>"   },
	{ 0x0BE4, x32, "<unknown>"   },
	{ 0x0BE8, x32, "<unknown>"   },
	{ 0x0BEC, x32, "<unknown>"   },
	{ 0x0BF0, x32, "<unknown>"   },
	
	{ 0x0BF4, x8,  "FractEnable" },
	{ 0x0BF5, x8,  "<unknown>"   },
	{ 0x0BF6, x32, "<unknown>"   },
	{ 0x0BFA, x32, "<unknown>"   },
	{ 0x0BFE, x16, "<unknown>"   },
	
	// ---
	
	{ 0x06B8, ptr, "JADBProc"    },
	
	{ 0x0CB2, x8,  "MMU32Bit"    },
	
	{ 0x0CC8, ptr, "TheGDevice"  },
	
	{ 0x0CD0, ptr, "AuxWinHead"  },
	
	{ 0x0D28, ptr, "JVBLTask"    },
	
	{ 0x0D32, ptr, "SynListHandle" },
	
	{ 0x0D50, x32, "MenuCInfo?"  },
	
	{ 0x0D92, x16, "DTQHdr.qFlags" },
	{ 0x0D94, ptr, "DTQHdr.qHead"  },
	{ 0x0D98, ptr, "DTQHdr.qTail"  },
	
	{ 0x0D9C, ptr, "JDTInstall"  },
	
	{ 0x0DA0, x16, "HiliteRGB.red"   },
	{ 0x0DA2, x16, "HiliteRGB.green" },
	{ 0x0DA4, x16, "HiliteRGB.blue"  },
};

#define NOPE  "Low memory exists only in Mac OS 9 and earlier, not Mac OS X"

int main( int argc, char** argv )
{
	using mac::sys::has_lowmem;
	
	if ( ! has_lowmem() )
	{
		write( STDERR_FILENO, STR_LEN( "lowmem: " NOPE "\n" ) );
		
		return 1;
	}
	
	char buffer[ 256 ];
	
	for ( int i = 0;  i < COUNT( global_defs );  ++i )
	{
		const global& g = global_defs[ i ];
		
		long addr = (long) g.addr;
		
		write( STDOUT_FILENO, str_len( g.name ) );
		
		write( STDOUT_FILENO, STR_LEN( ": " ) );
		
		const char* p = buffer;
		
		switch ( g.type )
		{
			case x8:
				buffer[ 0 ] = '$';
				gear::encode_8_bit_hex( *(char*) addr, buffer + 1 );
				buffer[ 3 ] = '\0';
				break;
			
			case x16:
				buffer[ 0 ] = '$';
				gear::encode_16_bit_hex( *(short*) addr, buffer + 1 );
				buffer[ 5 ] = '\0';
				break;
			
			case ptr:
				if ( *(long*) addr == 0 )
				{
					p = "<null>";
					break;
				}
				
				if ( *(long*) addr == -1 )
				{
					p = "<invalid>";
					break;
				}
				// fall through
				
			case x32:
				buffer[ 0 ] = '$';
				gear::encode_32_bit_hex( *(long*) addr, buffer + 1 );
				buffer[ 9 ] = '\0';
				break;
			
			case i8:
				p = gear::inscribe_decimal( *(char*) addr );
				break;
			
			case i16:
				p = gear::inscribe_decimal( *(short*) addr );
				break;
			
			case i32:
				p = gear::inscribe_decimal( *(long*) addr );
				break;
			
			case u8:
				p = gear::inscribe_unsigned_decimal( *(unsigned char*) addr );
				break;
			
			case u16:
				p = gear::inscribe_unsigned_decimal( *(unsigned short*) addr );
				break;
			
			case u32:
				p = gear::inscribe_unsigned_decimal( *(unsigned long*) addr );
				break;
			
			case str:
				{
					unsigned char* p = (unsigned char*) addr;
					
					long len = *p++;
					
					p = (unsigned char*) mempcpy( buffer, p, len );
					
					*p = '\0';
				}
				
				break;
			
			default:
				p = "???";
				break;
		}
		
		write( STDOUT_FILENO, str_len( p ) );
		
		write( STDOUT_FILENO, STR_LEN( "\n" ) );
	}
	
	return 0;
}
