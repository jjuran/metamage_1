def pad (width, x)
{
	var s = str x
	
	const gap = width - s.length
	
	if gap > 0 then
	{
		const space = " " * gap
		
		s = if x isa int then {space s} else {s space}
	}
	
	return s
}

def check
{
	const mapping = ... => ...
	
	if not (_ isa mapping) then
	{
		throw "non-mapping can't be a grid"
	}
	
	const k = _.key
	const v = _.value
	
	if not (v isa ...[]) then
	{
		throw "non-array value can't be a grid"
	}
	
	const dimensions (int[]) = k
	
	if not (dimensions isa u8[]) then
	{
		throw "grid dimensions must be non-negative integers, within reason"
	}
	
	const count = v.length
	
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
	
	const dimensions (int[]) = grid.key
	
	const elements (str[]) = grid.value map str
	
	const max_width = elements map { str( v ).length } per Math.max
	
	const cells (str[]) = elements map pad % max_width
	
	const g (str[]) = groups( dimensions[ 0 ], cells )
	
	return "\n".join *g
}
