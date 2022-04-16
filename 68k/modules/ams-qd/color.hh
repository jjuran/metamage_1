/*
	color.hh
	--------
*/

#ifndef COLOR_HH
#define COLOR_HH

enum
{
	Normal  = 1,
	Inverse = 2,
	
	B = 4,
	G = 8,
	R = 16,
	
	K = 32,
	Y = 64,
	M = 128,
	C = 256,
	
	CMY = C | M | Y,
	
	/*
	blackColor = Normal  | K,
	whiteColor = Inverse | R | G | B,
	
	cyanColor    = Normal | C | R,
	magentaColor = Normal | M | G,
	yellowColor  = Normal | Y | B,
	
	redColor   = magentaColor | yellowColor,
	greenColor = cyanColor    | yellowColor,
	blueColor  = cyanColor    | magentaColor,
	*/
};

inline
bool is_nongray( long color )
{
	return color & CMY;
}

#endif
