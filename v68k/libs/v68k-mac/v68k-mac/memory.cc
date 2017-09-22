/*
	memory.cc
	---------
*/

#include "v68k-mac/memory.hh"

// Standard C
#include <string.h>

// v68k
#include "v68k/endian.hh"

// v68k-mac
#include "v68k-mac/dynamic_globals.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif


namespace v68k {
namespace mac  {

bool ticking;

enum
{
	tag_ScreenRow,
	tag_MemTop,
	tag_MemTop_low_word,
	tag_HeapEnd,
	tag_HeapEnd_low_word,
	tag_TheZone,
	tag_TheZone_low_word,
	tag_UTableBase,
	tag_UTableBase_low_word,
	tag_MMOk_CPUFlag,
	tag_ApplLimit,
	tag_ApplLimit_low_word,
	tag_SysEvtMask,
	tag_SysEvtBuf,
	tag_SysEvtBuf_low_word,
	tag_EventQueue,
	tag_EventQueue_word_4 = tag_EventQueue + 4,  // 10 bytes
	tag_EvtBufCnt,
	tag_SysVersion,
	tag_VBLQueue,
	tag_VBLQueue_word_4 = tag_VBLQueue + 4,  // 10 bytes
	tag_Ticks,
	tag_Ticks_low_word,
	tag_MBState_esc,
	tag_KeyMaps,
	tag_KeyMaps_word_7 = tag_KeyMaps + 7,  // 16 bytes
	tag_UnitNtryCnt,
	tag_VIA,
	tag_VIA_low_word,
	tag_SPATBConfig,
	tag_SPKbdPrint,
	tag_Time,
	tag_Time_low_word,
	tag_BootDrive,
	tag_SFSaveDisk,
	tag_Kbd_Alarm,
	tag_MemErr,
	tag_SdVolEnable,
	tag_SoundBase,
	tag_SoundBase_low_word,
	tag_SoundActLvl,
	tag_CurPitch,
	tag_ROM85,
	tag_PortABUse,
	tag_SysZone,
	tag_SysZone_low_word,
	tag_ApplZone,
	tag_ApplZone_low_word,
	tag_ROMBase,
	tag_ROMBase_low_word,
	tag_CaretTime,
	tag_CaretTime_low_word,
	tag_ScrDmpEnbTyp,
	tag_DrvQHdr,
	tag_DrvQHdr_word_4 = tag_DrvQHdr + 4,  // 10 bytes
	tag_HpChk,
	tag_HpChk_low_word,
	tag_DefltStack,
	tag_DefltStack_low_word,
	tag_IAZNotify,
	tag_IAZNotify_low_word,
	tag_FCBSPtr,
	tag_FCBSPtr_low_word,
	tag_DefVCBPtr,
	tag_DefVCBPtr_low_word,
	tag_VCBQHdr,
	tag_VCBQHdr_word_4 = tag_VCBQHdr + 4,  // 10 bytes
	tag_FSFCBLen,
	tag_JHideCursor,
	tag_JHideCursor_low_word,
	tag_JShowCursor,
	tag_JShowCursor_low_word,
	tag_JInitCrsr,
	tag_JInitCrsr_low_word,
	tag_JSetCrsr,
	tag_JSetCrsr_low_word,
	tag_ScrnBase,
	tag_ScrnBase_low_word,
	tag_Mouse,
	tag_Mouse_low_word,
	tag_CrsrPin,
	tag_CrsrPin_word_3 = tag_CrsrPin + 3,  // 8 bytes
	tag_CrsrRect,
	tag_CrsrRect_word_3 = tag_CrsrRect + 3,  // 8 bytes
	tag_TheCrsr,
	tag_TheCrsr_word_33 = tag_TheCrsr + 33,  // 68 bytes
	tag_CrsrVisBusy,
	tag_JIODone,
	tag_JIODone_low_word,
	tag_CurApRefNum,
	tag_CurrentA5,
	tag_CurrentA5_low_word,
	tag_CurStackBase,
	tag_CurStackBase_low_word,
	tag_CurApName,
	tag_CurApName_word_15 = tag_CurApName + 15,  // 32 bytes
	tag_SaveSegHandle,
	tag_SaveSegHandle_low_word,
	tag_CurJTOffset,
	tag_CurPageOption,
	tag_PrintErr,
	tag_ROMFont0,
	tag_ROMFont0_low_word,
	tag_ToolScratch,
	tag_ToolScratch_word_3 = tag_ToolScratch + 3,  // 8 bytes
	tag_WindowList,
	tag_WindowList_low_word,
	tag_SaveUpdate,
	tag_PaintWhite,
	tag_WMgrPort,
	tag_WMgrPort_low_word,
	tag_OldStructure,
	tag_OldStructure_low_word,
	tag_OldContent,
	tag_OldContent_low_word,
	tag_GrayRgn,
	tag_GrayRgn_low_word,
	tag_SaveVisRgn,
	tag_SaveVisRgn_low_word,
	tag_IconBitmap,
	tag_IconBitmap_word_6 = tag_IconBitmap + 6,  // 14 bytes
	tag_TheMenu,
	tag_DragPattern,
	tag_DragPattern_word_3 = tag_DragPattern + 3,  // 8 bytes
	tag_DeskPattern,
	tag_DeskPattern_word_3 = tag_DeskPattern + 3,  // 8 bytes
	tag_FPState,
	tag_FPState_word_2 = tag_FPState + 2,
	tag_TopMapHndl,
	tag_TopMapHndl_low_word,
	tag_SysMap,
	tag_CurMap,
	tag_ResLoad,
	tag_ResErr,
	tag_CurActivate,
	tag_CurActivate_low_word,
	tag_CurDeactive,
	tag_CurDeactive_low_word,
	tag_ApplScratch,
	tag_ApplScratch_word_5 = tag_ApplScratch + 5,  // 12 bytes
	tag_ACount,
	tag_DAStrings,
	tag_DAStrings_word_7 = tag_DAStrings + 7,  // 16 bytes
	tag_TEScrpLength,
	tag_TEScrpHandle,
	tag_TEScrpHandle_low_word,
	tag_AppPacks,
	tag_AppPacks_word_15 = tag_AppPacks + 15,  // 32 bytes
	tag_AE8,
	tag_AE8_low_word,
	tag_AppParmHandle,
	tag_AppParmHandle_low_word,
	tag_DlgFont,
	tag_SCSIFlag,
	tag_MBarHeight,
	tag_last_A_trap,
	n_words
};

static uint16_t words[ n_words ];

static
void initialize()
{
	((char*) &words[ tag_ROMBase ])[ 1 ] = 0x40;  // ROMBase = 0x00400000
	
	words[ tag_VIA        ] = iota::big_u16( 0x00EF     );
	words[ tag_VIA + 1    ] = iota::big_u16(     0xE1FE );
	words[ tag_BootDrive  ] = 0xFFFF;  // initially -1
	words[ tag_ROM85      ] = 0xFFFF;  // indicates 64K ROM
	words[ tag_FSFCBLen   ] = 0xFFFF;  // indicates MFS
	words[ tag_CurApRefNum] = iota::big_u16( 2 );
	words[ tag_SaveUpdate ] = 0xFFFF;  // initially true
	words[ tag_PaintWhite ] = 0xFFFF;  // initially true
	words[ tag_AE8        ] = 0xFFFF;
	words[ tag_AE8 + 1    ] = 0xFFFF;
	words[ tag_MBarHeight ] = 0xFFFF;  // signals to use default menu bar height
}

static int initialization = (initialize(), 0);

short get_CurPageOption()
{
	return words[ tag_CurPageOption ];
}

struct global
{
	int16_t  addr;
	uint8_t  size_;
	uint16_t index;
	
