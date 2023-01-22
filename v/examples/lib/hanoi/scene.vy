module scene

export
def make (depth, get-tower)
{

const padding = 0
const margin  = 2

const height = depth + 2

const min-disc-width = 5 + 2 * padding
const max-disc-width = min-disc-width + 2 * (depth - 1)
const pedestal-width = max-disc-width + 4
const spindle-margin = margin + 2 + depth + padding + 1

const spindle = spindle-margin => "|"

const pedestal-top    = margin + 1 => "_" * (pedestal-width - 2)
const pedestal-bottom = margin     => '|' ("_" * (pedestal-width - 2)) '|'

const max-width = margin + pedestal-bottom.value.length

const empty-space = " " * max-width

def composite (src, dst)
{
	const margin = src.key
	const image  = src.value
	
	const space = " " * margin
	
	return dst[ 0 -> margin ] image dst[ margin + image.length -> dst.length ]
}

def render-disc-top (i)
{
	const offset = margin + 3 + (depth - i)
	
	return offset => "_" * (2 * (i + padding) + 1)
}

def render-disc (i)
{
	const offset = margin + 2 + (depth - i)
	
	return offset => '(' ("_" * (2 * (i + padding) + 1)) ')'
}

def render-peg (i)
{
	var buffer = [ empty-space (*) height ]
	
	var row = height
	
	--row; buffer[ row ] = composite( pedestal-bottom, buffer[ row ] )
	--row; buffer[ row ] = composite( pedestal-top,    buffer[ row ] )
	
	for d in get-tower i do
	{
		buffer[ row ] = composite( render-disc d, buffer[ row ] )
		
		--row
		
		buffer[ row ] = composite( render-disc-top d, buffer[ row ] )
	}
	
	buffer[ row ] = composite( spindle, buffer[ row ] )
	
	while row > 0 do
	{
		--row
		
		buffer[ row ] = composite( spindle, buffer[ row ] )
	}
	
	return buffer
}

def display (send, extra)
{
	var pegs = 0 .. 2 map render-peg
	
	var rows = 0 -> height map { const r = v; 0 .. 2 map { pegs[ v ][ r ] } }
	
	rows = ["", "", "", *rows, ""]
	
	if extra then
	{
		const (n, src, dst) = *extra
		
		rows[ 0 ] =
		rows[ 1 ] = [ empty-space (*) 3 ]
		
		rows[ 0 ][ src ] = composite( render-disc-top n, empty-space )
		rows[ 1 ][ src ] = composite( render-disc     n, empty-space )
		
		send rows
		
		rows[ 0 ][ src ] <-> rows[ 0 ][ dst ]
		rows[ 1 ][ src ] <-> rows[ 1 ][ dst ]
	}
	
	send rows
}

return display

}  # make

end.


        _|_
1      (___)
2     (_____)
3    (_______)
4  _(_________)_
  |_____________|

