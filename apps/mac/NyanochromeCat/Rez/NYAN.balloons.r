#ifndef __BALLOONS_R__

#define hmBalloonHelpVersion  0x0002
#define hmDefaultOptions      0

#define HMSTRResItemID        7

type 'hfdr'
{
	integer;
	longint;
	integer;
	integer;
	integer = $$CountOf(HFdrArray);

	array HFdrArray
	{
		switch
		{
			case HMSTRResItem:
				integer = 6;               /* 6 bytes */
				key int = HMSTRResItemID;
				integer;
		};
	};
};

#endif

resource 'STR ' (128)
{
	"Nyanochrome Cat\n"
	"\n"
	"This is a parody of Chris Torres' Nyan Cat animation in the form of "
	"an application that runs on black-and-white Macintosh computers."
};

resource 'hfdr' (-5696)
{
	hmBalloonHelpVersion,
	hmDefaultOptions,
	0,  /* balloon procID  */
	0,  /* balloon variant */
	{
		HMSTRResItem { 128 }
	}
};
