let mapping = ... => ...

let digit = '0' .. '9'
let upper = 'A' .. 'Z'
let lower = 'a' .. 'z'
let alpha = upper | lower
let alnum = alpha | digit

def is-bareword (s)
{
	if s and s isa string and not '\0' in s then
	{
		return not (begin s).find ~(alnum | '_' | '-')
	}
	
	return false
}

assert not is-bareword ""
assert not is-bareword "\0"
assert not is-bareword "foo\x00bar"
assert not is-bareword "foo:bar"

assert is-bareword "foo-bar"

def level-depth (v: ...)
{
	if (+) v > 1 then
	{
		return level-depth [ v ]
	}
	
	if v isa array then
	{
		return Iter.max( v map level-depth ) + 1
	}
	
	if v isa mapping then
	{
		return Iter.max( [ v.key, v.value ] map level-depth )
	}
	
	return 0
}

def level-depth-at-least (depth, v)
{
	if depth == 0 then
	{
		return true
	}
	
	if (+) v > 1 then
	{
		return level-depth-at-least( depth, [ v ] )
	}
	
	if v and v isa array then
	{
		if v.length > 1 or level-depth-at-least( depth, *v ) then
		{
			return v map level-depth-at-least % (depth - 1)
		}
	}
	
	if v isa mapping then
	{
		return [ v.key, v.value ] map level-depth-at-least % depth
	}
	
	return ()
}

assert level-depth 123 == 0
assert level-depth (1, 2, 3) == 1
assert level-depth [1, 2, 3] == 1
assert level-depth [1, [2], 3] == 2
assert level-depth (foo: 1) == 0
assert level-depth (foo: [1]) == 1

def render_at_depth (printer, node, depth=0, maybe_comma="")
{
	let print = printer % depth
	
	if (+) node > 1 and level-depth-at-least( 2, node ) then
	{
		print "("
		
		for x in [ node ] do
		{
			render_at_depth( printer, x, depth + 1, "," )
		}
		
		print ")"
	}
	else if node isa array and node.length > 1 and level-depth-at-least( 1, node ) then
	{
		print "["
		
		for x in node do
		{
			render_at_depth( printer, x, depth + 1, "," )
		}
		
		print "]" maybe_comma
	}
	else if node isa mapping then
	{
		let make-key = if is-bareword node.key then {str} else {rep}
		
		let key = make-key node.key
		
		if not level-depth-at-least( 1, node ) then
		{
			print key ": " rep( node.value ) maybe_comma
		}
		else if not level-depth-at-least( 1, node.key ) then
		{
			print key ":"
			
			let indent = node.value isa mapping
			
			render_at_depth( printer, node.value, depth + +indent, maybe_comma )
		}
		else
		{
			render_at_depth( printer, node.key, depth, "" )
			
			print "=>"
			
			render_at_depth( printer, node.value, depth, maybe_comma )
		}
	}
	else
	{
		print rep( node ) maybe_comma
	}
}

export
def pretty-printing (const printer)
{
	return lambda (value)
	{
		render_at_depth( printer, value )
	}
}

end.

Example
-------

let pretty-printing = import "pretty-printing"

let Tab = "\t"

def printer (depth, text)
{
	let tabs = Tab * depth
	
	print tabs text
}

let pretty-print = pretty-printing printer

pretty-print ...
