#!/usr/bin/env vx

print "1..1"

{
	def make_thread
	{
		var n = 5
		
		var t = thread
		{
			while n do
			{
				--n
			}
			
			print "ok 1"
			
			return 0
		}
		
		return t
	}
	
	var th = make_thread()
}()
