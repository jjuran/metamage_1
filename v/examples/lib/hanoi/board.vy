module board

export
def make (depth: int)
{
	var state = [ 1 .. depth map ..., [], [] ]
	
	def remove (n, src)
	{
		state[ src ] = [tail *state[ src ]]
	}
	
	def insert (n, dst)
	{
		state[ dst ] = [n, *state[ dst ]]
	}
	
	def move (n, src, dst)
	{
		state[ src ] = [tail *state[ src ]]
		state[ dst ] = [n,   *state[ dst ]]
	}
	
	def get (i)
	{
		return -state[ i ]  # bottom-up, e.g. [4, 3, 2, 1]
	}
	
	const methods = :get, :move, :remove, :insert
	
	return record methods
}
