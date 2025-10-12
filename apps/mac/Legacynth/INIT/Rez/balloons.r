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
	"Legacynth Extension\n"
	"\n"
	"Legacynth allows some older applications to play sound "
	"on newer Macintosh computers.\n"
	"\n"
	"To use Legacynth, place this extension in the Extensions folder "
	"and restart the computer.  Then open the Legacynth application "
	"and click Start."
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
