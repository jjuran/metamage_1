module fifo-icons

let vacuum = x"48"  # cyan
let air    = x"2a"  # light grey-blue
let stuff  = x"e3"  # green
let error  = x"d8"  # red
let shade  = x"f9"  # grey
let white  = x"00"  # self-
let black  = x"FF"  # explanatory

let blank_row = white * 32

def kv (f) { {f _.key => f _.value} }

def frame (src_width, dst_width, dst_height, x, y, src)
{
	let src_height = src.size / src_width
	
	let left  = (white * x)
	let right = (white * (dst_width - src_width - x))
	
	let top    = white * (dst_width * y)
	let bottom = white * (dst_width * (dst_height - src_height - y))
	
	let rows = [src / -src_width] map { left v right }
	
	return top (packed rows) bottom
}

def LSR_L (icon)
{
	let width = 32
	
	return packed ([icon / -width] map { white v[ 0 -> width - 1] }) 
}

def calc_mask (width, icon)
{
	let N = icon.size
	
	def neighbors (i)
	{
		return
		[
			(i - width) (*) (i - width >= 0),
			(i + width) (*) (i + width <  N),
			(i - 1) (*) ( i      % width != 0),
			(i + 1) (*) ((i + 1) % width != 0),
		]
	}
	
	var mask = black * N
	
	let outer_width = width
	let inner_width = width - 2
	
	let outer_height = N / width
	let inner_height = outer_height - 2
	
	let cap = black * outer_width
	let mid = white * inner_width
	
	let middle = black mid black * inner_height
	
	var frontier = cap middle cap
	
	while Iter.popcount frontier do
	{
		for i in (0 -> N ver { frontier[ v ] }) do
		{
			if not icon[ i ] and mask[ i ] then
			{
				for j in neighbors i do
				{
					frontier[ j ] = '\xFF'
				}
				
				mask[ i ] = '\0'
			}
			
			frontier[ i ] = '\0'
		}
	}
	
	return mask
}

def make_clock
{
	let clock_data =
	[
		"    X X X    ",
		"  X       X  ",
		"X     X     X",
		"X     X X   X",
		"X           X",
		"  X       X  ",
		"    X X X    ",
	]
	map { mince v } per string
	
	def place (image) { frame (7, 32, 7, 17, 0, image) }
	
	let clock = packed translated (clock_data, " X", x"00FF")
	
	return place clock => place calc_mask (7, clock)
}

let any_clock = make_clock()
let low_clock = (kv { (blank_row * 19) _ (blank_row *  6) }) any_clock
let high_time = (kv { (blank_row *  6) _ (blank_row * 19) }) any_clock

def make_porthole
{
	let edge = x"00FFFFFF00"
	let hole = x"FF050505FF"
	
	def place (image) { frame (5, 32, 32, 18, 21, image) }
	
	let porthole = edge (hole * 3) edge
	
	return place porthole => place translated (porthole, x"05", black)
}

def make_blank_guage
{
	let edge = x"00FFFFFF00"
	let tube = x"FF000000FF"
	
	def place (image) { frame (5, 32, 32, 18, 6, image) }
	
	let guage = edge (tube * 18) edge
	
	return place guage => place calc_mask (5, guage)
}

let blank_guage = make_blank_guage()

def make_pipe
{
	let edge = (white * 13) x"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" (white * 4)
	let Head = (white * 13) x"FFf8f72bf6f5f62bf7f8f9fafbfcFF" (white * 4)
	let tube = (white * 15)     x"FFf72bf6f5f62bf7f8f9FF"     (white * 6)
	
	let cap = edge Head Head edge
	
	let pipe = cap (tube * 24) cap
	
	return pipe => translated (pipe, x"2be3f5f6f7f8f9fafbfc", black * 10)
}

def make_fat_pipe
{
	let sections =
	[
		6: blank_row,
		1: (white * 14)     x"FFf72bf6f5FFFFFF2bf7f8f9FF"     (white * 5),
		2: (white * 14)     x"FFf72bf6FFe3e3e3FFf7f8f9FF"     (white * 5),
		2: (white * 13)   x"FFf72bf6f5FFe3e3e3FF2bf7f8f9FF"   (white * 4),
		2: (white * 13)   x"FFf72bf6FFe3e3e3e3e3FFf7f8f9FF"   (white * 4),
		3: (white * 12) x"FFf72bf6f5FFe3e3e3e3e3FF2bf7f8f9FF" (white * 3),
	]
	
	let upper = sections map { v.value * v.key }
	let lower = -upper
	
	let fat_pipe = packed (upper, lower)
	
	return fat_pipe => translated (fat_pipe, x"2be3f5f6f7f8f9", black * 7)
}

def solid_guage (color)
{
	let left  = white * 19
	let right = white * 10
	
	let fill = left (color * 3) right
	
	let blank = blank_row * 7
	
	return blank_guage.key | blank (fill * 18) blank => blank_guage.value
}

