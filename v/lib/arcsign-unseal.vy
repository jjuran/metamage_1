def arcsign-unseal
{
	const arcsign, const params = _
	
	const key = params[ "key" ]
	
	def unsealed
	{
		const load, const truncate = *_
		
		if var valid = arcsign.validate( load() ) then
		{
			const msg_key = valid[ 2 ]
			
			if msg_key != key then
			{
				return [ null, "UNRECOGNIZED KEY" ]
			}
			
			truncate( valid[ 0 ].length )
			
			return [ valid ]
		}
		
		if valid isa () then
		{
			return [ null, "not sealed" ]
		}
		
		return [ null, "INVALID SEAL" ]
	}
	
	return params[ "kits" ] map { v.key => unsealed v.value }
}
