module sockaddr

export
def display (sa)
{
	let family = u8 (sa[ 1 ] or sa[ 0 ])
	
	var addr
	
	if family == 2 then
	{
		addr = ".".join *(sa[ 4 -> 8 ] map u8)
	}
	else if family == 30 then
	{
		addr = ":".join( hex sa[ 8 -> 24 ] / -4 )
		
		if var p = (begin addr).find "0000:0000" then
		{
			let q = p
			
			p += ["0000", ":0000"+]
			
			addr = (q.past or ":") (p.rest or ":")
		}
		
		if sa[ 8 -> 20 ] == (x"00" * 10) x"ffff" then
		{
			addr = "::ffff:" ".".join *(sa[ 20 -> 24 ] map u8)
		}
		
		addr = "[" addr "]"
	}
	else
	{
		return sa
	}
	
	let port = u8 sa[ 2 ] * 256 + u8 sa[ 3 ]
	
	return addr ":" port
}
