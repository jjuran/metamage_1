module asm68k

def NOP { x"4E71" }
def RTS { x"4E75" }

let word = (big-endian u16).encode

def PEA (smode, sreg, sext)
{
	return x"4870" | word (smode * 2^3 + sreg), *(sext map word)
}

def TRAP (v)
{
	assert v in 0 .. 15
	
	return x"4E40" | word v
}

def LINK (An, disp)
{
	assert An in 0 .. 7
	
	return x"4E50" | word An, word disp
}

def UNLK (An)
{
	assert An in 0 .. 7
	
	return x"4E58" | word An
}

def Bcc (cc, offset)
{
	let disp = word offset
	
	let base = x"6000" | (packed byte cc) x"00"
	
	return if disp[ 0 ] or offset == 0 then { base, disp } else { base | disp }
}

def MOVEQ (immed, Dn)
{
	assert Dn in 0 .. 7
	
	assert immed in -128 .. 127
	
	return x"7000" | word u8 immed | word (0x200 * Dn)
}

def parse_int (s)
{
	return if substr( s, 0, 2 ) == "0x"
		then {unhex s}
		else {int   s}
}

let digit = '0' .. '9'
let xdigit = digit | 'a' .. 'f' | 'A' .. 'F'

let non_neg = "0" | ['1' .. '9', digit*]
let decimal = [ '-'?, non_neg ]
let hexadec = ["0x", xdigit+]
let numeral = (hexadec | decimal) => parse_int

let lwb = '.', ('L' => {2})
             | ('W' => {1})
             | ('B' => {0})

let lw = '.', ('L' => {1})
            | ('W' => {0})

let gr_range = '0' .. '7'

let Dn = 'D', gr_range => int
let An = ['A', gr_range => int] | ("SP" => {7})

let Xn = [Dn] | (An => {8 + _})
let PC = "PC"

let indirect = '(', An, ')'

let postinc = '(', An, ")+"
let predec  = "-(", An, ')'

let displacement = [numeral, "("] | ["(", numeral, ","]
let displacement_0 = displacement | ["(" => {0}]

let scales = 0 .. 3 map { byte (48 + 2^v) => {v} } per {a | b}
let scale = ["*", scales]? => {_ or 0}

let displace = displacement,   An, ")"
let indexed  = displacement_0, An, ",", Xn, lw, scale, ")"

def less2 (x) { int x - 2 }

let PCr-star = "*", [ "+", non_neg => less2 ] | ([ "-", non_neg ] => less2)
let PCr-disp = displacement,   PC, ")"
let PCr-indx = displacement_0, PC, ",", Xn, lw, scale, ")"

let absolute = hexadec => {+(i16 _ != _), _} * unhex
let immed = '#', numeral

def split_long (x)
{
	return u32 x div 2^16, u16 x
}

def sized_ea (size_code, mode, reg, exts)
{
	return
		if size_code == 2 and mode == 7 and reg == 4
			then { size_code, mode, reg, [split_long *exts] }
			else { _ }
}

let ea =
	([ Dn       ] => {0, _, []}) |
	([ An       ] => {1, _, []}) |
	([ indirect ] => {2, _, []}) |
	([ postinc  ] => {3, _, []}) |
	([ predec   ] => {4, _, []}) |
	([ displace ] => {5, tail _, [head _]}) |
	([ indexed  ] => lambda (d, A, R, z, x)
	                 {6, A, [R * 2^12 + z * 2^11 + x * 2^9 + u8 d]}) |
	([ absolute ] => lambda (z, addr)
	                 {7, z, [split_long^z addr]}) |
	([ PCr-star ] => {7, 2, [_]}) |
	([ PCr-disp ] => {7, 2, [_]}) |
	([ PCr-indx ] => lambda (d, R, z, x)
	                 {7, 3, [R * 2^12 + z * 2^11 + x * 2^9 + u8 d]}) |
	([ immed    ] => {7, 4, [_]}) |
	
	byteclass()

let whitespace = ' ' | '\t'

let space = whitespace+

let line_comment = ["//", ~byteclass('\n')*]

let separator = [line_comment?, '\n'] | ';'

let dc_w = [ "DC.W", space, numeral ] => word
let dc_l = [ "DC.L", space, numeral ] => {word (u32 _ div 2^16), word u16 _}

let pea = [ "PEA", space, ea ] => PEA

let trap = [ "TRAP", space, immed ] => TRAP

let link = [ "LINK", space, An, ',', immed ] => LINK
let unlk = [ "UNLK", space, An             ] => UNLK

let bsr = [ "BSR.S" => {1}, space, PCr-star ] => Bcc

let moveq = [ "MOVEQ", ".L"?, space, immed, ',', Dn ] => MOVEQ

let instruction = dc_w
                | dc_l
                | pea
                | trap
                | link
                | unlk
                | :NOP
                | :RTS
                | bsr
                | moveq

let instructions = [ whitespace*, instruction?, whitespace*, separator ]*

export let grammar = instructions
