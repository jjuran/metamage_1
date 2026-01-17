/*
	dissolve_bytes.hh
	-----------------
*/

static const unsigned short shipped_dissolve_bytes_A[] =
{
	0x4e56,      0,  // 002566:  LINK     A6,#0
	0x48e7, 0x1020,  // 00256a:  MOVEM.L  D3/A2,-(A7)
};

static const unsigned short shipped_dissolve_bytes_B[] =
{
	0x45fa, 0x002a,          // 0025c4:  LEA      *+44,A2    ; $0025f0
	0x2232, 0x0000,          // 0025c8:  MOVE.L   (A2,D0.W),D1
	0x2401,                  // 0025cc:  MOVE.L   D1,D2
	0x2050,                  // 0025ce:  MOVEA.L  (A0),A0     // srcBase
	0x2251,                  // 0025d0:  MOVEA.L  (A1),A1     // dstBase
	0xa852,                  // 0025d2:  _HideCursor
	0x6002,                  // 0025d4:  BRA.S    *+4    ; $0025d8
	0xb342,                  // 0025d6:  EOR.W    D1,D2
	0xb642,                  // 0025d8:  CMP.W    D2,D3
	0x6f06,                  // 0025da:  BLE.S    *+8    ; $0025e2
	0x13b0, 0x2000, 0x2000,  // 0025dc:  MOVE.B   (A0,D2.W),(A1,D2.W)
	0xe24a,                  // 0025e2:  LSR.W    #1,D2
	0x62f6,                  // 0025e4:  BHI.S    *-8    ; $0025dc
	0x66ee,                  // 0025e6:  BNE.S    *-16    ; $0025d6
	0x1290,                  // 0025e8:  MOVE.B   (A0),(A1)
	0xa853,                  // 0025ea:  _ShowCursor
	0x4cdf, 0x0408,          // 0025ec:  MOVEM.L  (A7)+,D3/A2
	0x4e5e,                  // 0025f0:  UNLK     A6
};

static const unsigned short patched_dissolve_bytes_A[] =
{
	0x4e56,      0,  // 002566:  LINK     A6,#0
	0x48e7, 0x1c30,  // 00256a:  MOVEM.L  D3-D5/A2-A3,-(A7)
};

static const unsigned short patched_dissolve_bytes_B[] =
{
	0x45fa, 0x002a,          // 0025c4:  LEA      *+44,A2    ; $0025f0
	0x2832, 0x0000,          // 0025c8:  MOVE.L   (A2,D0.W),D4
	0x2a04,                  // 0025cc:  MOVE.L   D4,D5
	0x2450,                  // 0025ce:  MOVEA.L  (A0),A2     // srcBase
	0x2651,                  // 0025d0:  MOVEA.L  (A1),A3     // dstBase
	0xa852,                  // 0025d2:  _HideCursor
	0x6002,                  // 0025d4:  BRA.S    *+4    ; $0025d8
	0xb945,                  // 0025d6:  EOR.W    D4,D5
	0xb645,                  // 0025d8:  CMP.W    D5,D3
	0x6f06,                  // 0025da:  BLE.S    *+8    ; $0025e2
	0x17b2, 0x5000, 0x5000,  // 0025dc:  MOVE.B   (A2,D5.W),(A3,D5.W)
	0xe24d,                  // 0025e2:  LSR.W    #1,D5
	0x62f6,                  // 0025e4:  BHI.S    *-8    ; $0025dc
	0x66ee,                  // 0025e6:  BNE.S    *-16    ; $0025d6
	0x1692,                  // 0025e8:  MOVE.B   (A2),(A3)
	0xa853,                  // 0025ea:  _ShowCursor
	0x4cdf, 0x0c38,          // 0025ec:  MOVEM.L  (A7)+,D3-D5/A2-A3
	0x4e5e,                  // 0025f0:  UNLK     A6
};
