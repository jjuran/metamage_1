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
	"Legacynth\n"
	"\n"
	"Legacynth allows some older applications to play sound "
	"on newer Macintosh computers.\n"
	"\n"
	"Leave Legacynth running in the background "
	"to have sound work in those applications."
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
