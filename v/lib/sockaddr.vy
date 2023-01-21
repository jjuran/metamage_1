module sockaddr

export
def display (sa)
{
	const family = u8 (sa[ 1 ] or sa[ 0 ])
	
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
			const q = p
			
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
	
	const port = u8 sa[ 2 ] * 256 + u8 sa[ 3 ]
	
	return addr ":" port
}
