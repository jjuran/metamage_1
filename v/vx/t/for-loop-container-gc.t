#!/usr/bin/env vx

print "1..1"

var lines = ()
var input = [ "DATA", "This is a test", ".", "QUIT", ] map { packed v "\n" }
var mark = 0
var cmds = str^[ DATA: { lines = [] }, QUIT: { break }, ]

def as-lines
{
	const buffer <- *_ = x""
	
	if var q = (begin buffer).find '\n' then
	{
		return q.past.string
	},
}

def recv-units (behead, src)
{
	var buffer = x""
	
	return receiver lambda
	{
		while true do
		{
			var x = behead &buffer
			
			if (+) x then
			{
				return x
			}
			
			buffer = buffer src[ mark++ ]
		}
	}
}

try
{
	for line in (recv-units % as-lines) input do
	{
		if ( lines isa () ) then
		{
			cmds[ line[ 0 -> 4 ] ] line
		}
		else if line != "." then
		{
			lines <-- line
		}
		else
		{
			{} lines
			
			lines = ()
		}
	}
	
	print "ok 1"
}
catch
{
	print "not ok 1"
}
