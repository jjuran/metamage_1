def pad (width, x)
{
	var s = str x
	
	let gap = width - s.length
	
	if gap > 0 then
	{
		let space = " " * gap
		
		s = if x isa int then {space s} else {s space}
	}
	
	return s
}

def check (grid)
{
	let mapping = ... => ...
	
	if not (grid isa mapping) then
	{
		throw "non-mapping can't be a grid"
	}
	
	let (k, v) = *grid
	
	if not (v isa ...[]) then
	{
		throw "non-array value can't be a grid"
	}
	
	let dimensions (int[]) = k
	
	if not (dimensions isa u8[]) then
	{
		throw "grid dimensions must be non-negative integers, within reason"
	}
	
	let count = v.length
	
	if dimensions per Math.product != count then
	{
		throw "number of grid elements doesn't match the dimensions"
	}
}

def groups (size, units)
{
	var result = []
	
	var i = 0
	var j
	
	while i < units.length do
	{
		j = i + size
		
		result <-- " ".join *units[ i -> j ]
		
		i = j
	}
	
	return result
}

export
def display-grid (grid)
{
	check grid
	
	let dimensions (int[]) = grid.key
	
	let elements (str[]) = grid.value map str
	
	let max_width = elements map { str( v ).length } per Math.max
	
	let cells (str[]) = elements map pad % max_width
	
	let g (str[]) = groups( dimensions[ 0 ], cells )
	
	return "\n".join *g
}
