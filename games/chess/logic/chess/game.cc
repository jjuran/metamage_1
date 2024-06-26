/*
	game.cc
	-------
*/

#include "chess/game.hh"


#pragma exceptions off


namespace chess
{

Type Game::type_at( Square addr ) const
{
	if ( Unit unit = board[ addr ] )
	{
		const Army& a = army[ unit < 0 ];
		
		Role role = role_of_unit( unit );
		
		return a.type[ role ];
	}
	
	return Type_none;
}

}
