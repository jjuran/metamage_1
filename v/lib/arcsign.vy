module arcsign

let ext_tag = "----- ARCSIGN EXTENSION -----"
let sig_tag = "----- ARCSIGN SIGNATURE -----"

let prefix = "\n" sig_tag "\n"

let pre_length = prefix.length
let key_length = 32 * 2
let sig_length = 64 * 2

let signature_block_length = pre_length
                           + key_length + 1
                           + sig_length + 2

export
let hash_functions = str^
[
	SHA-256: sha256,
]

def field (name, value)
{
	if value isa null then
	{
		return ""
	}
	
	return name ": " value "\n"
}

export
def seal (key, msg, date, hash)
{
	let f = if hash then { hash_functions[ hash ] } else { ... }
	
	let ext = "\n" ext_tag "\n" field( "Date", date ) field( "Hash", hash )
	
	let pub = ed25519-publickey key
	let sig = ed25519-sign( key, f msg ext )
	
	return ext prefix "\n".join (pub, sig / 2, "")
}

def strict_unhex (hex_input)
{
	let result = unhex hex_input
	
	if hex result != hex_input then
	{
		throw "malformed or non-canonical hexadecimal data"
	}
	
	return result
}

def parts_from_keysig (keysig)
{
	assert keysig.length == signature_block_length
	
	if keysig[ 0 -> pre_length ] != prefix then
	{
		return ()
	}
	
	let hex_lines = keysig[ pre_length -> signature_block_length ].lines()
	
	let data = hex_lines map strict_unhex
	
	let key = data[ 0 ]
	let sig = packed data[ 1 .. 2 ]
	
	return key, sig
}

def message_parts (sealed_message)
{
	var n = sealed_message.length
	
	var parts = ()
	
	if n >= signature_block_length then
	{
		let boundary = n - signature_block_length
		
		let msgext = sealed_message[ 0 -> boundary ]
		let keysig = sealed_message[ boundary -> n ]
		
		if let parts = parts_from_keysig keysig then
		{
			return msgext, parts  # msg ext, key, sig
		}
	}
	
	return ()
}

def msgext_parts (msgext)
{
	var i = msgext.length
	
	if msgext[ --i ] != '\n' then
	{
		return ()
	}
	
	var j
	
	while true do
	{
		j = i
		
		while msgext[ --i ] != '\n' do
		{
		}
		
		if msgext[ i + 1 -> j ] == ext_tag then
		{
			break
		}
	}
	
	++j
	
	let n = msgext.length
	
	let msg = msgext[ 0 -> i ]
	let ext = msgext[ j -> n ]
	
	return msg, ext
}

export
def validate (sealed_message)
{
	if let parts = message_parts sealed_message then
	{
		let (msgext, key, sig) = parts
		
		let (msg, ext) = msgext_parts msgext
		
		var f = ...
		
		if "Hash: SHA-256" in ext.lines() then
		{
			f = sha256
		}
		
		return ed25519-verify( key, f msgext, sig ) and [msg, ext, key]
	}
	
	return ()
}
