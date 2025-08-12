#!/usr/bin/env lvx

let tests = DATA.lines() ver ... map { [v / " --> "] }

let n_tests = tests.length

put "1.." n_tests


def s (pattern) { pattern => str }

def grouped { { "(" _ ")" }^+((+) _ > 1) " ".join _ }

let SP = ' '

let digit = '0' .. '9'

let number = digit+ => str

var group

let factor = number | &group

let term = [factor, [SP*, s '*', SP*, factor]*] => grouped
let sum  = [term,   [SP*, s '+', SP*, term  ]*] => grouped

group = ['(', SP*, sum, SP*, ')']

let grammar = sum

def parse (text)
{
	var p = begin text
	
	return p += grammar
}

for i in 1 .. n_tests do
{
	let (input, output) = *tests[ i - 1 ]
	
	let ok = parse input == output
	
	put (("not " * not ok) "ok " i)
}

end.

((1)) --> 1

3 * 4 * 5 --> (3 * 4 * 5)
3 + 4 + 5 --> (3 + 4 + 5)
3 + 4 * 5 --> (3 + (4 * 5))
3 * 4 + 5 --> ((3 * 4) + 5)

1 + (3 + 4) * 2 --> (1 + ((3 + 4) * 2))
