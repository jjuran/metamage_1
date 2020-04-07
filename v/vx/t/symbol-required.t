#!/usr/bin/env vx

const cases =
[
	"const 3",
	"const 3 = 4",
	"3 = 4",
	"3, 4 = 5",
	"var x, var y; x + y = 5",
]

const n = cases.length

print "1.." n

var i = 0

while i < n do
{
	const ok = try { eval cases[ i ]; "not ok" } catch { "ok" }
	
	print ok " " ++i
}
