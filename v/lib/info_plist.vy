module info_plist

def qq (s) { str ('"', s, '"') }

let encoding = qq "UTF-8"
let dtd_name = qq "-//Apple//DTD PLIST 1.0//EN"
let dtd_url  = qq "http://www.apple.com/DTDs/PropertyList-1.0.dtd"

export let version = "<?xml version=" (qq 1.0) " encoding=" encoding "?>"
export let doctype = "<!DOCTYPE plist PUBLIC " dtd_name " " dtd_url ">"

let mapping = ... => ...

let reifiers = ???

def reify (info)
{
	for r in reifiers do
	{
		if info isa r.key then
		{
			return r.value( info )
		}
	}
	
	throw "unimplemented XML conversion input"
}

reifiers =
[
	bool      => { str(_): null  },
	string    => { string: _     },
	int       => { integer: str _ },
	fraction  => { real:   str float _ },
	float     => { real:   str _ },
	mb32      => { string: str _ },
	mapping[] => { dict:   _ map { key: v.key, reify v.value } },
	array     => { array:  _ map reify },
]

export def convert (info)
{
	return plist: [ version: "1.0" ]: reify info
}
