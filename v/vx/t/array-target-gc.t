#!/usr/bin/env vx

print "1..2"

def make_sink
{
	var sink = []
	
	const append =
	{
		sink <-- 0  # "push() destination must be an array"
	}
	
	return append
}

var sinks = [ 0 ]

sinks[ 0 ] = make_sink()

V.tracker.cull()

try
{
	sinks[ 0 ]()
	
	print "ok 1"
}
catch
{
	print "not ok 1"
}

sinks = [[ 0 ]]

sinks[ 0 ][ 0 ] = make_sink()

V.tracker.cull()

try
{
	sinks[ 0 ][ 0 ]()
	
	print "ok 2"
}
catch
{
	print "not ok 2"
}
