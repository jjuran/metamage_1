module info_plist

def qq { str ('"', _, '"') }

const encoding = qq "UTF-8"
const dtd_name = qq "-//Apple//DTD PLIST 1.0//EN"
const dtd_url  = qq "http://www.apple.com/DTDs/PropertyList-1.0.dtd"

export const version = "<?xml version=" (qq 1.0) " encoding=" encoding "?>"
export const doctype = "<!DOCTYPE plist PUBLIC " dtd_name " " dtd_url ">"

const mapping = ... => ...

const reifiers

def reify
{
	for r in reifiers do
	{
		if _ isa r.key then
		{
			return r.value( _ )
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
	mapping[] => { dict:   _ map { key: _.key, reify _.value } },
	array     => { array:  _ map reify },
]

export def convert (info)
{
	return plist: [ version: "1.0" ]: reify info
}
