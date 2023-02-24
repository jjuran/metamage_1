let invalid-node = "invalid HTML node"

let mapping = ... => ...

let inline-level = ["a", "span"]

def qq (s) { '"' s '"' }

let entities = byte^
[
	'"': "quot",
	'&': "amp",
	'<': "lt",
	'>': "gt",
]

def escaped (c: byte)
{
	let entity =
		if c in entities
			then { entities[ c ] }
			else { "#" int c }
	
	return "&" entity ";"
}

def escape_text (text)
{
	return str (text map { escaped^+(v in "<&") v })
}

def escape_attr (text)
{
	return str (text map { escaped^+(v in "\"&") v })
}

def quote_attr (value)
{
	return qq escape_attr^+(value isa string) str value
}

def get-name-attrs-content (node)
{
	assert node isa mapping
	
	let name = node.key
	let data = node.value
	
	var attrs   = []
	var content = data
	
	if data isa mapping and data.key isa array then
	{
		# foo: [...]: bar
		
		attrs   = data.key
		content = data.value
	}
	
	return name, attrs, content
}

def is-inline-element (node)
{
	return node isa mapping and node.key in inline-level
}

def is-single-element (node)
{
	return node isa mapping and not [get-name-attrs-content node][ 2 ]
}

export
def render_html (printer, node, depth=0)
{
	let print = printer % depth
	
	if node isa string then
	{
		print escape_text node
	}
	else if node isa array then
	{
		for subnode in node do
		{
			render_html( printer, subnode, depth )
		}
	}
	else if node isa mapping then
	{
		let (name, attrs, content) = get-name-attrs-content node
		
		let attr = str (attrs map { " " v.key, "=", quote_attr v.value })
		
		if content isa null then
		{
			print "<" name attr ">"
		}
		else if content isa string then
		{
			let inner = escape_text content
			
			print "<" name attr ">" inner "</" name ">"
		}
		else if not content then
		{
			print "<" name attr "></" name ">"
		}
		else if name in inline-level or is-inline-element content then
		{
			var s = ""
			
			render_html( { s .= _ }, content )
			
			print "<" name attr ">" s "</" name ">"
		}
		else
		{
			print "<" name attr ">"
			render_html( printer, content, depth + 1 )
			print "</" name ">"
		}
	}
	else
	{
		throw invalid-node
	}
	
	return ()
}
