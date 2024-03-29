def arcsign-unseal (arcsign, params)
{
	let key = params[ "key" ]
	
	def unsealed (load, truncate)
	{
		if var valid = arcsign.validate( load() ) then
		{
			let msg_key = valid[ 2 ]
			
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
	
	return params[ "kits" ] map { v.key => unsealed *v.value }
}