	uint8_t size() const  { return (int8_t) size_ < 0 ? size_ & 0x3F : size_; }
	
	uint16_t word() const  { return int16_t( int8_t( index ) ); }
};

static inline
bool operator<( const global& g, int16_t addr )
{
	return g.addr + g.size() <= addr;
}

enum { ROM = -0x8000 };

static const global globals[] =
{
	{ ROM + 0x0008, 0x82, 0x00 },  // ROM version = 0x0000
	
	{ 0x0102, 0x84, 72              },  // ScrVRes, ScrHRes
	{ 0x0106, 2,    tag_ScreenRow   },
	{ 0x0108, 4,    tag_MemTop      },
	{ 0x0114, 4,    tag_HeapEnd     },
	{ 0x0118, 4,    tag_TheZone     },
	{ 0x011C, 4,    tag_UTableBase  },
	{ 0x012E, 2,    tag_MMOk_CPUFlag},  // MMInOk, CPUFlag
	{ 0x0130, 4,    tag_ApplLimit   },
	{ 0x0144, 2,    tag_SysEvtMask  },
	{ 0x0146, 4,    tag_SysEvtBuf   },
	{ 0x014A, 10,   tag_EventQueue  },
	{ 0x0154, 2,    tag_EvtBufCnt   },
	{ 0x015A, 2,    tag_SysVersion  },
	{ 0x0160, 10,   tag_VBLQueue    },
	{ 0x016A, 0xC4, tag_Ticks       },
	{ 0x0172, 2,    tag_MBState_esc },  // MBState, Tocks (Button escapes)
	{ 0x0174, 16,   tag_KeyMaps     },  // KeyMap, KeyPadMap, 4 more bytes
	{ 0x01D2, 2,    tag_UnitNtryCnt },
	{ 0x01D4, 4,    tag_VIA         },
	{ 0x01FA, 2,    tag_SPATBConfig },  // SPATalkB, SPConfig
	{ 0x0207, 2,    tag_SPKbdPrint  },  // SPKbd, SPPrint
	{ 0x020C, 0xC4, tag_Time        },
	{ 0x0210, 2,    tag_BootDrive   },
	{ 0x0214, 2,    tag_SFSaveDisk  },
	{ 0x021E, 2,    tag_Kbd_Alarm   },  // KbdType, AlarmState
	{ 0x0220, 2,    tag_MemErr      },
	{ 0x0260, 2,    tag_SdVolEnable },  // SdVolume, SdEnable
	{ 0x0266, 4,    tag_SoundBase   },
	{ 0x027E, 2,    tag_SoundActLvl },  // SoundActive, SoundLevel
	{ 0x0280, 2,    tag_CurPitch    },
	{ 0x028E, 2,    tag_ROM85       },
	{ 0x0290, 2,    tag_PortABUse   },  // PortAUse, PortBUse
	{ 0x02A6, 4,    tag_SysZone     },
	{ 0x02AA, 4,    tag_ApplZone    },
	{ 0x02AE, 4,    tag_ROMBase     },
	{ 0x02F0, 0x82, 0               },  // DoubleTime (high word)
	{ 0x02F2, 0x82, 15              },  // DoubleTime (low word)
	{ 0x02F4, 4,    tag_CaretTime   },
	{ 0x02F8, 2,    tag_ScrDmpEnbTyp},  // ScrDmpEnb, ScrDmpType
	{ 0x0308, 10,   tag_DrvQHdr     },
	{ 0x0316, 4,    tag_HpChk       },
	{ 0x031A, 0x83, 0xFF            },  // Lo3Bytes
	{ 0x0322, 4,    tag_DefltStack  },
	{ 0x033C, 4,    tag_IAZNotify   },
	{ 0x034E, 4,    tag_FCBSPtr     },
	{ 0x0352, 4,    tag_DefVCBPtr   },
	{ 0x0356, 10,   tag_VCBQHdr     },
	{ 0x03F6, 2,    tag_FSFCBLen    },
	{ 0x0800, 4,    tag_JHideCursor },
	{ 0x0804, 4,    tag_JShowCursor },
	{ 0x0814, 4,    tag_JInitCrsr   },
	{ 0x0818, 4,    tag_JSetCrsr    },
	{ 0x0824, 4,    tag_ScrnBase    },
	{ 0x0830, 4,    tag_Mouse       },
	{ 0x0834, 8,    tag_CrsrPin     },
	{ 0x083C, 8,    tag_CrsrRect    },
	{ 0x0844, 68,   tag_TheCrsr     },
	{ 0x08CC, 2,    tag_CrsrVisBusy },  // CrsrVis, CrsrBusy
	{ 0x08FC, 4,    tag_JIODone     },
	{ 0x0900, 2,    tag_CurApRefNum },
	{ 0x0904, 4,    tag_CurrentA5   },
	{ 0x0908, 4,    tag_CurStackBase},
	{ 0x0910, 32,   tag_CurApName   },
	{ 0x0930, 4,    tag_SaveSegHandle},
	{ 0x0934, 2,    tag_CurJTOffset },
	{ 0x0936, 2,    tag_CurPageOption},
	{ 0x0944, 2,    tag_PrintErr    },
	{ 0x0980, 4,    tag_ROMFont0    },
	{ 0x0984, 0x82, 3               },  // ApFontID (Geneva)
	{ 0x09CE, 8,    tag_ToolScratch },
	{ 0x09D6, 4,    tag_WindowList  },
	{ 0x09DA, 2,    tag_SaveUpdate  },
	{ 0x09DC, 2,    tag_PaintWhite  },
	{ 0x09DE, 4,    tag_WMgrPort    },
	{ 0x09E6, 4,    tag_OldStructure},
	{ 0x09EA, 4,    tag_OldContent  },
	{ 0x09EE, 4,    tag_GrayRgn     },
	{ 0x09F2, 4,    tag_SaveVisRgn  },
	{ 0x0A02, 0x84, 0x01            },  // OneOne
	{ 0x0A06, 0x84, 0xFF            },  // MinusOne
	{ 0x0A0E, 14,   tag_IconBitmap  },
	
	{ 0x0A26, 2,    tag_TheMenu     },
	
	{ 0x0A34, 8,    tag_DragPattern },
	{ 0x0A3C, 8,    tag_DeskPattern },
	{ 0x0A4A, 6,    tag_FPState     },
	{ 0x0A50, 4,    tag_TopMapHndl  },
	{ 0x0A58, 2,    tag_SysMap      },
	{ 0x0A5A, 2,    tag_CurMap      },
	{ 0x0A5E, 2,    tag_ResLoad     },
	{ 0x0A60, 2,    tag_ResErr      },
	{ 0x0A64, 4,    tag_CurActivate },
	{ 0x0A68, 4,    tag_CurDeactive },
	{ 0x0A78, 12,   tag_ApplScratch },
	{ 0x0A9A, 2,    tag_ACount      },
	{ 0x0AA0, 16,   tag_DAStrings   },
	{ 0x0AB0, 2,    tag_TEScrpLength},
	{ 0x0AB4, 4,    tag_TEScrpHandle},
	{ 0x0AB8, 32,   tag_AppPacks    },
	{ 0x0AE8, 4,    tag_AE8         },
	{ 0x0AEC, 4,    tag_AppParmHandle},
	{ 0x0AFA, 2,    tag_DlgFont     },
	{ 0x0B22, 2,    tag_SCSIFlag    },
	{ 0x0BAA, 2,    tag_MBarHeight  },
	{ 0x0BFE, 2,    tag_last_A_trap }
};

static
const global* find( const global* begin, const global* end, int16_t address )
{
	while ( begin < end )
	{
		if ( ! (*begin < address) )
		{
			break;
		}
		
		++begin;
	}
	
	return begin;
}

static const global* find_global( int16_t address )
{
	const global* begin = globals;
	const global* end   = globals + sizeof globals / sizeof globals[0];
	
	const global* it = find( begin, end, address );
	
	if ( it == end )
	{
		return NULL;
	}
	
	if ( address < it->addr )
	{
		return NULL;
	}
	
	return it;
}

static void refresh_dynamic_global( uint8_t tag )
{
	uint16_t* address = &words[ tag ];
	
	uint32_t longword;
	
	switch ( tag )
	{
		case tag_Ticks:
			/*
				We're coopting Tocks to be the number of Button() calls that
				can be made without blocking before reading Ticks again.
				The 1987 rewrite of Steve Capps' Clock only needs one, but the
				1984 original makes two consecutive calls.
			*/
			
			((uint8_t*) &words[ tag_MBState_esc ])[ 1 ] = 2;  // big-endian
			
			ticking = true;
			
			longword = get_Ticks();
			
			*(uint32_t*) address = big_longword( longword );
			
			break;
		
		case tag_Time:
			longword = get_Time();
			
			*(uint32_t*) address = big_longword( longword );
			
			break;
		
		default:
			break;
	}
}

static uint8_t buffer[ 68 ];  // needs to be as big as the largest global

static uint8_t* read_globals( const global* g, int16_t addr, uint32_t size )
{
	// size == 1 -> offset = 0
	// size == 2 -> offset = addr & 1
	// size == 4 -> offset = addr & 3
	
	const int offset = addr & size - 1;
	
	int i = offset - (addr - g->addr);
	
	const uint32_t end = addr + size;
	
	for ( ;; )
	{
		uint32_t width = g->size();
		
		if ( int8_t( g->size_ ) >= 0  ||  g->size_ >= 0xC0 )
		{
			if ( g->size_ >= 0xC0 )
			{
				refresh_dynamic_global( g->index );
			}
			
			return (uint8_t*) &words[ g->index ] + (addr - g->addr);
		}
		else
		{
			if ( width < 2 )
			{
				buffer[ i++ ] = g->index;
			}
			else
			{
				const uint16_t word = g->word();
				
				buffer[ i++ ] = word >> 8;
				buffer[ i++ ] = word;
				
				if ( width > 2 )
				{
					buffer[ i++ ] = word >> 8;
					buffer[ i++ ] = word;
					
					if ( width == 3 )
					{
						buffer[ i - 4 ] = 0;
						
						width = 4;
					}
				}
			}
		}
		
		addr += width;
		
		if ( addr >= end )
		{
			return buffer + offset;
		}
		
		if ( ++g == (globals + sizeof globals / sizeof globals[0]) )
		{
			return NULL;
		}
		
		if ( g->addr != addr )
		{
			return NULL;
		}
	}
	
	return buffer + offset;
}

static uint8_t* write_globals( const global* g, int16_t addr, uint32_t size )
{
	const uint16_t offset = addr - g->addr;
	
	if ( offset + size <= g->size_ )
	{
		return buffer + offset;
	}
	
	return NULL;
}

static uint8_t* update_globals( const global* g, int16_t addr, uint32_t size )
{
	const uint16_t offset = addr - g->addr;
	
	memcpy( (char*) &words[ g->index ] + offset, buffer + offset, size );
	
	return buffer;
}

uint8_t* translate( addr_t addr, uint32_t length, fc_t fc, mem_t access )
{
	if ( access == mem_exec )
	{
		// None of Mac low memory is executable
		
		return 0;  // NULL
	}
	
	if ( (addr >> 16) == 0x0040 )
	{
		addr -= 0x8000;
	}
	
	if ( const global* g = find_global( addr ) )
	{
		if ( access == mem_read )
		{
			return read_globals( g, addr, length );
		}
		else if ( access == mem_write )
		{
			return write_globals( g, addr, length );
		}
		else  // mem_update
		{
			return update_globals( g, addr, length );
		}
	}
	
	return 0;
}

}  // namespace mac
}  // namespace v68k
