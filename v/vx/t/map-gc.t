#!/usr/bin/env vx

print "1..1"

def make_sink
{
	var sink = []
	
	const append =
	{
		sink <-- 0  # "push() destination must be an array"
	}
	
	return append
}

var sinks = [0, 1] map make_sink

try
{
	sinks[ 0 ]()
	
	print "ok 1"
}
catch
{
	print "not ok 1"
}
