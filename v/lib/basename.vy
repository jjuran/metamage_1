export
def basename (path)
{
	var p = begin -path
	
	while *p == '/' do {++p}
	
	if let q = p.find '/' then
	{
		return -(q - p)
	}
	
	return -p.rest
}

assert basename "/foo/bar/" == "bar"
assert basename "/foo/bar" == "bar"
assert basename "foo/bar/" == "bar"
assert basename "foo/bar" == "bar"
assert basename "/bar/" == "bar"
assert basename "/bar" == "bar"
assert basename "bar/" == "bar"
assert basename "bar" == "bar"
