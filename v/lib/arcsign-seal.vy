def arcsign-seal (arcsign, params)
{
	const key   = params[ "key"   ]
	const hash  = params[ "hash"  ]
	const stamp = params[ "stamp" ]
	
	const date = stamp();
	
	for kit in params[ "kits" ] do
	{
		const (load, append) = *kit.value
		
		const msg = load()
		
		append( arcsign.seal( key, msg, date, hash ) )
	}
	
	return []  # Any non-success would have thrown
}
