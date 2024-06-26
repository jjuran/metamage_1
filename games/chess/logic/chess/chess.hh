/*
	chess.hh
	--------
*/

#ifndef CHESS_CHESS_HH
#define CHESS_CHESS_HH


namespace chess
{

/*
	 Chess notation:
	 
	 +----+----+----+----+----+----+----+----+
	 | a8 | b8 | c8 | d8 | e8 | f8 | g8 | h8 |
	 +----+----+----+----+----+----+----+----+
	 | a7 | b7 | c7 | d7 | e7 | f7 | g7 | h7 |
	 +----+----+----+----+----+----+----+----+
	 | a6 | b6 | c6 | d6 | e6 | f6 | g6 | h6 |
	 +----+----+----+----+----+----+----+----+
	 | a5 | b5 | c5 | d5 | e5 | f5 | g5 | h5 |
	 +----+----+----+----+----+----+----+----+
	 | a4 | b4 | c4 | d4 | e4 | f4 | g4 | h4 |
	 +----+----+----+----+----+----+----+----+
	 | a3 | b3 | c3 | d3 | e3 | f3 | g3 | h3 |
	 +----+----+----+----+----+----+----+----+
	 | a2 | b2 | c2 | d2 | e2 | f2 | g2 | h2 |
	 +----+----+----+----+----+----+----+----+
	 | a1 | b1 | c1 | d1 | e1 | f1 | g1 | h1 |
	 +----+----+----+----+----+----+----+----+
	 
	 Square numbers:
	 
	 +----+----+----+----+----+----+----+----+
	 | 57 | 58 | 59 | 60 | 61 | 62 | 63 | 64 |
	 +----+----+----+----+----+----+----+----+
	 | 49 | 50 | 51 | 52 | 53 | 54 | 55 | 56 |
	 +----+----+----+----+----+----+----+----+
	 | 41 | 42 | 43 | 44 | 45 | 46 | 47 | 48 |
	 +----+----+----+----+----+----+----+----+
	 | 33 | 34 | 35 | 36 | 37 | 38 | 39 | 40 |
	 +----+----+----+----+----+----+----+----+
	 | 25 | 26 | 27 | 28 | 29 | 30 | 31 | 32 |
	 +----+----+----+----+----+----+----+----+
	 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 |
	 +----+----+----+----+----+----+----+----+
	 |  9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
	 +----+----+----+----+----+----+----+----+
	 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |
	 +----+----+----+----+----+----+----+----+
	 
*/

enum square_t
{
	Square_none,
	
	a1, b1, c1, d1, e1, f1, g1, h1,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a8, b8, c8, d8, e8, f8, g8, h8,
	
	Removed = Square_none,
};

enum role_t
{
	Role_king,
	Role_queen,
	Role_bishop_1,
	Role_bishop_2,
	Role_knight_1,
	Role_knight_2,
	Role_rook_1,
	Role_rook_2,
	Role_pawn_1,
	// ...
	Role_pawn_8 = Role_pawn_1 + 7,
	
	Role_bishop = Role_bishop_1,
	Role_knight = Role_knight_1,
	Role_rook   = Role_rook_1,
	Role_pawn   = Role_pawn_1,
};

enum side_t
{
	White =  0x40,
	Black = -0x80,
};

enum unit_t
{
	Empty,
	
	Unit_white_king   = White | Role_king,
	// ...
	Unit_white_pawn_8 = White | Role_pawn_8,
	
	Unit_black_king   = Black | Role_king,
	// ...
	Unit_black_pawn_8 = Black | Role_pawn_8,
};

enum type_t
{
	Type_none,
	
	Type_pawn,
	Type_knight,
	Type_bishop,
	Type_rook,
	Type_queen,
	Type_king,
	Type_king_checkmated,
	Type_king_victorious,
};

typedef square_t Square;

typedef role_t Role;

#ifdef __MWERKS__

/*
	These definitions give us smaller 68K and PPC object code,
	and we still benefit from type safety in other builds.
*/

typedef unsigned char Type;
typedef   signed char Unit;

#else

typedef type_t Type;
typedef unit_t Unit;

#endif

inline
side_t side_of_unit( Unit unit )
{
	return side_t( unit & ~0x3f );
}

inline
Role role_of_unit( Unit unit )
{
	return Role( unit & 0x3f );
}

struct Game;

}

#endif