def mixed_guage (top, level, bottom)
{
	let left  = white * 19
	let right = white * 10
	
	let upper = left (top    * 3) right
	let bound = left (black  * 3) right
	let lower = left (bottom * 3) right
	
	let stuff = lower * level
	let empty = upper * (17 - level)
	let blank = blank_row * 7
	
	return blank_guage.key | blank empty bound stuff blank => blank_guage.value
}

def broken (pipe)
{
	let invariant = (white * 4) (black * 23)
	
	let mask = 0 -> 32 map { (black * (36 - v))(invariant)[ 0 -> 32 ] }
	
	return (kv {_ & packed mask}) pipe
}

def compose_masked (dst, src)
{
	return dst.key & ~src.value | src.key => dst.value | src.value
}

export const basic_icon = make_pipe()

let pipify = compose_masked % basic_icon

export const half_open_reading = pipify low_clock
export const half_open_writing = pipify high_time

export const writer_closed = pipify make_porthole()
export const pipe_underrun = pipify solid_guage vacuum
export const pipe_overrun  = pipify make_fat_pipe()

export const empty_guage = pipify solid_guage air
export const full_guage  = pipify solid_guage stuff

export const guage_levels = 1 .. 17 map { pipify mixed_guage (air, v, stuff) }

export const broken_pipe = broken pipify mixed_guage (error, 7, white)

def make_simple_badge
{
	let cube_data =
	[
		"      X X      ",
		"  X X . . X X  ",
		"X . . . . . . X",
		"X X X . . X X *",
		"X ^ - X X + + *",
		"X - ^ - X + + *",
		"X ^ - ^ X + + X",
		"  X X - X X X  ",
		"      X X      ",
	]
	map { mince v }
	
	let open_cube   = cube_data map { translated (v, "* ", "+\0") }
	let closed_cube = cube_data map { translated (v, "* ", "X\0") }
	
	let top   = packed (closed_cube map { ("\0" * 4) v ("\0" * 4) })
	let left  = packed (open_cube   map {            v ("\0" * 8) })
	let right = packed (closed_cube map { ("\0" * 8) v            })
	
	let upper =                         top       (white * (16 * 6))
	let lower = (white * (16 * 6)) (left | right)
	
	return frame (16, 32, 32, 1, 8, upper | lower)
}

def make_1_from_8 (icon)
{
	let light = x"00052a2b48f5f6f7f8f9fafbfc"
	let white = "0" * light.size
	let paint = x"d8e3FF"
	let black = "1" * paint.size
	
	return unbin translated (icon.string, paint light, black white)
}

def make_4_from_8 (icon)
{
	let light = x"0048"
	let white = "0" * light.size
	let dull = x"2a2bf5f6f7f8f9fafbfc"
	let gray = "c" * dull.size
	let other = x"05d8e3FF"
	let color = "138f"
	
	return unhex translated (icon.string, light dull other, white gray color)
}

let simple_badge_spec = make_simple_badge()
let simple_badge_mask = translated (simple_badge_spec, ".-^+X\xf9", black * 6)
let shadow_badge_mask = simple_badge_mask | LSR_L simple_badge_mask
let badge_shadow_mask = ~simple_badge_mask & shadow_badge_mask
let badge_shadow      = translated (badge_shadow_mask, black, shade)
let shadow_badge_spec = simple_badge_spec | badge_shadow

let badge_spec = shadow_badge_spec.string

let badge_data_8 = packed translated (badge_spec, ".-^+X",   x"2a7f7fb0FF")
let badge_data_4 = unhex  translated (badge_spec, ".-^+X\xf9\0", "cddefd0")
let badge_data_1 = unbin  translated (badge_spec, ".-^+X\xf9\0", "0011100")

let shadow_badge_mask_8 = shadow_badge_mask
let shadow_badge_mask_4 = make_4_from_8 shadow_badge_mask
let shadow_badge_mask_1 = make_1_from_8 shadow_badge_mask
let simple_badge_mask_1 = make_1_from_8 simple_badge_mask

def badged_icl8 (data) { data & ~shadow_badge_mask_8 | badge_data_8 }
def badged_icl4 (data) { data & ~shadow_badge_mask_4 | badge_data_4 }

def badged_ICN_ (icon)
{
	var (data, mask) = icon / 2
	
	# Use the simple (unshadowed) mask for the Bic operation, so we don't
	# clear pixels that would belong to the shadow (if we had one).
	
	# But use the shadowed badge mask for augmenting the icon mask,
	# so shadow pixels in colored icons don't get clipped out.
	
	data = data & ~simple_badge_mask_1 | badge_data_1
	mask = mask |  shadow_badge_mask_1
	
	return data mask
}

def make_ICN_ (icl8)
{
	return badged_ICN_ make_1_from_8 packed *icl8
}

let handlers = str^
[
	icl8: badged_icl8 * .key,
	icl4: badged_icl4 * make_4_from_8 * .key,
	"ICN#": make_ICN_,
]

export
def get (data, icon-type)
{
	return handlers[ icon-type ] data
}

end.
