module options

const reference = 0
const interpret = 1

export
def parse (argv, defs, error)
{
	const argc = argv.length
	
	var i = 0
	
	while ++i < argc do
	{
		var p = begin argv[ i ]
		
		if *p++ != '-' then
		{
			break
		}
		
		if *p == '-' then
		{
			# TODO:  long options
			++i
			break
		}
		
		while p do
		{
			const c = *p++
			
			if not c in defs then
			{
				error()
			}
			
			const opt = defs[ c ]
			
			const optvar = opt[ reference ]
			
			if const f = opt[ interpret ] then
			{
				const param
				
				if p then
				{
					param = p.rest
				}
				else
				{
					param = argv[ ++i ]
				}
				
				*optvar = f( param )
				
				break
			}
			else if *optvar isa int then
			{
				++*optvar
			}
		}
	}
	
	const args = argv[ i -> argc ]
	
	return args
}
