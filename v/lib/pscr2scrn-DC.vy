def unpack (input)
{
	let N = input.size
	
	let filler_table = input[   2 ->  10 ]
	let common_table = input[   9 -> 137 ]  # slot 0 is never used
	let picture_data = input[ 138 -> N   ]
	
	var output = x""
	
	var p = begin picture_data
	
	while p do
	{
		let x = i8 *p++
		
		if x < 0 then
		{
			let n = x mod         16 + 1
			let i = x mod 128 div 16
			
			output .= filler_table[[ i ]] * n
		}
		else if x > 0 then
		{
			output .= common_table[[ x ]]
		}
		else
		{
			output .= packed *p++
		}
	}
	
	return output
}
