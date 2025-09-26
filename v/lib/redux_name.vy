def drop (x)
{
	return lambda (s) { string (s / x) }
}

def repl (x)
{
	return lambda (s) { translated (s, x / 2) }
}

let mutations =
[
	drop '\'',
	drop "\u2019",  # typographer's apostrophe / closing single quote
	drop "\u00a1",  # inverted exclamation point
	drop '!',
	drop '.',
	repl " -",
	repl str ('A' .. 'Z', 'a' .. 'z'),
]

def redux_name (name)
{
	return mutations per name>-{ v L }
}
