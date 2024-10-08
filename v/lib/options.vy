module options

let reference = 0
let interpret = 1

export
def parse (argv, defs, error)
{
	let argc = argv.length
	
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
			let c = *p++
			
			if not c in defs then
			{
				error()
			}
			
			let opt = defs[ c ]
			
			let optvar = opt[ reference ]
			
			if let f = opt[ interpret ] then
			{
				let param = ???
				
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
	
	let args = argv[ i -> argc ]
	
	return args
}
