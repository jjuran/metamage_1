/*
	Splat_Bits.cc
	-------------
*/

#include "Chiaroscuro/Widget/Splat_Bits.hh"

// iota
#include "iota/bits.hh"

// Chiaroscuro
#include "Chiaroscuro/Types/BitMap.hh"


#define BIG16_9( _8, _7, _6, _5, _4, _3, _2, _1, _0 )  \
        BIG16  ( _8, _7, _6, _5, _4, _3, _2, _1,       \
                 _0, _ , _ , _ , _ , _ , _ , _ )

#define BIG16_7( _6, _5, _4, _3, _2, _1, _0 )  \
        BIG16_9( _6, _5, _4, _3, _2, _1, _0, _ , _ )


namespace Chiaroscuro
{

static const unsigned short document_window_splat_bits[] =
{
	BIG16_9( _,_,_,_,X,_,_,_,_ ),
	BIG16_9( _,X,_,_,X,_,_,X,_ ),
	BIG16_9( _,_,X,_,X,_,X,_,_ ),
	BIG16_9( _,_,_,_,_,_,_,_,_ ),
	BIG16_9( X,X,X,_,_,_,X,X,X ),
	BIG16_9( _,_,_,_,_,_,_,_,_ ),
	BIG16_9( _,_,X,_,X,_,X,_,_ ),
	BIG16_9( _,X,_,_,X,_,_,X,_ ),
	BIG16_9( _,_,_,_,X,_,_,_,_ ),
};

const BitMap document_window_splatBits =
{
	(char*) document_window_splat_bits,
	2,
	{ 0, 0, 9, 9 },
};

static const unsigned short utility_window_splat_bits[] =
{
	BIG16_7( _,_,_,X,_,_,_ ),
	BIG16_7( X,_,_,X,_,_,X ),
	BIG16_7( _,X,_,X,_,X,_ ),
	BIG16_7( _,_,_,_,_,_,_ ),
	BIG16_7( X,X,X,_,X,X,X ),
	BIG16_7( _,_,_,_,_,_,_ ),
	BIG16_7( _,X,_,X,_,X,_ ),
	BIG16_7( X,_,_,X,_,_,X ),
	BIG16_7( _,_,_,X,_,_,_ ),
};

const BitMap utility_window_splatBits =
{
	(char*) utility_window_splat_bits,
	2,
	{ 0, 0, 9, 7 },
};

}  // namespace Chiaroscuro
