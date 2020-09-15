const invalid-node = "invalid XML node"

const mapping = ... => ...

def qq { str ('"', _, '"') }

const entities = byte^
[
	'"': "quot",
	'&': "amp",
	'<': "lt",
	'>': "gt",
]

def escaped
{
	const c (byte)  = _
	
	if c in entities then
	{
		return "&" entities[ c ] ";"
	}
	
	const code = str int c
	
	return "&#" code ";"
}

def escape_text
{
	const text = _
	
	const echars = text map { if _ in "<&" then {escaped _} else {_} }
	
	return str echars
}

def get-name-attrs-content
{
	const node = _
	
	assert node isa mapping
	
	const name = node.key
	const data = node.value
	
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

def render_at_depth
{
	const printer, const node, const depth = _
	
	const print = printer % depth
	
	if node isa string then
	{
		print escape_text node
		return ()
	}
	
	if node isa mapping then
	{
		const name, const attrs, const content = get-name-attrs-content node
		
		const attr = str( attrs map { " " _.key, "=", qq _.value } )
		
		if content isa null then
		{
			print "<" name attr "/>"
		}
		else if content isa string then
		{
			const inner = escape_text content
			
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
def render_xml
{
	const printer, const doc = _
	
	render_at_depth( printer, doc, 0 )
}
