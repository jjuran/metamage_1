let space_char = ' ' | '\t' | '\n' | '\r'
let space = space_char*

let digit = '0' .. '9'
let upper = 'A' .. 'Z'
let lower = 'a' .. 'z'
let alpha = upper | lower
let alnum = alpha | digit

let tag_name = (alnum | '-')+
let attr_name = (alnum | '-')+
let attr_value = (alnum | '-')+  # when unquoted
let entity_name = alnum+

def until (q) { ~(byteclass q)* }

let separators = ["img", "link", "meta", "hr", "br"]

let entities = str^
[
	amp:  "&",
	apos: "'",
	gt:   ">",
	lt:   "<",
	bull: "\u2022",
	copy: "\u00a9",
]

let character_reference_body = ['#', digit+ => str * byte * int]

let entity_reference_body = [entity_name => { try {entities[ _ ]} catch {} }]

let reference_body = character_reference_body | entity_reference_body

let reference = ['&', reference_body, ';']

let print_text = reference | ~('<' | space_char) => str

let text_node = [print_text+, [space => str, print_text+]*] => str

def attr_list-pattern
{
	def delimited (q) { [q, [reference | ~byteclass(q) => str]* => str, q] }
	
	let value = delimited '\'' | delimited '"' | attr_value
	
	def kv (k, v) { k => v }
	
	def make_attr (match: ...) { kv (match (*) (3 - (+) match)) }
	
	let attr = [ attr_name => str, ['=', value]?, space ] => make_attr
	
	return (attr+ => {[_]}) | ("" => {[]})
}

let attr_list = attr_list-pattern()

let not-hyphen = ~(byteclass '-')

let comment_body = (not-hyphen+ | ['-', not-hyphen+])*

let comment = ["--", comment_body, "--", space]

let comments = comment+

def tag-pattern
{
	let metadata_body = (comments  => { comment: null })
	                  | (until '>' => { doctype: ""   })
	
	let proc_inst_body =
	[
		tag_name,
		space,
		attr_list,
	]
	=> { processing-instruction: null }
	
	let start_tag_body =
	[
		tag_name  => str,
		space,
		attr_list,
		'/'?,
	]
	=> { start-tag: [_] }
	
	let end_tag_body = [ '/', tag_name => { end-tag: _ }, space ]
	
	let tag_body = ['!', metadata_body]
	             | ['?', proc_inst_body, '?']
	             | start_tag_body
	             | end_tag_body
	
	return [ '<', tag_body, '>' ]
}

let lex_tag = tag-pattern()

export
def parse-html (data)
{
	var stack = [ ["DOC", null, [] ] ]
	
	def end (tag)
	{
		while stack and stack[ 0 ][ 0 ] != tag do
		{
			stack = [tail *stack]  # pop
		}
		
		assert bool stack
		
		let layer = stack[ 0 ]
		
		stack = [tail *stack]  # pop
		
		assert layer[ 0 ] == tag
		
		let attrs   = layer[ 1 ]
		let content = layer[ 2 ]
		
		var value = content
		
		if value isa array and value.length == 1 then
		{
			value = value[ 0 ]
		}
		
		if attrs then
		{
			value = attrs => value
		}
		
		assert stack[ 0 ][ 2 ] isa array
		
		stack[ 0 ][ 2 ] <-- tag => value
	}
	
	var p = begin data
	
	while (+) (var token = p += [space, text_node | lex_tag]) do
	{
		if token isa string then
		{
			if let p = (begin token).find ~space_char then
			{
				stack[ 0 ][ 2 ] <-- p.rest
			}
		}
		else if token.key == "start-tag" then
		{
			var value = token.value
			
			if value[ 0 ] in separators then
			{
				stack[ 0 ][ 2 ] <-- value[ 0 ] => value[ 1 ] => null
			}
			else
			{
				value <-- []
				
				stack = [value, *stack]  # push
			}
		}
		else if token.key == "end-tag" and not token.value in separators then
		{
			# Known separators don't get pushed onto the stack.
			# But <link ...></link> has been seen in the wild,
			# so don't try to unwind the stack for </link> when
			# <link> was never pushed in the first place.
			
			end token.value
		}
	}
	
	p += space
	
	if p then
	{
		throw "oops"
	}
	
	return stack[ 0 ][ 2 ][ 0 ]
}
