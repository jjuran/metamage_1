def readchar (fd) { *(fd.read 1 or "") }

def readline (fd, fail)
{
	var line = ""
	
	while true do
	{
		const c = readchar fd or return fail line
		
		# Postel's Law applies:  Accept LF in place of CRLF.
		
		if c == '\n' then
		{
			break
		}
		
		if c != '\r' then
		{
			line .= "" c
		}
	}
	
	return line
}
