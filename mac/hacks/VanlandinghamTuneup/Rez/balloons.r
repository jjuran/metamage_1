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
	"Vanlandingham Tune-up\n"
	"\n"
	"Vanlandingham is a bouncing ball demo for "
	"Macintosh computers with no color capability.\n"
	"\n"
	"This program makes Vanlandingham work on all Macintosh computers.\n"
	"\n"
	"To use this program, place it in the System Folder "
	"and restart the computer."
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
