module sndpipe

let basic = x"0101"
let admin = x"4a4a"
let sound = x"4b4b"

let switch_on = x"0000"
let allow_eof = x"2e2e"
let full_stop = x"5858"

let squarewave = x"ffff"
let freeform   = x"0000"
let fourtone_b = x"0101"
let fourtone_u = x"0201"

let big_u16 = (big-endian u16).encode
let big_u32 = (big-endian u32).encode

def headerize (domain, opcode, extent=x"")
{
	let reserved = x"0000"
	
	return reserved (big_u16 extent.size) domain opcode extent
}

let startup   = headerize (basic, switch_on)
let carry_on  = headerize (basic, allow_eof)
let interrupt = headerize (basic, full_stop)

def triplet (wavelength-amplitude-duration)
{
	return wavelength-amplitude-duration map big_u16 per packed
}

def square_wave_tones (tones)
{
	let triplets = (tones map triplet per packed) triplet [0, 0, 0]
	
	return headerize (sound, squarewave, triplets)
}

def free_form_sound (playrate, samples)
{
	return headerize (sound, freeform, packed (big_u32 playrate, samples))
}

def four_tone_start (duration, notes)
{
	let recID = x"00000000"
	
	let n0 = 4 - notes.length
	
	let meta = notes map {big_u32(v.key) big_u32 v.value.value}
	let data = notes map {v.value.key}
	
	let mgap = x"00" * 8 * n0
	let dgap = x"00" * 256 * n0
	
	let sndrec = packed (recID, big_u16 duration, meta, mgap, data, dgap)
	
	return headerize (sound, fourtone_b, sndrec)
}

def four_tone_update (duration, notes)
{
	let recID = x"00000000"
	
	let n0 = 4 - notes.length
	
	let meta = notes map {big_u32(v.key) big_u32 v.value.value}
	
	let mgap = x"00" * 8 * n0
	
	let sndrec = packed (recID, big_u16 duration, meta, mgap)
	
	return headerize (admin, fourtone_u, sndrec)
}

export
def player (send, recv=null)
{
	send startup
	
	let stop = send % interrupt
	let detach = send % carry_on
	
	let square_wave = send * square_wave_tones
	let free_form   = send * free_form_sound
	let four_tone   = send * four_tone_start
	let tone_update = send * four_tone_update
	
	let basic = :stop, :detach
	
	return record( basic, :square_wave, :free_form, :four_tone, :tone_update )
}
