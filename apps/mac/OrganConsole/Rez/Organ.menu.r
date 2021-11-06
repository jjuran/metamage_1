#include "Apple-menu.r"

resource 'MENU' (129) {
	2,
	textMenuProc,
	0x7FFFFFF6,
	enabled,
	"File",
	{
		"New", noIcon, noKey, noMark, plain,
		"Open…", noIcon, "O", noMark, plain,
		"Close", noIcon, "W", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Quit", noIcon, "Q", noMark, plain
	}
};

resource 'MENU' (130) {
	3,
	textMenuProc,
	0x7FFFFFC0,
	enabled,
	"Edit",
	{
		"Undo", noIcon, "Z", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Cut", noIcon, "X", noMark, plain,
		"Copy", noIcon, "C", noMark, plain,
		"Paste", noIcon, "V", noMark, plain,
		"Clear", noIcon, noKey, noMark, plain,
	}
};
