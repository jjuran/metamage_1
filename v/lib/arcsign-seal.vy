def arcsign-seal (arcsign, params)
{
	let key   = params[ "key"   ]
	let hash  = params[ "hash"  ]
	let stamp = params[ "stamp" ]
	
	let date = stamp();
	
	for kit in params[ "kits" ] do
	{
		let (load, append) = *kit.value
		
		let msg = load()
		
		append( arcsign.seal( key, msg, date, hash ) )
	}
	
	return []  # Any non-success would have thrown
}
