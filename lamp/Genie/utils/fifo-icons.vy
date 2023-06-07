module fifo-icons

const vacuum = x"48"  # cyan
const air    = x"2a"  # light grey-blue
const stuff  = x"e3"  # green
const error  = x"d8"  # red
const shade  = x"f9"  # grey
const white  = x"00"  # self-
const black  = x"FF"  # explanatory

const blank_row = white * 32

def kv (f) { {f _.key => f _.value} }

def frame (src_width, dst_width, dst_height, x, y, src)
{
	const src_height = src.size / src_width
	
	const left  = (white * x)
	const right = (white * (dst_width - src_width - x))
	
	const top    = white * (dst_width * y)
	const bottom = white * (dst_width * (dst_height - src_height - y))
	
	const rows = [src / -src_width] map { left v right }
	
	return top (packed rows) bottom
}

def LSR_L (icon)
{
	const width = 32
	
	return packed ([icon / -width] map { white v[ 0 -> width - 1] }) 
}

def calc_mask (width, icon)
{
	const N = icon.size
	
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
	
	const outer_width = width
	const inner_width = width - 2
	
	const outer_height = N / width
	const inner_height = outer_height - 2
	
	const cap = black * outer_width
	const mid = white * inner_width
	
	const middle = black mid black * inner_height
	
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
	const clock_data =
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
	
	const clock = packed translated (clock_data, " X", x"00FF")
	
	return place clock => place calc_mask (7, clock)
}

const any_clock = make_clock()
const low_clock = (kv { (blank_row * 19) _ (blank_row *  6) }) any_clock
const high_time = (kv { (blank_row *  6) _ (blank_row * 19) }) any_clock

def make_porthole
{
	const edge = x"00FFFFFF00"
	const hole = x"FF050505FF"
	
	def place (image) { frame (5, 32, 32, 18, 21, image) }
	
	const porthole = edge (hole * 3) edge
	
	return place porthole => place translated (porthole, x"05", black)
}

def make_blank_guage
{
	const edge = x"00FFFFFF00"
	const tube = x"FF000000FF"
	
	def place (image) { frame (5, 32, 32, 18, 6, image) }
	
	const guage = edge (tube * 18) edge
	
	return place guage => place calc_mask (5, guage)
}

const blank_guage = make_blank_guage()

def make_pipe
{
	const edge = (white * 13) x"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" (white * 4)
	const Head = (white * 13) x"FFf8f72bf6f5f62bf7f8f9fafbfcFF" (white * 4)
	const tube = (white * 15)     x"FFf72bf6f5f62bf7f8f9FF"     (white * 6)
	
	const cap = edge Head Head edge
	
	const pipe = cap (tube * 24) cap
	
	return pipe => translated (pipe, x"2be3f5f6f7f8f9fafbfc", black * 10)
}

def make_fat_pipe
{
	const sections =
	[
		6: blank_row,
		1: (white * 14)     x"FFf72bf6f5FFFFFF2bf7f8f9FF"     (white * 5),
		2: (white * 14)     x"FFf72bf6FFe3e3e3FFf7f8f9FF"     (white * 5),
		2: (white * 13)   x"FFf72bf6f5FFe3e3e3FF2bf7f8f9FF"   (white * 4),
		2: (white * 13)   x"FFf72bf6FFe3e3e3e3e3FFf7f8f9FF"   (white * 4),
		3: (white * 12) x"FFf72bf6f5FFe3e3e3e3e3FF2bf7f8f9FF" (white * 3),
	]
	
	const upper = sections map { v.value * v.key }
	const lower = -upper
	
	const fat_pipe = packed (upper, lower)
	
	return fat_pipe => translated (fat_pipe, x"2be3f5f6f7f8f9", black * 7)
}

def solid_guage (color)
{
	const left  = white * 19
	const right = white * 10
	
	const fill = left (color * 3) right
	
	const blank = blank_row * 7
	
	return blank_guage.key | blank (fill * 18) blank => blank_guage.value
}

def mixed_guage (top, level, bottom)
{
	const left  = white * 19
	const right = white * 10
	
	const upper = left (top    * 3) right
	const bound = left (black  * 3) right
	const lower = left (bottom * 3) right
	
	const stuff = lower * level
	const empty = upper * (17 - level)
	const blank = blank_row * 7
	
	return blank_guage.key | blank empty bound stuff blank => blank_guage.value
}

def broken (pipe)
{
	const invariant = (white * 4) (black * 23)
	
	const mask = 0 -> 32 map { (black * (36 - v))(invariant)[ 0 -> 32 ] }
	
	return (kv {_ & packed mask}) pipe
}

def compose_masked (dst, src)
{
	return dst.key & ~src.value | src.key => dst.value | src.value
}

export const basic_icon = make_pipe()

const pipify = compose_masked % basic_icon

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
	const cube_data =
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
	
	const open_cube   = cube_data map { translated (v, "* ", "+\0") }
	const closed_cube = cube_data map { translated (v, "* ", "X\0") }
	
	const top   = packed (closed_cube map { ("\0" * 4) v ("\0" * 4) })
	const left  = packed (open_cube   map {            v ("\0" * 8) })
	const right = packed (closed_cube map { ("\0" * 8) v            })
	
	const upper =                         top       (white * (16 * 6))
	const lower = (white * (16 * 6)) (left | right)
	
	return frame (16, 32, 32, 1, 8, upper | lower)
}

def make_1_from_8 (icon)
{
	const light = x"00052a2b48f5f6f7f8f9fafbfc"
	const white = "0" * light.size
	const paint = x"d8e3FF"
	const black = "1" * paint.size
	
	return unbin translated (icon.string, paint light, black white)
}

def make_4_from_8 (icon)
{
	const light = x"0048"
	const white = "0" * light.size
	const dull = x"2a2bf5f6f7f8f9fafbfc"
	const gray = "c" * dull.size
	const other = x"05d8e3FF"
	const color = "138f"
	
	return unhex translated (icon.string, light dull other, white gray color)
}

const simple_badge_spec = make_simple_badge()
const simple_badge_mask = translated (simple_badge_spec, ".-^+X\xf9", black * 6)
const shadow_badge_mask = simple_badge_mask | LSR_L simple_badge_mask
const badge_shadow_mask = ~simple_badge_mask & shadow_badge_mask
const badge_shadow      = translated (badge_shadow_mask, black, shade)
const shadow_badge_spec = simple_badge_spec | badge_shadow

const badge_spec = shadow_badge_spec.string

const badge_data_8 = packed translated (badge_spec, ".-^+X",   x"2a7f7fb0FF")
const badge_data_4 = unhex  translated (badge_spec, ".-^+X\xf9\0", "cddefd0")
const badge_data_1 = unbin  translated (badge_spec, ".-^+X\xf9\0", "0011100")

const shadow_badge_mask_8 = shadow_badge_mask
const shadow_badge_mask_4 = make_4_from_8 shadow_badge_mask
const shadow_badge_mask_1 = make_1_from_8 shadow_badge_mask
const simple_badge_mask_1 = make_1_from_8 simple_badge_mask

def badged_icl8 (data) { data & ~shadow_badge_mask_8 | badge_data_8 }
def badged_icl4 (data) { data & ~shadow_badge_mask_4 | badge_data_4 }

def badged_ICN_ (icon)
{
	var data, var mask = icon / 2
	
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
	return badged_ICN_ make_1_from_8 packed (icl8.key, icl8.value)
}

const handlers = str^
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
