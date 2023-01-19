var primes (int[]) = [ 2, 3 ]

var last_index = 1
var last_prime = 3

def next_prime
{
	var x = last_prime
	
	while true do
	{
		x += 2  # after 2, primes are always odd
		
		var i = 0
		
		while x % primes[ i ] != 0 do
		{
			if (primes[ i ] ^ 2 > x) or (++i > last_index) then
			{
				return x
			}
		}
	}
}

def nth_prime (n)
{
	while last_index < n do
	{
		last_prime = next_prime()
		++last_index
		
		primes <-- last_prime
	}
	
	return if n == last_index then { last_prime } else { primes[ n ] }
}

export nth_prime
