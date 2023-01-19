var primes (int[]) = [ 2, 3 ]

var last_index = 1

export
def generate
{
	var prime-numbers = _
	
	prime-numbers <== 2
	prime-numbers <== 3
	
	var x = 3
	
	while true do
	{
		x += 2  # after 2, primes are always odd
		
		var i = 0
		
		while x % primes[ i ] != 0 do
		{
			if (primes[ i ] ^ 2 > x) or (++i > last_index) then
			{
				prime-numbers <== x
				primes        <-- x
				
				++last_index
				
				break
			}
		}
	}
}
