def unpack (input)
{
	let N = input.size
	
	let filler_table = input[  2 -> 10 ]
	let picture_data = input[ 10 -> N  ]
	
	var output = x""
	
	var p = begin picture_data
	
	while p do
	{
		let x = i8 *p++
		
		if x < 0 then
		{
			let i = x mod         8
			let n = x mod 128 div 8 + 1
			
			output .= filler_table[[ i ]] * n
		}
		else if x >= 96 then
		{
			let n = x mod 4 * 256 + u8 *p++ + 1
			let i = x div 4 mod 8
			
			output .= filler_table[[ i ]] * n
		}
		else if x >= 64 then
		{
			let n = x mod 32 + 1
			
			output .= packed *p++ * n
		}
		else
		{
			let n = x + 1
			
			if n > p.rest.size then
			{
				return x""
			}
			
			output .= p.rest[ 0 -> n ]
			
			p += n
		}
	}
	
	return output
}
