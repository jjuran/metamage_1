#include "Apple-menu.r"

resource 'MENU' (129) {
	2,
	textMenuProc,
	0x7FFFFFF5,
	enabled,
	"File",
	{
		"New Game", noIcon, "N", noMark, plain,
		"Open�", noIcon, "O", noMark, plain,
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

resource 'MENU' (131) {
	4,
	textMenuProc,
	0x7FFFFFFF,
	enabled,
	"Options",
	{
		"Sound", noIcon, noKey, noMark, plain,
		"Full screen", noIcon, "F", noMark, plain,
	}
};
