module arcsign

const ext_tag = "----- ARCSIGN EXTENSION -----"
const sig_tag = "----- ARCSIGN SIGNATURE -----"

const prefix = "\n" sig_tag "\n"

const pre_length = prefix.length
const key_length = 32 * 2
const sig_length = 64 * 2

const signature_block_length = pre_length
                             + key_length + 1
                             + sig_length + 2

export
const hash_functions = str^
[
	SHA-256: sha256,
]

def field
{
	const name, const value = _
	
	if value isa null then
	{
		return ""
	}
	
	return name ": " value "\n"
}

export
def seal
{
	const key, const msg, const date, const hash = _
	
	const f = if hash then { hash_functions[ hash ] } else { {_} }
	
	const ext = "\n" ext_tag "\n" field( "Date", date ) field( "Hash", hash )
	
	const pub = ed25519-publickey key
	const sig = ed25519-sign( key, f msg ext )
	
	return ext prefix "\n".join (pub, sig / 2, "")
}

def strict_unhex
{
	const result = unhex _
	
	if hex result != _ then
	{
		throw "malformed or non-canonical hexadecimal data"
	}
	
	return result
}

def parts_from_keysig
{
	const keysig = _
	
	assert keysig.length == signature_block_length
	
	if keysig[ 0 -> pre_length ] != prefix then
	{
		return ()
	}
	
	const hex_lines = keysig[ pre_length -> signature_block_length ].lines()
	
	const data = hex_lines map strict_unhex
	
	const key = data[ 0 ]
	const sig = packed data[ 1 .. 2 ]
	
	return key, sig
}

def message_parts
{
	const sealed_message = _
	
	var n = sealed_message.length
	
	var parts = ()
	
	if n >= signature_block_length then
	{
		const boundary = n - signature_block_length
		
		const msgext = sealed_message[ 0 -> boundary ]
		const keysig = sealed_message[ boundary -> n ]
		
		if const parts = parts_from_keysig keysig then
		{
			return msgext, parts  # msg ext, key, sig
		}
	}
	
	return ()
}

def msgext_parts
{
	const msgext = _
	
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
	
	const n = msgext.length
	
	const msg = msgext[ 0 -> i ]
	const ext = msgext[ j -> n ]
	
	return msg, ext
}

export
def validate
{
	if const parts = message_parts _ then
	{
		const msgext, const key, const sig = parts
		
		const msg, const ext = msgext_parts msgext
		
		var f = {_}
		
		if "Hash: SHA-256" in ext.lines() then
		{
			f = sha256
		}
		
		return ed25519-verify( key, f msgext, sig ) and [msg, ext, key]
	}
	
	return ()
}
