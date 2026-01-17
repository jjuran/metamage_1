/*
	sleep.hh
	--------
*/

static const unsigned short shipped_sleep_loop[] =
{
	0x4e56, -4,  // 0000dc:  LINK     A6,#-4
	0x2f07,      // 0000e0:  MOVE.L   D7,-(A7)
	0x42a7,      // 0000e2:  CLR.L    -(A7)
	0xa975,      // 0000e4:  _TickCount
	0x2e1f,      // 0000e6:  MOVE.L   (A7)+,D7
	0x302e,  8,  // 0000e8:  MOVE.W   (8,A6),D0
	0x48c0,      // 0000ec:  EXT.L    D0
	0xde80,      // 0000ee:  ADD.L    D0,D7
	0x42a7,      // 0000f0:  CLR.L    -(A7)
	0xa975,      // 0000f2:  _TickCount
	0xbe9f,      // 0000f4:  CMP.L    (A7)+,D7
	0x6f02,      // 0000f6:  BLE.S    *+4       // $0000fa
	0x60f6,      // 0000f8:  BRA.S    *-8       // $0000f0
	0x2e1f,      // 0000fa:  MOVE.L   (A7)+,D7
	0x4e5e,      // 0000fc:  UNLK     A6
	0x205f,      // 0000fe:  MOVEA.L  (A7)+,A0
	0x544f,      // 000100:  ADDQ.W   #2,A7
	0x4ed0,      // 000102:  JMP      (A0)
};

static const unsigned short patched_sleep_trap[] =
{
	0x225f,  // 0000dc:  MOVEA.L  (A7)+,A1
	0x305f,  // 0000de:  MOVEA.W  (A7)+,A0
	0xa03b,  // 0000e0:  _Delay
	0x4ed1,  // 0000e2:  JMP      (A1)
	0x4e71,  // 0000e4:  NOP
	0x4e71,  // 0000e6:  NOP
	0x4e71,  // 0000e8:  NOP
	0x4e71,  // 0000ea:  NOP
	0x4e71,  // 0000ec:  NOP
	0x4e71,  // 0000ee:  NOP
	0x4e71,  // 0000f0:  NOP
	0x4e71,  // 0000f2:  NOP
	0x4e71,  // 0000f4:  NOP
	0x4e71,  // 0000f6:  NOP
	0x4e71,  // 0000f8:  NOP
	0x4e71,  // 0000fa:  NOP
	0x4e71,  // 0000fc:  NOP
	0x4e71,  // 0000fe:  NOP
	0x4e71,  // 000100:  NOP
	0x4e71,  // 000102:  NOP
};
