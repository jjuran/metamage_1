#!/usr/bin/env vx

const bindir = dirname realpath argv[ 0 ]
const libdir = bindir "/../../lib/"

const import = { const path = libdir _ ".vy"; eval( load path, path ) }

const date-time = import "date-time"

def recruit
{
	const connection = pipe()
	
	date-time.stamp time()
}

print "1..1"

recruit "Alice"
recruit "Bob"

print "ok 1"
