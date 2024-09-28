module logic

def other-peg (src, dst)
{
	return 3 - src - dst
}

export
def make-tower-mover (move-disc)
{
	def move-tower (n, src, dst)
	{
		if n > 0 then
		{
			let mid = other-peg( src, dst )
			
			move-tower( n - 1, src, mid )
			
			move-disc _
			
			move-tower( n - 1, mid, dst )
		}
	}
	
	return move-tower
}
