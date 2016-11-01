def arcsign-seal
{
	const arcsign, const params = _
	
	const key   = params[ "key"   ]
	const hash  = params[ "hash"  ]
	const stamp = params[ "stamp" ]
	
	const date = stamp();
	
	for kit in params[ "kits" ] do
	{
		const load, const append = *kit.value
		
		const msg = load()
		
		append( arcsign.seal( key, msg, date, hash ) )
	}
	
	return []  # Any non-success would have thrown
}
