let invalid-node = "invalid XML node"

let mapping = ... => ...

def qq (s) { str ('"', s, '"') }

let entities = byte^
[
	'"': "quot",
	'&': "amp",
	'<': "lt",
	'>': "gt",
]

def escaped (c: byte)
{
	if c in entities then
	{
		return "&" entities[ c ] ";"
	}
	
	let code = str int c
	
	return "&#" code ";"
}

def escape_text (text)
{
	let echars = text map { if v in "<&" then {escaped v} else {v} }
	
	return str echars
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

def render_at_depth (printer, node, depth)
{
	let print = printer % depth
	
	if node isa string then
	{
		print escape_text node
		return ()
	}
	
	if node isa mapping then
	{
		let (name, attrs, content) = get-name-attrs-content node
		
		let attr = str( attrs map { " " v.key, "=", qq v.value } )
		
		if content isa null then
		{
			print "<" name attr "/>"
		}
		else if content isa string then
		{
			let inner = escape_text content
			
			print "<" name attr ">" inner "</" name ">"
		}
		else
		{
			print "<" name attr ">"
			render_at_depth( printer, content, depth + 1 )
			print "</" name ">"
		}
		
		return ()
	}
	
	if node isa array then
	{
		for subnode in node do
		{
			render_at_depth( printer, subnode, depth )
		}
		
		return ()
	}
	
	throw invalid-node
}

export
def render_xml (printer, doc)
{
	render_at_depth( printer, doc, 0 )
}
