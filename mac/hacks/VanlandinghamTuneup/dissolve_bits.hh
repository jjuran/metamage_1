/*
	dissolve_bits.hh
	----------------
*/

static const unsigned short shipped_dissolve_bits[] =
{
	0x0f13,          // 002886:  BTST     D7,(A3)
	0x2644,          // 002888:  MOVEA.L  D4,A3
	0x6610,          // 00288a:  BNE.S    *+18     ; $00289c
	
	0x0d93,          // 00288c:  BCLR     D6,(A3)
	
	0xe288,          // 00288e:  LSR.L    #1,D0
	0x62c6,          // 002890:  BHI.S    *-56     ; $002858
	0xb780,          // 002892:  EOR.L    D3,D0
	0xb083,          // 002894:  CMP.L    D3,D0
	0x66bc,          // 002896:  BNE.S    *-66     ; $002854
	0x6000, 0xff36,  // 002898:  BRA      *-200    ; $0027d0
	
	0x0dd3,          // 00289c:  BSET     D6,(A3)
	
	0xe288,          // 00289e:  LSR.L    #1,D0
	0x62b6,          // 0028a0:  BHI.S    *-72     ; $002858
	0xb780,          // 0028a2:  EOR.L    D3,D0
	0xb083,          // 0028a4:  CMP.L    D3,D0
	0x66ac,          // 0028a6:  BNE.S    *-82     ; $002854
	0x6000, 0xff26,  // 0028a8:  BRA      *-216    ; $0027d0
};

static const unsigned short patched_dissolve_bits[] =
{
	0x0f13,          // 002886:  BTST     D7,(A3)
	0x2644,          // 002888:  MOVEA.L  D4,A3
	0x660a,          // 00288a:  BNE.S    *+12     ; $002896
	
	0x0d93,          // 00288c:  BCLR     D6,(A3)
	
	0x6008,          // 00288e:  BRA.S    *+10     ; $002898
	0x4e71,          // 002890:  NOP
	0x4e71,          // 002892:  NOP
	0x4e71,          // 002894:  NOP
	0x0dd3,          // 002896:  BSET     D6,(A3)
	0x4eb9,          // 002898:  JSR      DissolveBits_one_byte
	/*
	______, ______,  // 00289a:  <JSR target, set at run-time>
	
	0xe288,          // 00289e:  LSR.L    #1,D0
	0x62b6,          // 0028a0:  BHI.S    *-72     ; $002858
	0xb780,          // 0028a2:  EOR.L    D3,D0
	0xb083,          // 0028a4:  CMP.L    D3,D0
	0x66ac,          // 0028a6:  BNE.S    *-82     ; $002854
	0x6000, 0xff26,  // 0028a8:  BRA      *-216    ; $0027d0
	*/
};
