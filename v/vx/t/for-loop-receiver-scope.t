#!/usr/bin/env vx

print "1..1"

var threads = []
var writers = []

var i = 2

def make-thread (reader)
{
	return thread { reader.read 1 }
}

def make-pipe (reader, writer)
{
	threads <-- make-thread auto reader
	
	return auto writer
}

for w in receiver { if i-- then {make-pipe pipe()} } do
{
	writers <-- *w
}

print (("not " * (writers per {b - a} != 1)) "ok 1")
